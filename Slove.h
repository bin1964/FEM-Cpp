#pragma once

#include <memory>
#include <vector>
#include "Matrix.h"
#include "Fem2D.h"
#include "Input.h"
#include "Geometry2D.h"
#include "shape.h"

class Slove
{
public:
    Slove() = default;

    Matrix gK;
    Matrix gF;
    Matrix u;
    int dof_per_node = 2;
    std::vector<int> constrained_dofs;
    std::vector<double> constrained_values;

    int Fixed_Displacement_Constraints(const struct Displace& dis, const std::vector<Node>& nodes);
    int Assemble_Global_Stiff(const std::vector<std::unique_ptr<ShapeSpace::ShapeInterface>>& shape);
    int Apply_Constraint_to_Stiff();
    int Apply_Load_to_Force_Vector(const struct Load& load, const std::vector<Node>& nodes, const std::vector<Element>& elements);
    int Slove_Linear_System();
};
