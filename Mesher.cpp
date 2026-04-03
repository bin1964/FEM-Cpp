#include "Mesher.h"
#include "Input.h"

#include <algorithm>
#include <sstream>
#include <unordered_map>

namespace {
struct ParsedElement {
	int dim;
	vector<int> node_ids;
};

bool GetSupportedElementInfo(int gmsh_type, int& node_count, int& dim) {
	if (gmsh_type == 2) {
		node_count = 3;
		dim = 2;
		return true;
	}
	if (gmsh_type == 3) {
		node_count = 4;
		dim = 2;
		return true;
	}
	if (gmsh_type == 5) {
		node_count = 8;
		dim = 3;
		return true;
	}
	return false;
}
}

int Mesher::Generate_Mesh(const struct ElementType& element_type, const struct Geom_RVE& geom_rve, const struct Grid_size& grid_size) {
	nodes.clear();
	elements.clear();
	dim = 2;

	int temp = 0;
	if (element_type.eletype == EleType::Quad) {
		cout << "Generating quadrilateral mesh..." << endl;
		temp = Generate_qua_grids(geom_rve, grid_size);
	}
	else if (element_type.eletype == EleType::Tria) {
		cout << "Generating triangular mesh..." << endl;
		temp = Generate_tri_grids(geom_rve, grid_size);
	}
	else if (element_type.eletype == EleType::Hexa8 || element_type.eletype == EleType::Read_Mesh) {
		cout << "Reading mesh data..." << endl;
		temp = Read_mesh();
	}
	else {
		cout << "Error: unsupported mesh type!" << endl;
		return 0;
	}

	if (temp == 1) {
		cout << "Mesh generation completed." << endl;
		return Export_mesh_data_tecplot();
	}

	cout << "Error: mesh generation failed!" << endl;
	return 0;
}

int Mesher::Generate_qua_grids(const struct Geom_RVE& geom_rve, const struct Grid_size& grid_size) {
	dim = 2;
	dx = grid_size.delta_x;
	dy = grid_size.delta_y;

	int x_sec_num = int(geom_rve.len_x / dx + 1e-8) + 1;
	int y_sec_num = int(geom_rve.wid_y / dy + 1e-8) + 1;

	for (int i = 0; i < y_sec_num; ++i) {
		double y = geom_rve.origin[1] + i * dy;
		for (int j = 0; j < x_sec_num; ++j) {
			double x = geom_rve.origin[0] + j * dx;
			Node nd(x, y, 0.0);
			if ((i == 0 || i == y_sec_num - 1) && (j == 0 || j == x_sec_num - 1)) {
				nd.type = 2;
			}
			else if (i == 0 || i == y_sec_num - 1 || j == 0 || j == x_sec_num - 1) {
				nd.type = 1;
			}
			else {
				nd.type = 0;
			}
			nodes.push_back(nd);
		}
	}

	for (int i = 0; i < y_sec_num - 1; i++) {
		for (int j = 0; j < x_sec_num - 1; j++) {
			Element ele_temp;
			ele_temp.nodes_id.push_back(i * x_sec_num + j);
			ele_temp.nodes_id.push_back(i * x_sec_num + j + 1);
			ele_temp.nodes_id.push_back((i + 1) * x_sec_num + j + 1);
			ele_temp.nodes_id.push_back((i + 1) * x_sec_num + j);
			ele_temp.mat = 0;
			elements.push_back(ele_temp);
		}
	}
	return 1;
}

int Mesher::Generate_tri_grids(const Geom_RVE& geom_rve, const Grid_size& grid_size) {
	dim = 2;
	dx = grid_size.delta_x;
	dy = grid_size.delta_y;

	int x_sec_num = int(geom_rve.len_x / dx + 1e-8) + 1;
	int y_sec_num = int(geom_rve.wid_y / dy + 1e-8) + 1;

	for (int i = 0; i < y_sec_num; ++i) {
		double y = geom_rve.origin[1] + i * dy;
		for (int j = 0; j < x_sec_num; ++j) {
			double x = geom_rve.origin[0] + j * dx;
			Node nd(x, y, 0.0);
			if ((i == 0 || i == y_sec_num - 1) && (j == 0 || j == x_sec_num - 1)) {
				nd.type = 2;
			}
			else if (i == 0 || i == y_sec_num - 1 || j == 0 || j == x_sec_num - 1) {
				nd.type = 1;
			}
			else {
				nd.type = 0;
			}
			nodes.push_back(nd);
		}
	}

	for (int i = 0; i < y_sec_num - 1; ++i) {
		for (int j = 0; j < x_sec_num - 1; j++) {
			Element ele_temp1;
			ele_temp1.nodes_id.push_back(i * x_sec_num + j);
			ele_temp1.nodes_id.push_back(i * x_sec_num + j + 1);
			ele_temp1.nodes_id.push_back((i + 1) * x_sec_num + j);
			ele_temp1.mat = 0;
			elements.push_back(ele_temp1);
		}
	}

	for (int i = 0; i < y_sec_num - 1; ++i) {
		for (int j = 0; j < x_sec_num - 1; j++) {
			Element ele_temp2;
			ele_temp2.nodes_id.push_back(i * x_sec_num + j + 1);
			ele_temp2.nodes_id.push_back((i + 1) * x_sec_num + j + 1);
			ele_temp2.nodes_id.push_back((i + 1) * x_sec_num + j);
			ele_temp2.mat = 0;
			elements.push_back(ele_temp2);
		}
	}
	return 1;
}

int Mesher::Read_mesh() {
	ifstream idata("data\\mesh_inp.dat");
	if (!idata.is_open()) {
		cout << "Error: mesh_inp.dat cannot be opened!" << endl;
		return 0;
	}

	string line;
	while (getline(idata, line)) {
		if (line == "$Nodes") {
			break;
		}
	}
	if (idata.eof()) {
		cout << "Error: $Nodes section is missing in mesh_inp.dat!" << endl;
		return 0;
	}

	getline(idata, line);
	const int nodn = stoi(line);
	unordered_map<int, Node> temp_nod;
	for (int i = 0; i < nodn; ++i) {
		getline(idata, line);
		istringstream istr(line);
		int nnum = 0;
		double x = 0.0;
		double y = 0.0;
		double z = 0.0;
		istr >> nnum >> x >> y >> z;
		temp_nod[nnum] = Node(x, y, z);
	}

	getline(idata, line);
	if (line != "$EndNodes") {
		cout << "Error: $EndNodes section is missing in mesh_inp.dat!" << endl;
		return 0;
	}

	while (getline(idata, line)) {
		if (line == "$Elements") {
			break;
		}
	}
	if (idata.eof()) {
		cout << "Error: $Elements section is missing in mesh_inp.dat!" << endl;
		return 0;
	}

	getline(idata, line);
	const int elen = stoi(line);
	vector<ParsedElement> parsed_elements;
	int max_dim = 0;
	for (int i = 0; i < elen; ++i) {
		getline(idata, line);
		if (line.empty()) {
			continue;
		}

		istringstream istr(line);
		int elnum = 0;
		int etype = 0;
		int num_tags = 0;
		istr >> elnum >> etype >> num_tags;

		int node_count = 0;
		int element_dim = 0;
		if (!GetSupportedElementInfo(etype, node_count, element_dim)) {
			continue;
		}

		for (int j = 0; j < num_tags; ++j) {
			int tag = 0;
			istr >> tag;
		}

		ParsedElement parsed;
		parsed.dim = element_dim;
		for (int j = 0; j < node_count; ++j) {
			int node_id = 0;
			istr >> node_id;
			parsed.node_ids.push_back(node_id);
		}

		if (static_cast<int>(parsed.node_ids.size()) != node_count) {
			cout << "Error: failed to parse all node ids of an element!" << endl;
			return 0;
		}

		parsed_elements.push_back(parsed);
		max_dim = max(max_dim, element_dim);
	}

	getline(idata, line);
	if (line != "$EndElements") {
		cout << "Error: $EndElements section is missing in mesh_inp.dat!" << endl;
		return 0;
	}

	if (parsed_elements.empty() || max_dim == 0) {
		cout << "Error: no supported elements were found in mesh_inp.dat!" << endl;
		return 0;
	}

	unordered_map<int, int> num_nod;
	for (const auto& parsed : parsed_elements) {
		if (parsed.dim != max_dim) {
			continue;
		}

		Element temp_ele;
		for (int original_node_id : parsed.node_ids) {
			auto node_it = temp_nod.find(original_node_id);
			if (node_it == temp_nod.end()) {
				cout << "Error: element refers to an undefined node id " << original_node_id << "!" << endl;
				return 0;
			}

			auto renumber_it = num_nod.find(original_node_id);
			if (renumber_it == num_nod.end()) {
				nodes.push_back(node_it->second);
				const int new_id = static_cast<int>(nodes.size()) - 1;
				num_nod[original_node_id] = new_id;
				temp_ele.nodes_id.push_back(new_id);
			}
			else {
				temp_ele.nodes_id.push_back(renumber_it->second);
			}
		}
		temp_ele.mat = 0;
		elements.push_back(temp_ele);
	}

	if (elements.empty()) {
		cout << "Error: no highest-dimensional supported elements were kept from mesh_inp.dat!" << endl;
		return 0;
	}

	dim = max_dim;
	return 1;
}

int Mesher::Export_mesh_data_tecplot() {
	if (elements.empty()) {
		cout << "Error: no mesh data is available to export!" << endl;
		return 0;
	}

	ofstream otec("data\\mesh\\mesh_tec.dat");
	otec << "TITLE = Tecplot_Meshes" << endl;

	int any = 0;
	int iNum = int(elements[any].nodes_id.size());

	if (dim == 3 || iNum == 8) {
		otec << "VARIABLES = X, Y, Z" << endl;
		otec << "ZONE N=" << (int)nodes.size() << ", E=" << (int)elements.size()
			<< ", F=FEPOINT, ET=BRICK" << endl;

		for (int i = 0; i < (int)nodes.size(); ++i) {
			otec << nodes[i].x << "  " << nodes[i].y << "  " << nodes[i].z << endl;
		}
		otec << endl;

		for (int j = 0; j < (int)elements.size(); ++j) {
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
		otec << "VARIABLES = X, Y" << endl;
		string etType = (iNum == 3) ? "TRIANGLE" : "QUADRILATERAL";
		otec << "ZONE N=" << (int)nodes.size() << ", E=" << (int)elements.size()
			<< ", F=FEPOINT, ET=" << etType << endl;

		for (int i = 0; i < (int)nodes.size(); ++i) {
			otec << nodes[i].x << "  " << nodes[i].y << endl;
		}
		otec << endl;

		for (int j = 0; j < (int)elements.size(); ++j) {
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
	return 1;
}
