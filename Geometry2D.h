#pragma once
#include <cmath>
#include <stdlib.h>
#include <vector>
#include "Matrix.h"

//---------------------------------------------------------------------------
//二维点类
class Point_2D {
public:
	double x;
	double y;

	//构造函数
	Point_2D() {};
	Point_2D(double px, double py);

	//成员函数
	double distance_to(const Point_2D& pt)const;
	double distance_to(const double& px, const double& py)const;
};
//-----------------------------------------------------------------------------
//二维线段类
class Line_2D {
public:
	Point_2D point[2];		//线段的两个端点
	double len;				//线段长度
	bool virtual_line;		//线段是否为虚线，false:实线，true:虚线

	//构造函数
	Line_2D() {};
	Line_2D(Point_2D p0, Point_2D p1);

	//成员函数
	double length();	//线段长度
	int contain(const Point_2D& point_temp)const;    //判断线段是否包含一个点
	int contain(const double& px, const double& py)const;    //判断线段是否包含一个点

private:
	double A, B, D;
};