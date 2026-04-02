#include "shape.h"
#include "Input.h"
#include "Mesher.h"

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

namespace {
void EvaluateHex8Shape(double xi, double eta, double zeta, double N[8], double dN_dxi[8], double dN_deta[8], double dN_dzeta[8]) {
	const double xi_sign[8] = {-1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0};
	const double eta_sign[8] = {-1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0};
	const double zeta_sign[8] = {-1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0};

	for (int i = 0; i < 8; ++i) {
		N[i] = 0.125 * (1.0 + xi * xi_sign[i]) * (1.0 + eta * eta_sign[i]) * (1.0 + zeta * zeta_sign[i]);
		dN_dxi[i] = 0.125 * xi_sign[i] * (1.0 + eta * eta_sign[i]) * (1.0 + zeta * zeta_sign[i]);
		dN_deta[i] = 0.125 * eta_sign[i] * (1.0 + xi * xi_sign[i]) * (1.0 + zeta * zeta_sign[i]);
		dN_dzeta[i] = 0.125 * zeta_sign[i] * (1.0 + xi * xi_sign[i]) * (1.0 + eta * eta_sign[i]);
	}
}
}

namespace ShapeSpace {
	Qua4::Qua4(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo) {
		Ele_Mat = ele_mat;
		Nod_Num = nod_num;
		Nod_Coo = nod_coo;
		Nod_Co2 = nod_coo;
		Ele_Stif = Matrix(8, 8);
		Ele_DMtr = Matrix(3, 3);
		Ele_BMtr = Matrix(3, 8);
		GaussW = Matrix(3, 1);
		GaussW << 5.0 / 9, 8.0 / 9, 5.0 / 9;
		GaussX = Matrix(3, 1);
		GaussX << -sqrt(15.0) / 5.0, 0.0, sqrt(15.0) / 5.0;
		Ele_Area = 0.0;
		detJacobi = 0.0;

		try {
			if (Ele_Mat.size() == 0 || Nod_Num.size() == 0 || Nod_Coo.size() == 0) {
				throw ElementException("failed to initialize quadrilateral element");
			}
		}
		catch (const ElementException& ex) {
			cout << ex.what() << endl;
		}
	}

	bool Qua4::CalcDMtr() {
		double E = Ele_Mat(0, 0);
		double v = Ele_Mat(0, 1);
		Ele_DMtr << 1, v, 0,
			v, 1, 0,
			0, 0, (1 - v) / 2;
		Ele_DMtr *= E / (1 - v * v);
		return true;
	}

	Matrix Qua4::CalcJacobi(Matrix x, Matrix y, Matrix& Ns, Matrix& Nt) {
		Matrix Jacobi(2, 2);
		Matrix a = Ns.transpose() * x;
		Matrix b = Ns.transpose() * y;
		Matrix c = Nt.transpose() * x;
		Matrix d = Nt.transpose() * y;
		Jacobi << a(0, 0), b(0, 0), c(0, 0), d(0, 0);
		return Jacobi;
	}

	bool Qua4::CalcBMtr(double s, double t) {
		Ele_BMtr = Matrix(3, 8);
		Matrix Nxy(2, 1);
		Matrix Ns(4, 1);
		Matrix Nt(4, 1);

		Ns << t - 1, -t + 1, t + 1, -t - 1;
		Nt << s - 1, -s - 1, s + 1, -s + 1;
		Ns *= 0.25;
		Nt *= 0.25;

		Matrix Jacobi = CalcJacobi(Nod_Coo.col(0), Nod_Coo.col(1), Ns, Nt);
		detJacobi = Jacobi.determinant();

		try {
			if (detJacobi < 1e-10) {
				throw ElementException("invalid Jacobian determinant");
			}
		}
		catch (const ElementException& ex) {
			cout << ex.what() << endl;
		}

		for (size_t i = 0; i < 4; ++i) {
			Nxy(0, 0) = Ns(i, 0);
			Nxy(1, 0) = Nt(i, 0);
			Nxy = Jacobi.inverse() * Nxy;

			Matrix block(3, 2);
			block << Nxy(0, 0), 0,
				0, Nxy(1, 0),
				Nxy(1, 0), Nxy(0, 0);
			Ele_BMtr.reblock(0, 2 * i, 3, 2, block);
		}
		return true;
	}

	bool Qua4::CalcStif() {
		Ele_Stif = Matrix(8, 8);
		CalcDMtr();
		for (size_t i = 0; i < 3; ++i) {
			for (size_t j = 0; j < 3; ++j) {
				CalcBMtr(GaussX(i, 0), GaussX(j, 0));
				Ele_Stif += GaussW(i, 0) * GaussW(j, 0) * detJacobi * Ele_BMtr.transpose() * Ele_DMtr * Ele_BMtr;
			}
		}
		return true;
	}

	bool Qua4::CalcLoad(Matrix load, Matrix node, Matrix& gF) {
		size_t i = 0;
		size_t j = 0;
		while (Nod_Num(0, i) != node(0, 0)) {
			++i;
		}
		while (Nod_Num(0, j) != node(1, 0)) {
			++j;
		}

		double l = sqrt(pow(Nod_Coo(i, 0) - Nod_Coo(j, 0), 2) + pow(Nod_Coo(i, 1) - Nod_Coo(j, 1), 2));
		for (int k = 0; k < 2; ++k) {
			gF(static_cast<size_t>((node(0, 0) - 1) * 2 + k), 0) += l * (load(0, k) / 3 + load(1, k) / 6);
			gF(static_cast<size_t>((node(1, 0) - 1) * 2 + k), 0) += l * (load(1, k) / 3 + load(0, k) / 6);
		}
		return true;
	}

	bool Qua4::AsseStif(Matrix& gK) {
		vector<int> ENDOF(2 * iNum);
		for (size_t j = 0; j < static_cast<size_t>(iNum); ++j) {
			ENDOF[2 * j] = static_cast<int>(2 * Nod_Num(0, j));
			ENDOF[2 * j + 1] = static_cast<int>(2 * Nod_Num(0, j) + 1);
		}
		for (size_t r = 0; r < ENDOF.size(); ++r) {
			for (size_t c = 0; c < ENDOF.size(); ++c) {
				gK(static_cast<size_t>(ENDOF[r]), static_cast<size_t>(ENDOF[c])) += Ele_Stif(r, c);
			}
		}
		return true;
	}

	bool Qua4::CalcCooAfter(Matrix& Disp) {
		Nod_Co2 = Nod_Coo;
		for (int i = 0; i < iNum; ++i) {
			for (int j = 0; j < 2; ++j) {
				Nod_Co2(i, j) += Disp(static_cast<size_t>(Nod_Num(0, i) * 2 + j), 0);
			}
		}
		return true;
	}

	Matrix Qua4::GetCooBefore() {
		return Nod_Coo;
	}

	Matrix Qua4::GetCooAfter() {
		return Nod_Co2;
	}

	Tri3::Tri3(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo) {
		Ele_Mat = ele_mat;
		Nod_Num = nod_num;
		Nod_Coo = nod_coo;
		Nod_Co2 = nod_coo;
		Ele_Area = 0.0;
		Ele_Stif = Matrix(6, 6);
		Ele_DMtr = Matrix(3, 3);
		Ele_BMtr = Matrix(3, 6);

		try {
			if (Ele_Mat.size() == 0 || Nod_Num.size() == 0 || Nod_Coo.size() == 0) {
				throw ElementException("failed to initialize triangle element");
			}
		}
		catch (const ElementException& ex) {
			cout << ex.what() << endl;
		}
	}

	bool Tri3::CalcDMtr() {
		double E = Ele_Mat(0, 0);
		double v = Ele_Mat(0, 1);
		Ele_DMtr << 1, v, 0,
			v, 1, 0,
			0, 0, (1 - v) / 2;
		Ele_DMtr *= E / (1 - v * v);
		return true;
	}

	bool Tri3::CalcBMtr() {
		double a[3];
		double b[3];
		double c[3];
		for (size_t i = 0; i < 3; ++i) {
			a[i] = Nod_Coo((i + 1) % 3, 0) * Nod_Coo((i + 2) % 3, 1) - Nod_Coo((i + 2) % 3, 0) * Nod_Coo((i + 1) % 3, 1);
			b[i] = Nod_Coo((i + 1) % 3, 1) - Nod_Coo((i + 2) % 3, 1);
			c[i] = Nod_Coo((i + 2) % 3, 0) - Nod_Coo((i + 1) % 3, 0);
		}

		double delta = a[0] + a[1] + a[2];
		Ele_Area = delta / 2;

		try {
			if (delta <= epsilon) {
				throw ElementException("triangle area is not positive");
			}
		}
		catch (const ElementException& ex) {
			cout << ex.what() << endl;
		}

		Ele_BMtr << b[0], 0, b[1], 0, b[2], 0,
			0, c[0], 0, c[1], 0, c[2],
			c[0], b[0], c[1], b[1], c[2], b[2];
		Ele_BMtr = (1 / delta) * Ele_BMtr;
		return true;
	}

	bool Tri3::CalcStif() {
		Ele_Stif = Matrix(6, 6);
		CalcDMtr();
		CalcBMtr();
		Ele_Stif = Ele_BMtr.transpose() * Ele_DMtr * Ele_BMtr * Ele_Area;
		return true;
	}

	bool Tri3::CalcLoad(Matrix load, Matrix node, Matrix& gF) {
		size_t i = 0;
		size_t j = 0;
		while (Nod_Num(0, i) != node(0, 0)) {
			++i;
		}
		while (Nod_Num(0, j) != node(1, 0)) {
			++j;
		}

		double l = sqrt(pow(Nod_Coo(i, 0) - Nod_Coo(j, 0), 2) + pow(Nod_Coo(i, 1) - Nod_Coo(j, 1), 2));
		for (int k = 0; k < 2; ++k) {
			gF(static_cast<size_t>((node(0, 0) - 1) * 2 + k), 0) += l * (load(0, k) / 3 + load(1, k) / 6);
			gF(static_cast<size_t>((node(1, 0) - 1) * 2 + k), 0) += l * (load(1, k) / 3 + load(0, k) / 6);
		}
		return true;
	}

	bool Tri3::AsseStif(Matrix& gK) {
		vector<int> ENDOF(2 * iNum);
		for (size_t j = 0; j < static_cast<size_t>(iNum); ++j) {
			ENDOF[2 * j] = static_cast<int>(2 * Nod_Num(0, j));
			ENDOF[2 * j + 1] = static_cast<int>(2 * Nod_Num(0, j) + 1);
		}
		for (size_t r = 0; r < ENDOF.size(); ++r) {
			for (size_t c = 0; c < ENDOF.size(); ++c) {
				gK(static_cast<size_t>(ENDOF[r]), static_cast<size_t>(ENDOF[c])) += Ele_Stif(r, c);
			}
		}
		return true;
	}

	bool Tri3::CalcCooAfter(Matrix& Disp) {
		Nod_Co2 = Nod_Coo;
		for (size_t i = 0; i < static_cast<size_t>(iNum); ++i) {
			for (size_t j = 0; j < 2; ++j) {
				size_t irow = static_cast<size_t>(Nod_Num(0, i) * 2 + j);
				Nod_Co2(i, j) += Disp(irow, 0);
			}
		}
		return true;
	}

	Matrix Tri3::GetCooBefore() {
		return Nod_Coo;
	}

	Matrix Tri3::GetCooAfter() {
		return Nod_Co2;
	}

	Hex8::Hex8(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo) {
		Ele_Mat = ele_mat;
		Nod_Num = nod_num;
		Nod_Coo = nod_coo;
		Nod_Co2 = nod_coo;
		Ele_Stif = Matrix(24, 24);
		Ele_DMtr = Matrix(6, 6);
		Ele_BMtr = Matrix(6, 24);
		GaussW = Matrix(2, 1);
		GaussW << 1.0, 1.0;
		GaussX = Matrix(2, 1);
		const double gauss_point = 1.0 / sqrt(3.0);
		GaussX << -gauss_point, gauss_point;
		detJacobi = 0.0;

		try {
			if (Ele_Mat.size() == 0 || Nod_Num.size() == 0 || Nod_Coo.size() == 0) {
				throw ElementException("failed to initialize hexahedral element");
			}
		}
		catch (const ElementException& ex) {
			cout << ex.what() << endl;
		}
	}

	bool Hex8::CalcDMtr() {
		double E = Ele_Mat(0, 0);
		double v = Ele_Mat(0, 1);
		const double factor = E / ((1 + v) * (1 - 2 * v));
		Ele_DMtr << 1 - v, v, v, 0, 0, 0,
			v, 1 - v, v, 0, 0, 0,
			v, v, 1 - v, 0, 0, 0,
			0, 0, 0, (1 - 2 * v) / 2, 0, 0,
			0, 0, 0, 0, (1 - 2 * v) / 2, 0,
			0, 0, 0, 0, 0, (1 - 2 * v) / 2;
		Ele_DMtr *= factor;
		return true;
	}

	bool Hex8::CalcBMtr(double xi, double eta, double zeta) {
		Ele_BMtr = Matrix(6, 24);
		double N[8];
		double dN_dxi[8];
		double dN_deta[8];
		double dN_dzeta[8];
		EvaluateHex8Shape(xi, eta, zeta, N, dN_dxi, dN_deta, dN_dzeta);

		Matrix Jacobi(3, 3);
		for (int i = 0; i < 8; ++i) {
			Jacobi(0, 0) += dN_dxi[i] * Nod_Coo(i, 0);
			Jacobi(0, 1) += dN_deta[i] * Nod_Coo(i, 0);
			Jacobi(0, 2) += dN_dzeta[i] * Nod_Coo(i, 0);
			Jacobi(1, 0) += dN_dxi[i] * Nod_Coo(i, 1);
			Jacobi(1, 1) += dN_deta[i] * Nod_Coo(i, 1);
			Jacobi(1, 2) += dN_dzeta[i] * Nod_Coo(i, 1);
			Jacobi(2, 0) += dN_dxi[i] * Nod_Coo(i, 2);
			Jacobi(2, 1) += dN_deta[i] * Nod_Coo(i, 2);
			Jacobi(2, 2) += dN_dzeta[i] * Nod_Coo(i, 2);
		}

		detJacobi = Jacobi.determinant();
		try {
			if (detJacobi < 1e-10) {
				throw ElementException("invalid Jacobian determinant");
			}
		}
		catch (const ElementException& ex) {
			cout << ex.what() << endl;
		}

		Matrix invJacobi = Jacobi.inverse();
		for (int i = 0; i < 8; ++i) {
			Matrix grad_nat(3, 1);
			grad_nat << dN_dxi[i], dN_deta[i], dN_dzeta[i];
			Matrix grad_xyz = invJacobi * grad_nat;

			const double dNdx = grad_xyz(0, 0);
			const double dNdy = grad_xyz(1, 0);
			const double dNdz = grad_xyz(2, 0);

			Matrix block(6, 3);
			block << dNdx, 0, 0,
				0, dNdy, 0,
				0, 0, dNdz,
				dNdy, dNdx, 0,
				0, dNdz, dNdy,
				dNdz, 0, dNdx;
			Ele_BMtr.reblock(0, static_cast<size_t>(3 * i), 6, 3, block);
		}
		return true;
	}

	bool Hex8::CalcStif() {
		Ele_Stif = Matrix(24, 24);
		CalcDMtr();
		for (size_t i = 0; i < 2; ++i) {
			for (size_t j = 0; j < 2; ++j) {
				for (size_t k = 0; k < 2; ++k) {
					CalcBMtr(GaussX(i, 0), GaussX(j, 0), GaussX(k, 0));
					Ele_Stif += GaussW(i, 0) * GaussW(j, 0) * GaussW(k, 0) * detJacobi * Ele_BMtr.transpose() * Ele_DMtr * Ele_BMtr;
				}
			}
		}
		return true;
	}

	bool Hex8::CalcLoad(Matrix, Matrix, Matrix&) {
		return true;
	}

	bool Hex8::AsseStif(Matrix& gK) {
		vector<int> ENDOF(3 * iNum);
		for (size_t j = 0; j < static_cast<size_t>(iNum); ++j) {
			ENDOF[3 * j] = static_cast<int>(3 * Nod_Num(0, j));
			ENDOF[3 * j + 1] = static_cast<int>(3 * Nod_Num(0, j) + 1);
			ENDOF[3 * j + 2] = static_cast<int>(3 * Nod_Num(0, j) + 2);
		}
		for (size_t r = 0; r < ENDOF.size(); ++r) {
			for (size_t c = 0; c < ENDOF.size(); ++c) {
				gK(static_cast<size_t>(ENDOF[r]), static_cast<size_t>(ENDOF[c])) += Ele_Stif(r, c);
			}
		}
		return true;
	}

	bool Hex8::CalcCooAfter(Matrix& Disp) {
		Nod_Co2 = Nod_Coo;
		for (int i = 0; i < iNum; ++i) {
			for (int j = 0; j < 3; ++j) {
				Nod_Co2(i, j) += Disp(static_cast<size_t>(Nod_Num(0, i) * 3 + j), 0);
			}
		}
		return true;
	}

	Matrix Hex8::GetCooBefore() {
		return Nod_Coo;
	}

	Matrix Hex8::GetCooAfter() {
		return Nod_Co2;
	}

	int Generate_Element_Classes(vector<unique_ptr<ShapeInterface>>& shape, const Mat_para& mat_para, const Mesher& mesh_data) {
		double E = mat_para.E;
		double v = mat_para.v;
		Matrix e_mat(1, 2);
		e_mat << E, v;

		int eNums = static_cast<int>(mesh_data.elements.size());
		int iNum = 0;
		for (int i = 0; i < eNums; ++i) {
			vector<int> inodes_id = mesh_data.elements[i].nodes_id;
			int currentNodeCount = static_cast<int>(inodes_id.size());

			if (i == 0) {
				iNum = currentNodeCount;
			}
			else if (currentNodeCount != iNum) {
				cout << "Error: inconsistent node count across elements." << endl;
				return 0;
			}

			vector<vector<double>> nc_temps;
			for (int j = 0; j < currentNodeCount; ++j) {
				vector<double> nc_temp;
				nc_temp.push_back(mesh_data.nodes[inodes_id[j]].x);
				nc_temp.push_back(mesh_data.nodes[inodes_id[j]].y);
				if (currentNodeCount == 8) {
					nc_temp.push_back(mesh_data.nodes[inodes_id[j]].z);
				}
				nc_temps.push_back(nc_temp);
			}

			Matrix e_num(inodes_id);
			Matrix e_coo(nc_temps);

			if (currentNodeCount == 4) {
				shape.emplace_back(make_unique<Qua4>(e_mat, e_num, e_coo));
			}
			else if (currentNodeCount == 3) {
				shape.emplace_back(make_unique<Tri3>(e_mat, e_num, e_coo));
			}
			else if (currentNodeCount == 8) {
				shape.emplace_back(make_unique<Hex8>(e_mat, e_num, e_coo));
			}
			else {
				cout << "Error: unsupported node count " << currentNodeCount << endl;
				return 0;
			}
		}

		cout << "Element classes generated." << endl;
		return 1;
	}
}
