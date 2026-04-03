#pragma once

#include <memory>
#include <vector>
#include "Matrix.h"
#include "FEMException.h"

struct Mat_para;
class Mesher;

namespace ShapeSpace {
	// 单元接口：统一抽象不同类型单元的刚度、载荷和位移后坐标计算。
	class ShapeInterface {
	public:
		// 计算单元本构矩阵。
		virtual bool CalcDMtr() = 0;
		// 计算单元刚度矩阵。
		virtual bool CalcStif() = 0;
		// 计算并装配单元等效载荷。
		virtual bool CalcLoad(Matrix load, Matrix node, Matrix& gF) = 0;
		// 将单元刚度装配进总体刚度矩阵。
		virtual bool AsseStif(Matrix& gK) = 0;
		// 根据位移结果计算变形后坐标。
		virtual bool CalcCooAfter(Matrix& Disp) = 0;
		// 获取变形前坐标。
		virtual Matrix GetCooBefore() = 0;
		// 获取变形后坐标。
		virtual Matrix GetCooAfter() = 0;
		virtual ~ShapeInterface() = default;
	};

	// 四节点四边形单元。
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
		// 单元节点数。
		const int iNum = 4;

		// 构造四边形单元对象。
		Qua4(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo);
		// 计算二维平面问题本构矩阵。
		bool CalcDMtr() override;
		// 在给定积分点计算 B 矩阵。
		bool CalcBMtr(double s, double t);
		// 计算单元刚度矩阵。
		bool CalcStif() override;
		// 计算雅可比矩阵。
		Matrix CalcJacobi(Matrix x, Matrix y, Matrix& Ns, Matrix& Nt);
		// 计算边界载荷等效节点力。
		bool CalcLoad(Matrix load, Matrix node, Matrix& gF) override;
		// 装配单元刚度矩阵。
		bool AsseStif(Matrix& gK) override;
		// 计算变形后坐标。
		bool CalcCooAfter(Matrix& Disp) override;
		// 获取变形前坐标。
		Matrix GetCooBefore() override;
		// 获取变形后坐标。
		Matrix GetCooAfter() override;
	};

	// 三节点三角形单元。
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
		// 单元节点数。
		const int iNum = 3;

		// 构造三角形单元对象。
		Tri3(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo);
		// 计算二维平面问题本构矩阵。
		bool CalcDMtr() override;
		// 计算常应变三角形单元的 B 矩阵。
		bool CalcBMtr();
		// 计算单元刚度矩阵。
		bool CalcStif() override;
		// 计算边界载荷等效节点力。
		bool CalcLoad(Matrix load, Matrix node, Matrix& gF) override;
		// 装配单元刚度矩阵。
		bool AsseStif(Matrix& gK) override;
		// 计算变形后坐标。
		bool CalcCooAfter(Matrix& Disp) override;
		// 获取变形前坐标。
		Matrix GetCooBefore() override;
		// 获取变形后坐标。
		Matrix GetCooAfter() override;
	};

	// 八节点六面体单元。
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
		// 单元节点数。
		const int iNum = 8;

		// 构造六面体单元对象。
		Hex8(Matrix& ele_mat, Matrix& nod_num, Matrix& nod_coo);
		// 计算三维弹性本构矩阵。
		bool CalcDMtr() override;
		// 在给定积分点计算三维 B 矩阵。
		bool CalcBMtr(double xi, double eta, double zeta);
		// 计算单元刚度矩阵。
		bool CalcStif() override;
		// 计算单元载荷等效节点力。
		bool CalcLoad(Matrix load, Matrix node, Matrix& gF) override;
		// 装配单元刚度矩阵。
		bool AsseStif(Matrix& gK) override;
		// 计算变形后坐标。
		bool CalcCooAfter(Matrix& Disp) override;
		// 获取变形前坐标。
		Matrix GetCooBefore() override;
		// 获取变形后坐标。
		Matrix GetCooAfter() override;
	};

	// 根据网格信息批量创建单元对象。
	int Generate_Element_Classes(std::vector<std::unique_ptr<ShapeInterface>>& shape, const Mat_para& mat_para, const Mesher& mesh_data);
}
