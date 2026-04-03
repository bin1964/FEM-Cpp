#pragma once

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// 节点类：统一保存二维/三维问题中的节点坐标与节点类型。
class Node {
public:
	// 节点空间坐标，二维问题中 z 通常为 0。
	double x, y, z;
	// 节点类型：0 内点，1 边界面点，2 边界线点，3 角点。
	int type;

	// 构造默认节点或显式指定三维坐标，默认节点类型为内点。
	Node(const double ix = 0, const double iy = 0, const double iz = 0);
	// 构造同时带有三维坐标和节点类型的节点。
	Node(const double ix, const double iy, const double iz, const int itype);
	// 构造二维节点，并显式指定节点类型。
	Node(const double ix, const double iy, const int itype);
};

// 单元类：保存单元节点编号以及材料编号。
class Element {
public:
	// 单元包含的节点编号列表，编号与 nodes 数组保持一致。
	vector<int> nodes_id;
	// 单元材料编号，当前程序中通常默认为 0。
	int mat;

	Element() {};
};

// 边界边类：主要服务于二维边界识别和载荷施加。
class Side
{
public:
	// 边类型：0 内线段，1 力边线，2 自由边线，3 固定边线。
	int type;
	// 构成边线的节点编号。
	vector<int> nodes_id;
};
