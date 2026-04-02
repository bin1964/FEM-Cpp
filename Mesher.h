#pragma once

#include "Fem2D.h"

class Mesher {
public:
	vector<Element> elements;
	vector<Node> nodes;
	int dim = 2;

	Mesher() {};

	//生成网格
	int Generate_Mesh(const struct ElementType& element_type, const struct Geom_RVE& geom_rve, const struct Grid_size& grid_size);
	
private:
	double dx, dy;
	//划分四边形网格
	int Generate_qua_grids(const struct Geom_RVE& geom_rve, const struct Grid_size& grid_size);
	//读取网格数据
	int Read_mesh();
	//将离散数据输出为tecplot格式
	int Export_mesh_data_tecplot();
	//划分三角形网格
	int Generate_tri_grids(const struct Geom_RVE& geom_rve, const struct Grid_size& grid_size);
};
