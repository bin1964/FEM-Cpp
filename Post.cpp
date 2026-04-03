#include "Post.h"

// 将位移结果导出为 Tecplot 文本格式，并自动区分二维与三维输出。
int Post::Output_Disp_Tecplot(vector<Node>& nodes, vector<Element>& elements, Matrix& Disp)
{
    if (nodes.empty() || elements.empty()) {
        cout << "Error: no mesh data is available for post-processing." << endl;
        return 0;
    }

    const int dof_per_node = static_cast<int>(Disp.getRows() / nodes.size());
    if (dof_per_node != 2 && dof_per_node != 3) {
        cout << "Error: unsupported displacement vector size." << endl;
        return 0;
    }

    ofstream otec("data\\results\\result_disp.dat");
    otec << "TITLE = Tecplot_Displacement" << endl;

    int any = 0;
    int iNum = int(elements[any].nodes_id.size());

    if (dof_per_node == 3 || iNum == 8) {
        otec << "VARIABLES = X, Y, Z, Ux, Uy, Uz" << endl;
        otec << "ZONE N=" << int(nodes.size()) << ", E=" << int(elements.size())
            << ", F=FEPOINT, ET=BRICK" << endl;

        for (int i = 0; i < int(nodes.size()); ++i) {
            otec << nodes[i].x + Disp(3 * i, 0) << "  "
                << nodes[i].y + Disp(3 * i + 1, 0) << "  "
                << nodes[i].z + Disp(3 * i + 2, 0) << "  "
                << Disp(3 * i, 0) << "  "
                << Disp(3 * i + 1, 0) << "  "
                << Disp(3 * i + 2, 0) << endl;
        }
        otec << endl;

        for (int j = 0; j < int(elements.size()); ++j) {
            otec << elements[j].nodes_id[0] + 1 << "  "
                << elements[j].nodes_id[1] + 1 << "  "
                << elements[j].nodes_id[2] + 1 << "  "
                << elements[j].nodes_id[3] + 1 << "  "
                << elements[j].nodes_id[4] + 1 << "  "
                << elements[j].nodes_id[5] + 1 << "  "
                << elements[j].nodes_id[6] + 1 << "  "
                << elements[j].nodes_id[7] + 1 << endl;
        }
    }
    else {
        otec << "VARIABLES = X, Y, Ux, Uy" << endl;
        string etType = (iNum == 3) ? "TRIANGLE" : "QUADRILATERAL";

        otec << "ZONE N=" << int(nodes.size()) << ", E=" << int(elements.size())
            << ", F=FEPOINT, ET=" << etType << endl;

        for (int i = 0; i < int(nodes.size()); ++i) {
            otec << nodes[i].x + Disp(2 * i, 0) << "  "
                << nodes[i].y + Disp(2 * i + 1, 0) << "  "
                << Disp(2 * i, 0) << "  "
                << Disp(2 * i + 1, 0) << endl;
        }
        otec << endl;

        for (int j = 0; j < int(elements.size()); ++j) {
            if (iNum == 4) {
                otec << elements[j].nodes_id[0] + 1 << "  "
                    << elements[j].nodes_id[1] + 1 << "  "
                    << elements[j].nodes_id[2] + 1 << "  "
                    << elements[j].nodes_id[3] + 1 << endl;
            }
            else {
                otec << elements[j].nodes_id[0] + 1 << "  "
                    << elements[j].nodes_id[1] + 1 << "  "
                    << elements[j].nodes_id[2] + 1 << endl;
            }
        }
    }
    otec.close();

    cout << "Displacement result exported." << endl;
    return 1;
}
