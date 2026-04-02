#pragma once

#include<iomanip>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<memory>
#include"Matrix.h"
using namespace std;

//The parameter of material
struct Mat_para {
	string keywords;
	bool mark;
	double E;
	double v;
};

//The geometry of the RVE
struct Geom_RVE {
	string keywords;
	bool mark;
	double origin[3];
	double len_x, wid_y, hei_z;
};
// 单元类型
enum EleType{ Quad, Tria, Hexa8, Read_Mesh };
struct ElementType 
{
	string keywords;
	bool mark;
	EleType eletype;
};
//The size of grid
struct Grid_size {
	string keywords;
	bool mark;
	double delta_x, delta_y, delta_z;
};

//The load condition
struct Load {
	string keywords;
	bool mark;
	int num;
	vector<string> domain_type;
	vector<vector<double> > coef;
	vector<vector<string> > load_type;
	vector<vector<double> > value;
};

//The displacement condition
struct Displace {
	string keywords;
	bool mark;
	int num;
	vector<string> domain_type;
	vector<vector<double> > coef;
	vector<vector<string> > disp_type;
	vector<vector<double> > value;
};

class Input {
public:
	struct Mat_para mat_para;
	struct Geom_RVE geom_rve;
	struct Grid_size grid_size;
	struct ElementType element_type;
	struct Load load;
	struct Displace displace;

	Input() {};

	int Data_Initialization();
	int Read_Infile(ifstream& infile);
	string Get_Line(ifstream& infile) const;

private:
	int Read_geom_rve(struct Geom_RVE& geom_rve, ifstream& infile);
	int Read_grid_size(struct Grid_size& grid_size, ifstream& infile);
	int Read_mat_para(struct Mat_para& mat_para, ifstream& infile);
	int Read_load(struct Load& load, ifstream& infile);
	int Read_displacement(struct Displace& displace, ifstream& infile);
	int Read_element_type(struct ElementType& element_type, ifstream& infile);
};
