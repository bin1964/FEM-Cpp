#include"Geometry2D.h"

//二维点类构造函数
Point_2D::Point_2D(double px, double py)
{
	x = px;
	y = py;
}
//点到点的距离（参数是对象）
double Point_2D::distance_to(const Point_2D& pt)const
{
	double rv2 = (x - pt.x) * (x - pt.x) + (y - pt.y) * (y - pt.y);
	return sqrt(rv2);
}
//点到点的距离（参数是坐标）
double Point_2D::distance_to(const double& px, const double& py)const
{
	double rv2 = (x - px) * (x - px) + (y - py) * (y - py);
	return sqrt(rv2);
}

//二维线段的构造函数
Line_2D::Line_2D(Point_2D p0, Point_2D p1) {
	point[0] = p0;
	point[1] = p1;
	A = p1.y - p0.y;
	B = -(p1.x - p0.x);
	D = -(A * p0.x + B * p0.y);
	len = length();
	if (len == 0) virtual_line = false;
	else virtual_line = true;
}
//线段长度
double Line_2D::length() {
	return sqrt((point[1].x - point[0].x) * (point[1].x - point[0].x) + (point[1].y - point[0].y) * (point[1].y - point[0].y));
}
//判断线段包含一个点
int Line_2D::contain(const Point_2D& point_temp) const{
	//一点到线段两端点的距离大于线段的长度，则不在
	if (fabs(point_temp.distance_to(point[0]) + point_temp.distance_to(point[1]) - point[0].distance_to(point[1])) > 1e-8) {
		return 0;
	}
	else {
		return 1;
	}
}
//判断线段包含一个点
int Line_2D::contain(const double& px, const double& py) const {
	Point_2D point_temp(px, py);
	//一点到线段两端点的距离大于线段的长度，则不在
	if (fabs(point_temp.distance_to(point[0]) + point_temp.distance_to(point[1]) - point[0].distance_to(point[1])) > 1e-8) {
		return 0;
	}
	else {
		return 1;
	}
}


