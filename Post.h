#pragma once

#include "Slove.h"

// 后处理类：负责将位移结果整理并导出为 Tecplot 格式。
class Post
{
public:
	Post() {};

	// 输出位移结果文件，自动区分二维与三维格式。
	int Output_Disp_Tecplot(vector<Node>& nodes, vector<Element>& elements, Matrix& Disp);
};
