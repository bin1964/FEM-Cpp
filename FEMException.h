#pragma once

#include <stdexcept>
#include <string>

// FEM 异常基类：统一承载程序内部各模块抛出的异常信息。
class FEMException : public std::runtime_error {
public:
	explicit FEMException(const std::string& message)
		: std::runtime_error(message) {}
};

// 输入相关异常。
class InputException : public FEMException {
public:
	explicit InputException(const std::string& message)
		: FEMException("Input Error: " + message) {}
};

// 网格相关异常。
class MeshException : public FEMException {
public:
	explicit MeshException(const std::string& message)
		: FEMException("Mesh Error: " + message) {}
};

// 求解器相关异常。
class SolverException : public FEMException {
public:
	explicit SolverException(const std::string& message)
		: FEMException("Solver Error: " + message) {}
};

// 单元相关异常。
class ElementException : public FEMException {
public:
	explicit ElementException(const std::string& message)
		: FEMException("Element Error: " + message) {}
};
