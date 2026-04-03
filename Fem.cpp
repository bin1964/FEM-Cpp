#include "Fem.h"

// 按三维坐标构造节点，默认视为内点。
Node::Node(double ix, double iy, double iz) {
	x = ix;
	y = iy;
	z = iz;
	type = 0;
}

// 按三维坐标和节点类型构造节点。
Node::Node(double ix, double iy, double iz, int itype) {
	x = ix;
	y = iy;
	z = iz;
	type = itype;
}

// 按二维坐标和节点类型构造节点，z 坐标默认取 0。
Node::Node(double ix, double iy, int itype) {
	x = ix;
	y = iy;
	z = 0.0;
	type = itype;
}
