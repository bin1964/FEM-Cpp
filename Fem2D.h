#pragma once

#include<iostream>
#include<cmath>
#include<vector>
using namespace std;

class Node {
public:
	double x, y, z;
	int type;	//0内点，1边界面点，2边界线点，3角点

	Node(const double ix = 0, const double iy = 0, const double iz = 0);
	Node(const double ix, const double iy, const double iz, const int itype);
	Node(const double ix, const double iy, const int itype);
};

class Element {
public:
	vector<int> nodes_id;
	int mat;

	Element() {};
};

//边线类头文件；
class Side
{
public:
	int type;	//表示边线的类型；0：内线段；1：力边线；2：自由边线；3：固定边线
	vector<int> nodes_id;
};
