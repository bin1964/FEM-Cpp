#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "Matrix.h"

using namespace std;

// 材料参数：当前程序主要使用弹性模量和泊松比。
struct Mat_para {
	string keywords;
	bool mark;
	double E;
	double v;
};

// 几何参数：统一描述二维/三维 RVE 的原点与尺寸。
struct Geom_RVE {
	string keywords;
	bool mark;
	double origin[3];
	double len_x, wid_y, hei_z;
};

// 单元类型枚举：用于控制网格生成或外部网格读取逻辑。
enum EleType { Quad, Tria, Hexa8, Read_Mesh };

// 单元类型设置。
struct ElementType
{
	string keywords;
	bool mark;
	EleType eletype;
};

// 网格尺寸：二维使用 dx、dy，三维额外使用 dz。
struct Grid_size {
	string keywords;
	bool mark;
	double delta_x, delta_y, delta_z;
};

// 载荷条件：保存载荷作用域、作用类型与数值。
struct Load {
	string keywords;
	bool mark;
	int num;
	vector<string> domain_type;
	vector<vector<double>> coef;
	vector<vector<string>> load_type;
	vector<vector<double>> value;
};

// 位移约束条件：保存约束区域、约束分量与目标值。
struct Displace {
	string keywords;
	bool mark;
	int num;
	vector<string> domain_type;
	vector<vector<double>> coef;
	vector<vector<string>> disp_type;
	vector<vector<double>> value;
};

// 输入类：负责输入文件初始化、逐段解析与数据存储。
class Input {
public:
	// 材料参数。
	struct Mat_para mat_para;
	// 几何参数。
	struct Geom_RVE geom_rve;
	// 网格步长参数。
	struct Grid_size grid_size;
	// 单元类型参数。
	struct ElementType element_type;
	// 载荷参数。
	struct Load load;
	// 位移边界条件参数。
	struct Displace displace;

	Input() {};

	// 初始化所有输入字段的默认值。
	int Data_Initialization();
	// 按关键字分段读取输入文件。
	int Read_Infile(ifstream& infile);
	// 读取一行有效内容，并跳过以 % 开头的注释行。
	string Get_Line(ifstream& infile) const;

private:
	// 读取几何参数段。
	int Read_geom_rve(struct Geom_RVE& geom_rve, ifstream& infile);
	// 读取网格尺寸段。
	int Read_grid_size(struct Grid_size& grid_size, ifstream& infile);
	// 读取材料参数段。
	int Read_mat_para(struct Mat_para& mat_para, ifstream& infile);
	// 读取载荷段。
	int Read_load(struct Load& load, ifstream& infile);
	// 读取位移约束段。
	int Read_displacement(struct Displace& displace, ifstream& infile);
	// 读取单元类型段。
	int Read_element_type(struct ElementType& element_type, ifstream& infile);
};
