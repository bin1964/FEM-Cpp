#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

// 全局数值容差，主要用于矩阵求逆和几何判定。
const double epsilon = 1e-12;

// 矩阵类：提供有限元程序所需的基础矩阵运算能力。
class Matrix {
public:
	// 矩阵数据存储区。
	vector<vector<double>> data;
	// 行数。
	size_t m_rows;
	// 列数。
	size_t m_cols;
	// 用于 << 初始化矩阵时的当前行位置。
	size_t current_row = 0;
	// 用于 << 初始化矩阵时的当前列位置。
	size_t current_col = 0;

	// 默认构造函数，得到空矩阵。
	Matrix();
	// 构造指定大小的零矩阵。
	Matrix(size_t rows, size_t cols);
	// 由 double 一维数组构造单行矩阵。
	Matrix(const vector<double>& vec2D);
	// 由 int 一维数组构造单行矩阵。
	Matrix(const vector<int>& vec2D);
	// 由二维数组构造矩阵。
	Matrix(const vector<vector<double>>& vec2D);
	// 获取矩阵行数。
	size_t getRows() const;
	// 获取矩阵列数。
	size_t getCols() const;
	// 设置指定位置元素。
	void setElements(size_t row, size_t col, double value);
	// 读取指定位置元素。
	double getElement(size_t row, size_t col) const;
	// 获取矩阵总元素数。
	size_t size() const;

	// 使用 () 访问并可修改矩阵元素。
	double& operator()(size_t row, size_t col);
	// 使用 () 只读访问矩阵元素。
	const double& operator()(size_t row, size_t col) const;

	// 使用 [] 访问指定行。
	vector<double>& operator[](size_t row);
	// 使用 [] 只读访问指定行。
	const vector<double>& operator[](size_t row) const;

	// 提取指定列形成列向量。
	Matrix col(size_t col) const;

	// 矩阵加法。
	Matrix operator+(const Matrix& others) const;
	// 原位矩阵加法。
	Matrix& operator+=(const Matrix& others);
	// 矩阵乘法。
	Matrix operator*(const Matrix& others) const;
	// 矩阵与标量相乘。
	Matrix operator*(const double& other) const;
	// 原位标量乘法。
	Matrix& operator*=(double scalar);
	// 计算方阵行列式。
	double determinant() const;
	// 计算矩阵转置。
	Matrix transpose() const;
	// 计算矩阵逆。
	Matrix inverse() const;
	// 提取子块矩阵。
	Matrix block(size_t row, size_t col, size_t br_val, size_t bc_val) const;
	// 将子块矩阵写回原矩阵。
	Matrix& reblock(size_t row, size_t col, size_t br_val, size_t bc_val, const Matrix& mat);
	// 打印矩阵内容。
	void print() const;

	// 使用 << 逐元素填充矩阵。
	Matrix& operator<<(double value);
	// 配合 << 实现逗号链式初始化。
	Matrix& operator,(double value);
};

// 输出一维 double 向量。
ostream& operator<<(ostream& out, const vector<double>& A);
// 标量左乘矩阵。
Matrix operator*(const double& other, const Matrix& mat);
