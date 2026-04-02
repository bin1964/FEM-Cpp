#include "Fem2D.h"

Node::Node(double ix, double iy, double iz) {
	x = ix;
	y = iy;
	z = iz;
	type = 0;
}

Node::Node(double ix, double iy, double iz, int itype) {
	x = ix;
	y = iy;
	z = iz;
	type = itype;	//0内点	1边界点	2角点
}

Node::Node(double ix, double iy, int itype) {
	x = ix;
	y = iy;
	z = 0.0;
	type = itype;
}
