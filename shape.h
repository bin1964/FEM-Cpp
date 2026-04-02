#pragma once

#include <memory>
#include <vector>
#include "Matrix.h"
#include "FEMException.h"

struct Mat_para;
class Mesher;

namespace ShapeSpace {
	class ShapeInterface {
	public:
		virtual bool CalcDMtr() = 0;
		virtual bool CalcStif() = 0;
		virtual bool CalcLoad(Matrix load, Matrix node, Matrix& gF) = 0;
		virtual bool AsseStif(Matrix& gK) = 0;
		virtual bool CalcCooAfter(Matrix& Disp) = 0;
		virtual Matrix GetCooBefore() = 0;
		virtual Matrix GetCooAfter() = 0;
		virtual ~ShapeInterface() = default;
	};

	class Qua4 : public ShapeInterface {
	private:
		Matrix Ele_Mat;
		Matrix Nod_Num;
		Matrix Nod_Coo;
		Matrix Nod_Co2;
		Matrix Ele_Stif;
		Matrix Ele_DMtr;
		Matrix Ele_BMtr;
		Matrix GaussW;
		Matrix GaussX;
		double Ele_Area;
		double detJacobi;

	public:
		const int iNum = 4;

		Qua4(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo);
		bool CalcDMtr() override;
		bool CalcBMtr(double s, double t);
		bool CalcStif() override;
		Matrix CalcJacobi(Matrix x, Matrix y, Matrix& Ns, Matrix& Nt);
		bool CalcLoad(Matrix load, Matrix node, Matrix& gF) override;
		bool AsseStif(Matrix& gK) override;
		bool CalcCooAfter(Matrix& Disp) override;
		Matrix GetCooBefore() override;
		Matrix GetCooAfter() override;
	};

	class Tri3 : public ShapeInterface {
	private:
		Matrix Ele_Mat;
		Matrix Nod_Num;
		Matrix Nod_Coo;
		Matrix Nod_Co2;
		Matrix Ele_Stif;
		Matrix Ele_DMtr;
		Matrix Ele_BMtr;
		double Ele_Area;

	public:
		const int iNum = 3;

		Tri3(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo);
		bool CalcDMtr() override;
		bool CalcBMtr();
		bool CalcStif() override;
		bool CalcLoad(Matrix load, Matrix node, Matrix& gF) override;
		bool AsseStif(Matrix& gK) override;
		bool CalcCooAfter(Matrix& Disp) override;
		Matrix GetCooBefore() override;
		Matrix GetCooAfter() override;
	};

	class Hex8 : public ShapeInterface {
	private:
		Matrix Ele_Mat;
		Matrix Nod_Num;
		Matrix Nod_Coo;
		Matrix Nod_Co2;
		Matrix Ele_Stif;
		Matrix Ele_DMtr;
		Matrix Ele_BMtr;
		Matrix GaussW;
		Matrix GaussX;
		double detJacobi;

	public:
		const int iNum = 8;

		Hex8(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo);
		bool CalcDMtr() override;
		bool CalcBMtr(double xi, double eta, double zeta);
		bool CalcStif() override;
		bool CalcLoad(Matrix load, Matrix node, Matrix& gF) override;
		bool AsseStif(Matrix& gK) override;
		bool CalcCooAfter(Matrix& Disp) override;
		Matrix GetCooBefore() override;
		Matrix GetCooAfter() override;
	};

	int Generate_Element_Classes(std::vector<std::unique_ptr<ShapeInterface>>& shape, const Mat_para& mat_para, const Mesher& mesh_data);
}
