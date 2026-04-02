#include "Slove.h"

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

namespace {
struct ZoneBounds
{
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    bool is3d;
};

struct HexFaceInfo
{
    int nodes[4];
    int fixed_axis;
    double fixed_value;
    int param_axis_1;
    int param_axis_2;
};

Matrix MakeColumnMatrix(const vector<double>& values)
{
    Matrix result(values.size(), 1);
    for (size_t i = 0; i < values.size(); ++i)
    {
        result(i, 0) = values[i];
    }
    return result;
}

int DetectSpatialDimension(const vector<Node>& nodes, const vector<Element>* elements = nullptr)
{
    if (elements != nullptr)
    {
        for (const auto& element : *elements)
        {
            if (element.nodes_id.size() == 8)
            {
                return 3;
            }
        }
    }

    for (const auto& node : nodes)
    {
        if (abs(node.z) > 1e-10)
        {
            return 3;
        }
    }
    return 2;
}

int DofIndex(int node_id, int component, int dof_per_node)
{
    return dof_per_node * node_id + component;
}

bool MatchesPoint(const Node& node, const vector<double>& coef)
{
    const double px = coef.size() >= 1 ? coef[0] : 0.0;
    const double py = coef.size() >= 2 ? coef[1] : 0.0;
    const double pz = coef.size() >= 3 ? coef[2] : 0.0;
    const double dist2 = (node.x - px) * (node.x - px) +
        (node.y - py) * (node.y - py) +
        (node.z - pz) * (node.z - pz);
    return sqrt(dist2) <= 1e-8;
}

bool MatchesLine2D(const Node& node, const vector<double>& coef)
{
    Point_2D p0(coef[0], coef[1]);
    Point_2D p1(coef[2], coef[3]);
    Line_2D line(p0, p1);
    return line.contain(node.x, node.y);
}

bool MatchesSurface3D(const Node& node, const vector<double>& coef)
{
    const double value = coef[0] * node.x + coef[1] * node.y + coef[2] * node.z + coef[3];
    return abs(value) <= 1e-8;
}

ZoneBounds MakeZoneBounds(const vector<double>& coef)
{
    ZoneBounds zone{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false};
    if (coef.size() >= 4)
    {
        zone.xmin = min(coef[0], coef[1]);
        zone.xmax = max(coef[0], coef[1]);
        zone.ymin = min(coef[2], coef[3]);
        zone.ymax = max(coef[2], coef[3]);
    }
    if (coef.size() >= 6)
    {
        zone.zmin = min(coef[4], coef[5]);
        zone.zmax = max(coef[4], coef[5]);
        zone.is3d = true;
    }
    return zone;
}

bool IsPointInsideZone(const Node& node, const ZoneBounds& zone, int spatial_dim)
{
    const bool in_xy = node.x >= zone.xmin - 1e-8 && node.x <= zone.xmax + 1e-8 &&
        node.y >= zone.ymin - 1e-8 && node.y <= zone.ymax + 1e-8;
    if (!in_xy)
    {
        return false;
    }
    if (spatial_dim == 2)
    {
        return true;
    }
    return zone.is3d &&
        node.z >= zone.zmin - 1e-8 && node.z <= zone.zmax + 1e-8;
}

bool IsElementInsideZone(const Element& element, const vector<Node>& nodes, const ZoneBounds& zone, int spatial_dim)
{
    for (int node_id : element.nodes_id)
    {
        if (!IsPointInsideZone(nodes[node_id], zone, spatial_dim))
        {
            return false;
        }
    }
    return true;
}

double ComputeElementArea2D(const Element& element, const vector<Node>& nodes)
{
    double twice_area = 0.0;
    for (size_t i = 0; i < element.nodes_id.size(); ++i)
    {
        const Node& current = nodes[element.nodes_id[i]];
        const Node& next = nodes[element.nodes_id[(i + 1) % element.nodes_id.size()]];
        twice_area += current.x * next.y - next.x * current.y;
    }
    return 0.5 * abs(twice_area);
}

void AssembleTriangleZoneLoad(const Element& element, const vector<double>& load_components, double area, int dof_per_node, vector<double>& equright)
{
    const double nodal_weight = area / 3.0;
    for (int node_id : element.nodes_id)
    {
        equright[DofIndex(node_id, 0, dof_per_node)] += nodal_weight * load_components[0];
        equright[DofIndex(node_id, 1, dof_per_node)] += nodal_weight * load_components[1];
    }
}

int AssembleQuadrilateralZoneLoad(const Element& element, const vector<Node>& nodes, const vector<double>& load_components, int dof_per_node, vector<double>& equright)
{
    const double gauss_point = 1.0 / sqrt(3.0);
    const double gauss_points[2] = {-gauss_point, gauss_point};
    double local_force[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    for (double s : gauss_points)
    {
        for (double t : gauss_points)
        {
            const double N[4] =
            {
                0.25 * (1.0 - s) * (1.0 - t),
                0.25 * (1.0 + s) * (1.0 - t),
                0.25 * (1.0 + s) * (1.0 + t),
                0.25 * (1.0 - s) * (1.0 + t)
            };
            const double dNds[4] =
            {
                -0.25 * (1.0 - t),
                0.25 * (1.0 - t),
                0.25 * (1.0 + t),
                -0.25 * (1.0 + t)
            };
            const double dNdt[4] =
            {
                -0.25 * (1.0 - s),
                -0.25 * (1.0 + s),
                0.25 * (1.0 + s),
                0.25 * (1.0 - s)
            };

            double dxds = 0.0;
            double dyds = 0.0;
            double dxdt = 0.0;
            double dydt = 0.0;
            for (int i = 0; i < 4; ++i)
            {
                const Node& node = nodes[element.nodes_id[i]];
                dxds += dNds[i] * node.x;
                dyds += dNds[i] * node.y;
                dxdt += dNdt[i] * node.x;
                dydt += dNdt[i] * node.y;
            }

            const double det_jacobian = abs(dxds * dydt - dyds * dxdt);
            if (det_jacobian <= epsilon)
            {
                return 0;
            }

            for (int i = 0; i < 4; ++i)
            {
                local_force[2 * i] += N[i] * load_components[0] * det_jacobian;
                local_force[2 * i + 1] += N[i] * load_components[1] * det_jacobian;
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        const int node_id = element.nodes_id[i];
        equright[DofIndex(node_id, 0, dof_per_node)] += local_force[2 * i];
        equright[DofIndex(node_id, 1, dof_per_node)] += local_force[2 * i + 1];
    }

    return 1;
}

void EvaluateHex8Shape(double xi, double eta, double zeta, double N[8], double dN_dxi[8], double dN_deta[8], double dN_dzeta[8])
{
    const double xi_sign[8] = {-1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0};
    const double eta_sign[8] = {-1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0};
    const double zeta_sign[8] = {-1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0};

    for (int i = 0; i < 8; ++i)
    {
        N[i] = 0.125 * (1.0 + xi * xi_sign[i]) * (1.0 + eta * eta_sign[i]) * (1.0 + zeta * zeta_sign[i]);
        dN_dxi[i] = 0.125 * xi_sign[i] * (1.0 + eta * eta_sign[i]) * (1.0 + zeta * zeta_sign[i]);
        dN_deta[i] = 0.125 * eta_sign[i] * (1.0 + xi * xi_sign[i]) * (1.0 + zeta * zeta_sign[i]);
        dN_dzeta[i] = 0.125 * zeta_sign[i] * (1.0 + xi * xi_sign[i]) * (1.0 + eta * eta_sign[i]);
    }
}

double ComputeHexJacobianDet(const Element& element, const vector<Node>& nodes, double xi, double eta, double zeta, double N[8])
{
    double dN_dxi[8];
    double dN_deta[8];
    double dN_dzeta[8];
    EvaluateHex8Shape(xi, eta, zeta, N, dN_dxi, dN_deta, dN_dzeta);

    Matrix jacobi(3, 3);
    for (int i = 0; i < 8; ++i)
    {
        const Node& node = nodes[element.nodes_id[i]];
        jacobi(0, 0) += dN_dxi[i] * node.x;
        jacobi(0, 1) += dN_deta[i] * node.x;
        jacobi(0, 2) += dN_dzeta[i] * node.x;
        jacobi(1, 0) += dN_dxi[i] * node.y;
        jacobi(1, 1) += dN_deta[i] * node.y;
        jacobi(1, 2) += dN_dzeta[i] * node.y;
        jacobi(2, 0) += dN_dxi[i] * node.z;
        jacobi(2, 1) += dN_deta[i] * node.z;
        jacobi(2, 2) += dN_dzeta[i] * node.z;
    }

    return abs(jacobi.determinant());
}

int AssembleHexahedronZoneLoad(const Element& element, const vector<Node>& nodes, const vector<double>& load_components, int dof_per_node, vector<double>& equright)
{
    const double gauss_point = 1.0 / sqrt(3.0);
    const double gauss_points[2] = {-gauss_point, gauss_point};

    for (double xi : gauss_points)
    {
        for (double eta : gauss_points)
        {
            for (double zeta : gauss_points)
            {
                double N[8];
                const double det_jacobian = ComputeHexJacobianDet(element, nodes, xi, eta, zeta, N);
                if (det_jacobian <= epsilon)
                {
                    return 0;
                }

                for (int i = 0; i < 8; ++i)
                {
                    for (int comp = 0; comp < dof_per_node; ++comp)
                    {
                        equright[DofIndex(element.nodes_id[i], comp, dof_per_node)] += N[i] * load_components[comp] * det_jacobian;
                    }
                }
            }
        }
    }

    return 1;
}

bool FaceMatchesSurface(const Element& element, const vector<Node>& nodes, const HexFaceInfo& face, const vector<double>& surface_coef)
{
    for (int local_node : face.nodes)
    {
        if (!MatchesSurface3D(nodes[element.nodes_id[local_node]], surface_coef))
        {
            return false;
        }
    }
    return true;
}

int AssembleHexahedronSurfaceLoad(const Element& element, const vector<Node>& nodes, const HexFaceInfo& face, const vector<double>& load_components, int dof_per_node, vector<double>& equright)
{
    const double gauss_point = 1.0 / sqrt(3.0);
    const double gauss_points[2] = {-gauss_point, gauss_point};

    for (double gp1 : gauss_points)
    {
        for (double gp2 : gauss_points)
        {
            double xi = 0.0;
            double eta = 0.0;
            double zeta = 0.0;
            const double params[3] = {0.0, 0.0, 0.0};
            double natural[3] = {params[0], params[1], params[2]};
            natural[face.fixed_axis] = face.fixed_value;
            natural[face.param_axis_1] = gp1;
            natural[face.param_axis_2] = gp2;
            xi = natural[0];
            eta = natural[1];
            zeta = natural[2];

            double N[8];
            double dN_dxi[8];
            double dN_deta[8];
            double dN_dzeta[8];
            EvaluateHex8Shape(xi, eta, zeta, N, dN_dxi, dN_deta, dN_dzeta);

            double tangent_1[3] = {0.0, 0.0, 0.0};
            double tangent_2[3] = {0.0, 0.0, 0.0};
            for (int i = 0; i < 8; ++i)
            {
                const Node& node = nodes[element.nodes_id[i]];
                const double deriv_1 = (face.param_axis_1 == 0) ? dN_dxi[i] : (face.param_axis_1 == 1 ? dN_deta[i] : dN_dzeta[i]);
                const double deriv_2 = (face.param_axis_2 == 0) ? dN_dxi[i] : (face.param_axis_2 == 1 ? dN_deta[i] : dN_dzeta[i]);
                tangent_1[0] += deriv_1 * node.x;
                tangent_1[1] += deriv_1 * node.y;
                tangent_1[2] += deriv_1 * node.z;
                tangent_2[0] += deriv_2 * node.x;
                tangent_2[1] += deriv_2 * node.y;
                tangent_2[2] += deriv_2 * node.z;
            }

            const double cross_x = tangent_1[1] * tangent_2[2] - tangent_1[2] * tangent_2[1];
            const double cross_y = tangent_1[2] * tangent_2[0] - tangent_1[0] * tangent_2[2];
            const double cross_z = tangent_1[0] * tangent_2[1] - tangent_1[1] * tangent_2[0];
            const double det_surface = sqrt(cross_x * cross_x + cross_y * cross_y + cross_z * cross_z);
            if (det_surface <= epsilon)
            {
                return 0;
            }

            for (int i = 0; i < 8; ++i)
            {
                for (int comp = 0; comp < dof_per_node; ++comp)
                {
                    equright[DofIndex(element.nodes_id[i], comp, dof_per_node)] += N[i] * load_components[comp] * det_surface;
                }
            }
        }
    }

    return 1;
}
}

int Slove::Fixed_Displacement_Constraints(const struct Displace& dis, const vector<Node>& nodes)
{
    dof_per_node = DetectSpatialDimension(nodes);
    constrained_dofs.assign(dof_per_node * nodes.size(), 0);
    constrained_values.assign(dof_per_node * nodes.size(), 0.0);

    for (int i = 0; i < dis.num; ++i)
    {
        if (dof_per_node == 3 && dis.domain_type[i] == "Line")
        {
            cout << "Error: line displacement constraints are only supported for 2D problems." << endl;
            return 0;
        }
        if (dof_per_node == 2 && dis.domain_type[i] == "Surface")
        {
            cout << "Error: surface displacement constraints are only supported for 3D problems." << endl;
            return 0;
        }
        if (dof_per_node == 3 && dis.domain_type[i] == "Zone" && dis.coef[i].size() != 6)
        {
            cout << "Error: 3D zone displacement constraints require xmin xmax ymin ymax zmin zmax." << endl;
            return 0;
        }

        const ZoneBounds zone = MakeZoneBounds(dis.coef[i]);
        int count = 0;
        for (int j = 0; j < static_cast<int>(nodes.size()); ++j)
        {
            bool matched =
                (dis.domain_type[i] == "Point" && MatchesPoint(nodes[j], dis.coef[i])) ||
                (dis.domain_type[i] == "Line" && MatchesLine2D(nodes[j], dis.coef[i])) ||
                (dis.domain_type[i] == "Surface" && MatchesSurface3D(nodes[j], dis.coef[i])) ||
                (dis.domain_type[i] == "Zone" && IsPointInsideZone(nodes[j], zone, dof_per_node));

            if (matched)
            {
                ++count;
                for (int k = 0; k < static_cast<int>(dis.disp_type[i].size()); ++k)
                {
                    if (dis.disp_type[i][k] == "Fixed_displacement_x")
                    {
                        constrained_dofs[DofIndex(j, 0, dof_per_node)] = 1;
                        constrained_values[DofIndex(j, 0, dof_per_node)] = dis.value[i][k];
                    }
                    else if (dis.disp_type[i][k] == "Fixed_displacement_y")
                    {
                        constrained_dofs[DofIndex(j, 1, dof_per_node)] = 1;
                        constrained_values[DofIndex(j, 1, dof_per_node)] = dis.value[i][k];
                    }
                    else if (dis.disp_type[i][k] == "Fixed_displacement_z")
                    {
                        if (dof_per_node != 3)
                        {
                            cout << "Error: z displacement constraints require a 3D problem." << endl;
                            return 0;
                        }
                        constrained_dofs[DofIndex(j, 2, dof_per_node)] = 1;
                        constrained_values[DofIndex(j, 2, dof_per_node)] = dis.value[i][k];
                    }
                    else if (dis.disp_type[i][k] != "Pure_shear")
                    {
                        cout << "Error: unsupported displacement constraint type." << endl;
                        return 0;
                    }
                }
            }
            else if (dis.domain_type[i] == "All_boundaries" && dis.disp_type[i][0] == "Pure_shear" && dof_per_node == 2 && nodes[j].type != 0)
            {
                ++count;
                constrained_dofs[DofIndex(j, 0, dof_per_node)] = 1;
                constrained_dofs[DofIndex(j, 1, dof_per_node)] = 1;
                constrained_values[DofIndex(j, 0, dof_per_node)] = nodes[j].y * dis.value[i][0];
                constrained_values[DofIndex(j, 1, dof_per_node)] = nodes[j].x * dis.value[i][0];
            }
        }

        if (count == 0)
        {
            cout << "Error: no nodes matched the displacement constraint." << endl;
            return 0;
        }
    }

    u = MakeColumnMatrix(constrained_values);
    cout << "Displacement constraints collected." << endl;
    return 1;
}

int Slove::Assemble_Global_Stiff(const vector<unique_ptr<ShapeSpace::ShapeInterface>>& shape)
{
    if (constrained_dofs.empty())
    {
        cout << "Error: initialize displacement constraints before assembling stiffness." << endl;
        return 0;
    }

    gK = Matrix(constrained_dofs.size(), constrained_dofs.size());

    for (int i = 0; i < static_cast<int>(shape.size()); ++i)
    {
        if (!shape[i]->CalcStif())
        {
            cout << "Error: failed to compute element stiffness at index " << i << endl;
            return 0;
        }
        if (!shape[i]->AsseStif(gK))
        {
            cout << "Error: failed to assemble element stiffness at index " << i << endl;
            return 0;
        }
    }

    cout << "Global stiffness matrix assembled." << endl;
    for (int i = 0; i < gK.getRows(); ++i)
    {
        if (gK(i, i) == 0.0)
        {
            cout << "Warning: gK(" << i << "," << i << ") is zero." << endl;
        }
    }
    return 1;
}

int Slove::Apply_Constraint_to_Stiff()
{
    if (gK.getRows() == 0 || gF.getRows() == 0)
    {
        cout << "Error: stiffness matrix or load vector is empty." << endl;
        return 0;
    }

    double max_k = 0.0;
    for (int i = 0; i < gK.getRows(); ++i)
    {
        for (int j = 0; j < gK.getCols(); ++j)
        {
            max_k = max(max_k, abs(gK(i, j)));
        }
    }
    if (max_k <= epsilon)
    {
        max_k = 1.0;
    }

    const double penalty_factor = 1e12 * max_k;
    for (int i = 0; i < static_cast<int>(constrained_dofs.size()); ++i)
    {
        if (constrained_dofs[i] == 1)
        {
            gK.setElements(i, i, penalty_factor);
            gF.setElements(i, 0, penalty_factor * constrained_values[i]);
        }
    }

    cout << "Constraints applied to stiffness matrix." << endl;
    return 1;
}

int Slove::Apply_Load_to_Force_Vector(const struct Load& load, const vector<Node>& nodes, const vector<Element>& elements)
{
    dof_per_node = DetectSpatialDimension(nodes, &elements);
    const size_t total_dofs = constrained_dofs.empty() ? static_cast<size_t>(dof_per_node * nodes.size()) : constrained_dofs.size();
    vector<double> equright(total_dofs, 0.0);

    const HexFaceInfo hex_faces[6] =
    {
        {{0, 1, 2, 3}, 2, -1.0, 0, 1},
        {{4, 5, 6, 7}, 2, 1.0, 0, 1},
        {{0, 1, 5, 4}, 1, -1.0, 0, 2},
        {{3, 2, 6, 7}, 1, 1.0, 0, 2},
        {{0, 3, 7, 4}, 0, -1.0, 1, 2},
        {{1, 2, 6, 5}, 0, 1.0, 1, 2}
    };

    for (int i = 0; i < load.num; ++i)
    {
        if (dof_per_node == 3 && load.domain_type[i] == "Line")
        {
            cout << "Error: line loads are only supported for 2D problems." << endl;
            return 0;
        }
        if (dof_per_node == 2 && load.domain_type[i] == "Surface")
        {
            cout << "Error: surface loads are only supported for 3D problems." << endl;
            return 0;
        }
        if (dof_per_node == 3 && load.domain_type[i] == "Zone" && load.coef[i].size() != 6)
        {
            cout << "Error: 3D zone loads require xmin xmax ymin ymax zmin zmax." << endl;
            return 0;
        }

        vector<double> load_components(dof_per_node, 0.0);
        for (int j = 0; j < static_cast<int>(load.load_type[i].size()); ++j)
        {
            if (load.load_type[i][j] == "Force_x")
            {
                load_components[0] += load.value[i][j];
            }
            else if (load.load_type[i][j] == "Force_y")
            {
                load_components[1] += load.value[i][j];
            }
            else if (load.load_type[i][j] == "Force_z")
            {
                if (dof_per_node != 3)
                {
                    cout << "Error: z force loads require a 3D problem." << endl;
                    return 0;
                }
                load_components[2] += load.value[i][j];
            }
        }

        if (load.domain_type[i] == "Point")
        {
            for (int j = 0; j < static_cast<int>(nodes.size()); ++j)
            {
                if (MatchesPoint(nodes[j], load.coef[i]))
                {
                    for (int comp = 0; comp < dof_per_node; ++comp)
                    {
                        equright[DofIndex(j, comp, dof_per_node)] += load_components[comp];
                    }
                }
            }
        }

        if (load.domain_type[i] == "Line")
        {
            vector<Side> sides;
            Point_2D poi[2];
            poi[0].x = load.coef[i][0];
            poi[0].y = load.coef[i][1];
            poi[1].x = load.coef[i][2];
            poi[1].y = load.coef[i][3];

            Line_2D flin(poi[0], poi[1]);
            for (int j = 0; j < static_cast<int>(elements.size()); ++j)
            {
                for (int k = 0; k < static_cast<int>(elements[j].nodes_id.size()); ++k)
                {
                    vector<int> noid;
                    if (k == static_cast<int>(elements[j].nodes_id.size()) - 1)
                    {
                        noid.push_back(elements[j].nodes_id[k]);
                        noid.push_back(elements[j].nodes_id[0]);
                    }
                    else
                    {
                        noid.push_back(elements[j].nodes_id[k]);
                        noid.push_back(elements[j].nodes_id[k + 1]);
                    }

                    Point_2D poi_nod0(nodes[noid[0]].x, nodes[noid[0]].y);
                    Point_2D poi_nod1(nodes[noid[1]].x, nodes[noid[1]].y);
                    if (flin.contain(poi_nod0) && flin.contain(poi_nod1))
                    {
                        Side side_temp;
                        side_temp.nodes_id = noid;
                        side_temp.type = 1;
                        sides.push_back(side_temp);
                    }
                }
            }

            for (const auto& side : sides)
            {
                int node0 = side.nodes_id[0];
                int node1 = side.nodes_id[1];
                Point_2D p0(nodes[node0].x, nodes[node0].y);
                Point_2D p1(nodes[node1].x, nodes[node1].y);
                double L = p0.distance_to(p1.x, p1.y);

                if (L <= 1e-8)
                {
                    cout << "Warning: skipped a line load on a near-zero edge." << endl;
                    continue;
                }

                equright[DofIndex(node0, 0, dof_per_node)] += (L / 2.0) * load_components[0];
                equright[DofIndex(node1, 0, dof_per_node)] += (L / 2.0) * load_components[0];
                equright[DofIndex(node0, 1, dof_per_node)] += (L / 2.0) * load_components[1];
                equright[DofIndex(node1, 1, dof_per_node)] += (L / 2.0) * load_components[1];
            }
        }

        if (load.domain_type[i] == "Surface")
        {
            int matched_faces = 0;
            for (const auto& element : elements)
            {
                if (element.nodes_id.size() != 8)
                {
                    continue;
                }

                for (const auto& face : hex_faces)
                {
                    if (!FaceMatchesSurface(element, nodes, face, load.coef[i]))
                    {
                        continue;
                    }

                    if (!AssembleHexahedronSurfaceLoad(element, nodes, face, load_components, dof_per_node, equright))
                    {
                        cout << "Warning: skipped a surface load on an invalid hexahedral face." << endl;
                        continue;
                    }
                    ++matched_faces;
                }
            }

            if (matched_faces == 0)
            {
                cout << "Warning: no hexahedral faces matched the surface load." << endl;
            }
        }

        if (load.domain_type[i] == "Zone")
        {
            const ZoneBounds zone = MakeZoneBounds(load.coef[i]);
            int matched_elements = 0;

            for (int j = 0; j < static_cast<int>(elements.size()); ++j)
            {
                const Element& element = elements[j];
                if (!IsElementInsideZone(element, nodes, zone, dof_per_node))
                {
                    continue;
                }

                if (element.nodes_id.size() == 3)
                {
                    const double area = ComputeElementArea2D(element, nodes);
                    if (area <= epsilon)
                    {
                        cout << "Warning: skipped a zone load on a near-zero area element." << endl;
                        continue;
                    }
                    AssembleTriangleZoneLoad(element, load_components, area, dof_per_node, equright);
                }
                else if (element.nodes_id.size() == 4)
                {
                    if (!AssembleQuadrilateralZoneLoad(element, nodes, load_components, dof_per_node, equright))
                    {
                        cout << "Warning: skipped a zone load on an invalid quadrilateral element." << endl;
                        continue;
                    }
                }
                else if (element.nodes_id.size() == 8)
                {
                    if (!AssembleHexahedronZoneLoad(element, nodes, load_components, dof_per_node, equright))
                    {
                        cout << "Warning: skipped a zone load on an invalid hexahedral element." << endl;
                        continue;
                    }
                }
                else
                {
                    cout << "Error: unsupported element type for zone load." << endl;
                    return 0;
                }

                ++matched_elements;
            }

            if (matched_elements == 0)
            {
                cout << "Warning: no full elements matched the zone load." << endl;
            }
        }
    }

    gF = MakeColumnMatrix(equright);
    cout << "Load vector assembled." << endl;
    return 1;
}

int Slove::Slove_Linear_System()
{
    u = gK.inverse() * gF;
    cout << "Linear system solved." << endl;
    return 1;
}
