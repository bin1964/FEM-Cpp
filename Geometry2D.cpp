#include "Geometry2D.h"

// 构造二维点对象。
Point_2D::Point_2D(double px, double py)
{
	x = px;
	y = py;
}

// 计算当前点到另一个点对象的欧氏距离。
double Point_2D::distance_to(const Point_2D& pt) const
{
	double rv2 = (x - pt.x) * (x - pt.x) + (y - pt.y) * (y - pt.y);
	return sqrt(rv2);
}

// 计算当前点到指定坐标点的欧氏距离。
double Point_2D::distance_to(const double& px, const double& py) const
{
	double rv2 = (x - px) * (x - px) + (y - py) * (y - py);
	return sqrt(rv2);
}

// 构造二维线段，并预先保存长度和直线方程参数。
Line_2D::Line_2D(Point_2D p0, Point_2D p1) {
	point[0] = p0;
	point[1] = p1;
	A = p1.y - p0.y;
	B = -(p1.x - p0.x);
	D = -(A * p0.x + B * p0.y);
	len = length();
	virtual_line = (len != 0);
}

// 计算线段长度。
double Line_2D::length() {
	return sqrt((point[1].x - point[0].x) * (point[1].x - point[0].x) + (point[1].y - point[0].y) * (point[1].y - point[0].y));
}

// 判断给定点对象是否位于线段上。
int Line_2D::contain(const Point_2D& point_temp) const {
	if (fabs(point_temp.distance_to(point[0]) + point_temp.distance_to(point[1]) - point[0].distance_to(point[1])) > 1e-8) {
		return 0;
	}
	return 1;
}

// 判断给定坐标点是否位于线段上。
int Line_2D::contain(const double& px, const double& py) const {
	Point_2D point_temp(px, py);
	if (fabs(point_temp.distance_to(point[0]) + point_temp.distance_to(point[1]) - point[0].distance_to(point[1])) > 1e-8) {
		return 0;
	}
	return 1;
}
