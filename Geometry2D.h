#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>
#include "Matrix.h"

// 二维点类：提供平面点坐标及距离计算。
class Point_2D {
public:
	double x;
	double y;

	// 构造二维点。
	Point_2D() {};
	Point_2D(double px, double py);

	// 计算当前点到另一个点对象的距离。
	double distance_to(const Point_2D& pt) const;
	// 计算当前点到指定坐标的距离。
	double distance_to(const double& px, const double& py) const;
};

// 二维线段类：提供线段长度与点在线段上的判定。
class Line_2D {
public:
	// 线段两个端点。
	Point_2D point[2];
	// 线段长度。
	double len;
	// 是否为有效线段，false 表示退化为点。
	bool virtual_line;

	// 构造二维线段。
	Line_2D() {};
	Line_2D(Point_2D p0, Point_2D p1);

	// 计算线段长度。
	double length();
	// 判断一个点对象是否在线段上。
	int contain(const Point_2D& point_temp) const;
	// 判断一个坐标点是否在线段上。
	int contain(const double& px, const double& py) const;

private:
	// 线段所在直线的一般式参数：A*x + B*y + D = 0。
	double A, B, D;
};
