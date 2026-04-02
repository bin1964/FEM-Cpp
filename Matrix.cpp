#include "Matrix.h"
#include <cmath>
using namespace std;

//默认构造函数
Matrix::Matrix() : m_rows(0), m_cols(0) {};

//构造函数
Matrix::Matrix(size_t rows, size_t cols) : m_rows(rows), m_cols(cols) {
	data = vector<vector<double>>(rows, vector<double>(cols, 0));
}

//一维向量构造(一行n列)
Matrix::Matrix(const vector<double>& vec2D) {
	size_t N = vec2D.size();
	m_rows = 1;
	m_cols = N;
	vector<double> temp(N);
	data.assign(1, temp);
	for (size_t i = 0; i < N; ++i) {
		data[0][i] = vec2D[i];
	}
}
//一维向量构造（整数）
Matrix::Matrix(const vector<int>& vec2D) {
	size_t N = vec2D.size();
	m_rows = 1;
	m_cols = N;
	vector<double> temp(N);
	data.assign(1, temp);
	for (size_t i = 0; i < N; ++i) {
		data[0][i] = vec2D[i];
	}
}

//二维向量构造（n行m列）
Matrix::Matrix(const vector<vector<double>>& vec2D) {
	m_rows = vec2D.size();
	m_cols = vec2D[0].size();
	data = vec2D;
}

//获取矩阵行数
size_t Matrix::getRows() const {
	return m_rows;
}

//获取矩阵列数
size_t Matrix::getCols() const {
	return m_cols;
}

//设置矩阵元素
void Matrix::setElements(size_t row, size_t col, double value) {
	if (row >= m_rows || col >= m_cols) {
		throw out_of_range("Matrix index out of range");
	}
	data[row][col] = value;
}

//获取矩阵元素
double Matrix::getElement(size_t row, size_t col) const {
	if (row >= m_rows || col >= m_cols) {
		throw out_of_range("Matrix index out of range");
	}
	return data[row][col];
}

//获取矩阵总元素数
size_t Matrix::size() const {
	return m_rows * m_cols;
}

//获取矩阵元素（使用()）
//可修改
double& Matrix::operator()(size_t row, size_t col) {
	if (row >= m_rows || col >= m_cols) {
		throw out_of_range("Matrix index out of range");
	}
	return data[row][col];
}
//仅访问
const double& Matrix::operator()(size_t row, size_t col) const {
	if (row >= m_rows || col >= m_cols) {
		throw out_of_range("Matrix index out of range");
	}
	return data[row][col];
}

//获取一行元素（使用[]）
vector<double>& Matrix::operator[](size_t row) {
	if (row >= m_rows) {
		throw out_of_range("Matrix index out of range");
	}
	return data[row];
}

const vector<double>& Matrix::operator[](size_t row) const {
	if (row >= m_rows) {
		throw out_of_range("Matrix index out of range");
	}
	return data[row];
}

//获取一列元素
Matrix Matrix::col(size_t col) const {
	if (col >= m_cols) {
		throw out_of_range("Matrix index out of range");
	}
	Matrix result(m_rows, 1);
	for (size_t i = 0; i < m_rows; ++i) {
		result[i][0] = data[i][col];
	}
	return result;
}

//矩阵加法
//第一个const表示函数不会修改参数 others 的状态；第二个const表示不会修改当前对象（*this）的状态；仅用于读取。
Matrix Matrix::operator+(const Matrix& others) const {
	if (m_rows != others.m_rows || m_cols != others.m_cols) {
		throw invalid_argument("Matrix dimensions do not match for addition");
	}

	Matrix result(m_rows, m_cols);
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_cols; ++j) {
			result.data[i][j] = data[i][j] + others.data[i][j];
		}
	}
	return result;
}

Matrix& Matrix::operator+=(const Matrix& others) {
	if (m_rows != others.m_rows || m_cols != others.m_cols) {
		throw invalid_argument("Matrix dimensions do not match for addition");
	}

	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_cols; ++j) {
			(*this)(i, j) += others(i, j);
		}
	}
	return *this;
}

//矩阵乘法
Matrix Matrix::operator*(const Matrix& others) const {
	if (m_cols != others.m_rows) {
		throw invalid_argument("Matrix dimensions do not match for multiplication");
	}

	Matrix result(m_rows, others.m_cols);
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < others.m_cols; ++j) {
			for (size_t k = 0; k < m_cols; ++k) {
				result.data[i][j] += data[i][k] * others.data[k][j];
			}
		}
	}
	return result;
}

//矩阵与数字相乘
Matrix Matrix::operator*(const double& other) const {
	Matrix result(m_rows, m_cols);
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_cols; ++j) {
			result(i, j) = other * (*this)(i, j);
		}
	}
	return result;
}
// *=
Matrix& Matrix::operator*=(double scalar) {
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_cols; ++j) {
			(*this)(i, j) *= scalar;
		}
	}
	return *this;
}

//矩阵乘法（数*mat）
Matrix operator*(const double& other, const Matrix& mat) {
	return mat * other;
}

//矩阵的行列式
double Matrix::determinant() const {
	if (m_rows != m_cols) {
		throw invalid_argument("Matrix must be square to compute determinant");
	}
	if (m_rows == 1) {
		return data[0][0];
	}
	if (m_rows == 2) {
		return data[0][0] * data[1][1] - data[0][1] * data[1][0];
	}

	double det = 0;
	for (size_t j = 0; j < m_cols; ++j) {
		Matrix submatrix(m_rows - 1, m_cols - 1);
		for (size_t i = 1; i < m_rows; ++i) {
			size_t col_index = 0;
			for (size_t k = 0; k < m_cols; ++k) {
				if (k != j) {
					submatrix[i - 1][col_index] = data[i][k];
					col_index++;
				}
			}
		}
		double sign = (j % 2 == 0) ? 1 : -1;
		det += sign * data[0][j] * submatrix.determinant();
	}
	return det;
}

//矩阵转置
Matrix Matrix::transpose() const {
	Matrix result(m_cols, m_rows);
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_cols; ++j) {
			result.data[j][i] = data[i][j];
		}
	}
	return result;
}

//矩阵的逆
Matrix Matrix::inverse() const {
	//检查是否为方阵
	if (m_rows != m_cols) {
		throw invalid_argument("Matrix must be square to compute inverse");
	}

	size_t n = m_rows;

	//创建增广矩阵[A|I]
	Matrix augmented(n, 2 * n);

	//复制原矩阵到增广矩阵左侧
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < n; ++j) {
			augmented(i, j) = (*this)(i, j);
		}
	}

	//复制单位矩阵到增广矩阵右侧
	for (size_t i = 0; i < n; ++i) {
		augmented(i, i + n) = 1.0;
	}

	//高斯-约旦消元法
	for (size_t i = 0; i < n; ++i) {
		//寻找主元（当前列最大的元素）
		size_t maxRow = i;
		double maxVal = fabs(augmented(i, i));

		for (size_t k = i + 1; k < n; ++k) {
			double val = fabs(augmented(k, i));
			if (val > maxVal) {
				maxVal = val;
				maxRow = k;
			}
		}

		//如果主元太小，矩阵可能接近奇异
		if (maxVal < epsilon) {
			throw runtime_error("Matrix is singular or nearly singular");
		}

		//交换行
		if (maxRow != i) {
			for (size_t j = 0; j < 2 * n; ++j) {
				swap(augmented(i, j), augmented(maxRow, j));
			}
		}

		//将主元归一化为1
		double pivot = augmented(i, i);
		for (size_t j = 0; j < 2 * n; ++j) {
			augmented(i, j) /= pivot;
		}

		//消元，使其他行的对应列元素为0
		for (size_t k = 0; k < n; ++k) {
			if (k != i) {
				double factor = augmented(k, i);
				for (size_t j = 0; j < 2 * n; ++j) {
					augmented(k, j) -= factor * augmented(i, j);
				}
			}
		}
	}

	//提取增广矩阵右侧的逆矩阵
	Matrix result(n, n);
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < n; ++j) {
			result(i, j) = augmented(i, j + n);
		}
	}

	return result;
}

//获取矩阵子块
//(row,col)是起始位置，(br_val,bc_val)是子块大小
Matrix Matrix::block(size_t row, size_t col, size_t br_val, size_t bc_val) const {
	// 检查起始位置是否有效
	if (row >= m_rows || col >= m_cols) {
		throw out_of_range("Start position out of range");
	}

	// 检查子块是否超出矩阵边界
	if (row + br_val > m_rows || col + bc_val > m_cols) {
		throw out_of_range("Submatrix dimensions out of range");
	}
	Matrix result(br_val, bc_val);
	for (size_t i = 0; i < br_val; ++i) {
		for (size_t j = 0; j < bc_val; ++j) {
			result(i, j) = (*this)(i + row, j + col);
		}
	}
	return result;
}

//修改子块
//(row,col)是起始位置，(br_val,bc_val)是子块大小
//mat是修改内容
Matrix& Matrix::reblock(size_t row, size_t col, size_t br_val, size_t bc_val, const Matrix& mat) {
	// 检查起始位置是否有效
	if (row >= m_rows || col >= m_cols) {
		throw out_of_range("Start position out of range");
	}

	// 检查子块是否超出矩阵边界
	if (row + br_val > m_rows || col + bc_val > m_cols) {
		throw out_of_range("Submatrix dimensions out of range");
	}

	for (size_t i = 0; i < br_val; ++i) {
		for (size_t j = 0; j < bc_val; ++j) {
			(*this)(i + row, j + col) = mat(i, j);
		}
	}
	return *this;
}

//打印矩阵
void Matrix::print() const {
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_cols; ++j) {
			cout << (*this)(i, j) << " ";
		}
		cout << endl;
	}
}

// 重载 << 运算符
Matrix& Matrix::operator<<(double value) {
	if (current_row < m_rows && current_col < m_cols) {
		data[current_row][current_col] = value;
		current_col++;
		if (current_col == m_cols) {
			current_row++;
			current_col = 0;
		}
	}
	return *this;
}

// 重载逗号运算符
Matrix& Matrix::operator,(double value) {
	return (*this) << value;
}

//输出vector<double>
ostream& operator<<(ostream& out, const vector<double>& A) {
	int cols = A.size();
	out << "[";
	for (size_t i = 0; i < cols; i++) {
		if (i == cols - 1) {
			out << A[i] << "]";
			break;
		}
		out << A[i] << ",";
	}
	return out;
}