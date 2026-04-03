#pragma once

#include "Fem.h"

// 网格类：负责生成网格、读取外部网格并导出 Tecplot 格式结果。
class Mesher {
public:
	// 单元集合。
	vector<Element> elements;
	// 节点集合。
	vector<Node> nodes;
	// 网格维度，2 表示二维，3 表示三维。
	int dim = 2;

	Mesher() {};

	// 根据输入参数生成或读取网格，并同步导出 Tecplot 网格文件。
	int Generate_Mesh(const struct ElementType& element_type, const struct Geom_RVE& geom_rve, const struct Grid_size& grid_size);
	
private:
	// 二维结构化网格在 x、y 方向上的步长。
	double dx, dy;
	// 生成二维结构化四边形网格。
	int Generate_qua_grids(const struct Geom_RVE& geom_rve, const struct Grid_size& grid_size);
	// 从外部 mesh_inp.dat 文件读取网格数据。
	int Read_mesh();
	// 将当前网格导出为 Tecplot 可读格式。
	int Export_mesh_data_tecplot();
	// 生成二维结构化三角形网格。
	int Generate_tri_grids(const struct Geom_RVE& geom_rve, const struct Grid_size& grid_size);
};
