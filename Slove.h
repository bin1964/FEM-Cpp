#pragma once

#include <memory>
#include <vector>
#include "Matrix.h"
#include "Fem.h"
#include "Input.h"
#include "Geometry2D.h"
#include "shape.h"

// 求解器类：负责约束处理、刚度矩阵组装、载荷组装与线性方程求解。
class Slove
{
public:
    Slove() = default;

    // 总体刚度矩阵。
    Matrix gK;
    // 总体载荷向量。
    Matrix gF;
    // 位移解向量。
    Matrix u;
    // 每个节点的自由度数，二维为 2，三维为 3。
    int dof_per_node = 2;
    // 被约束的自由度标记数组。
    std::vector<int> constrained_dofs;
    // 约束自由度对应的目标值。
    std::vector<double> constrained_values;

    // 收集位移边界条件并建立约束自由度表。
    int Fixed_Displacement_Constraints(const struct Displace& dis, const std::vector<Node>& nodes);
    // 根据单元对象组装总体刚度矩阵。
    int Assemble_Global_Stiff(const std::vector<std::unique_ptr<ShapeSpace::ShapeInterface>>& shape);
    // 采用罚函数法将位移约束施加到刚度矩阵与载荷向量中。
    int Apply_Constraint_to_Stiff();
    // 按输入载荷定义生成总体载荷向量。
    int Apply_Load_to_Force_Vector(const struct Load& load, const std::vector<Node>& nodes, const std::vector<Element>& elements);
    // 求解线性方程组得到位移向量。
    int Slove_Linear_System();
};
