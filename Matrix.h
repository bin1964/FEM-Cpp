#include <iostream>
#include <vector>
#include <stdexcept>
#pragma once
using namespace std;
const double epsilon = 1e-12;

class Matrix {
public:
	vector<vector<double>> data;
	size_t m_rows;
	size_t m_cols;
	size_t current_row = 0;
	size_t current_col = 0;

	//默认构造函数
	Matrix();
	//构造函数
	Matrix(size_t rows, size_t cols);
	//一维向量构造
	Matrix(const vector<double>& vec2D);
	//一维向量构造（整数）
	Matrix(const vector<int>& vec2D);
	//二维向量构造
	Matrix(const vector<vector<double>>& vec2D);
	//获取矩阵行数
	size_t getRows() const;
	//获取矩阵列数
	size_t getCols() const;
	//设置矩阵元素
	void setElements(size_t row, size_t col, double value);
	//获取矩阵元素
	double getElement(size_t row, size_t col) const;
	//获取矩阵总元素数
	size_t size() const;




	//获取矩阵元素（使用()）
	//可修改
	double& operator()(size_t row, size_t col);
	//仅访问
	const double& operator()(size_t row, size_t col) const;

	//获取一行元素（使用[]）
	vector<double>& operator[](size_t row);
	const vector<double>& operator[](size_t row) const;

	//获取一列元素
	Matrix col(size_t col) const;

	//矩阵加法
	Matrix operator+(const Matrix& others) const;
	Matrix& operator+=(const Matrix& others);
	//矩阵乘法
	Matrix operator*(const Matrix& others) const;
	//矩阵与数字相乘
	Matrix operator*(const double& other) const;
	Matrix& operator*=(double scalar);
	//矩阵的行列式
	double determinant() const;
	//矩阵转置
	Matrix transpose() const;
	//矩阵的逆
	Matrix inverse() const;
	//获取子块
	Matrix block(size_t row, size_t col, size_t br_val, size_t bc_val) const;
	//修改子块
	Matrix& reblock(size_t row, size_t col, size_t br_val, size_t bc_val, const Matrix& mat);
	//打印矩阵
	void print() const;

	//重载 << 运算符
	Matrix& operator<<(double value);
	//重载逗号运算符
	Matrix& operator,(double value);
};

ostream& operator<<(ostream& out, const vector<double>& A);
//矩阵乘法（数*mat）
Matrix operator*(const double& other, const Matrix& mat);
