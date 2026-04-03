#include "Input.h"

namespace {
// 将一整行字符串按空白符拆分为 token 列表。
vector<string> Tokenize(const string& line) {
	istringstream istr(line);
	vector<string> tokens;
	string token;
	while (istr >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

// 将字符串转换为 double 数值。
double ParseDouble(const string& token) {
	return stod(token);
}

// 判断 token 是否为合法载荷分量关键字。
bool IsLoadTypeToken(const string& token) {
	return token == "Force_x" || token == "Force_y" || token == "Force_z";
}

// 判断 token 是否为合法位移约束关键字。
bool IsDisplacementTypeToken(const string& token) {
	return token == "Fixed_displacement_x" ||
		token == "Fixed_displacement_y" ||
		token == "Fixed_displacement_z" ||
		token == "Pure_shear";
}

// 检查载荷作用域与参数个数是否匹配。
bool IsValidLoadDomain(const string& domain_type, size_t coef_count) {
	if (domain_type == "Point") {
		return coef_count == 2 || coef_count == 3;
	}
	if (domain_type == "Line") {
		return coef_count == 4;
	}
	if (domain_type == "Surface") {
		return coef_count == 4;
	}
	if (domain_type == "Zone") {
		return coef_count == 4 || coef_count == 6;
	}
	return false;
}

// 检查位移约束作用域与参数个数是否匹配。
bool IsValidDisplacementDomain(const string& domain_type, size_t coef_count) {
	if (domain_type == "All_boundaries") {
		return coef_count == 0;
	}
	return IsValidLoadDomain(domain_type, coef_count);
}

// 将载荷/位移方向关键字编码成分量掩码，便于做重复性检查。
int GetComponentMask(const string& token) {
	if (token == "Force_x" || token == "Fixed_displacement_x") {
		return 1;
	}
	if (token == "Force_y" || token == "Fixed_displacement_y") {
		return 2;
	}
	if (token == "Force_z" || token == "Fixed_displacement_z") {
		return 4;
	}
	return 0;
}
}

// 按关键字分段读取输入文件。
int Input::Read_Infile(ifstream& infile) {
	cout << "Reading input file..." << endl;

	while (!infile.eof()) {
		istringstream istr(Get_Line(infile));
		if (infile.eof()) {
			break;
		}

		string str_temp;
		istr >> str_temp;
		if (str_temp.empty()) {
			continue;
		}
		else if (str_temp == "Mat_Parameter") {
			if (Read_mat_para(mat_para, infile) == 0) {
				return 0;
			}
		}
		else if (str_temp == "RVE_Geometry") {
			if (Read_geom_rve(geom_rve, infile) == 0) {
				return 0;
			}
		}
		else if (str_temp == "Element_Type" || str_temp == "ElementType") {
			if (Read_element_type(element_type, infile) == 0) {
				return 0;
			}
		}
		else if (str_temp == "Grid_Size") {
			if (Read_grid_size(grid_size, infile) == 0) {
				return 0;
			}
		}
		else if (str_temp == "Load") {
			if (Read_load(load, infile) == 0) {
				return 0;
			}
		}
		else if (str_temp == "Displacement") {
			if (Read_displacement(displace, infile) == 0) {
				return 0;
			}
		}
		else {
			cout << "the key words \"" << str_temp << "\" is not defined!" << endl;
			return 0;
		}
	}

	cout << "^_^ Keywords loaded successfully!" << endl;

	if (!mat_para.mark) {
		cout << "Attention: \"Mat_Parameter\" will use default parameters!" << endl;
	}
	if (!geom_rve.mark) {
		cout << "Attention: \"RVE_Geometry\" will use default parameters!" << endl;
	}
	if (!element_type.mark) {
		cout << "Attention: \"Element_Type\" will use default parameters!" << endl;
	}
	if (!grid_size.mark) {
		cout << "Attention: \"Grid_Size\" will use default parameters!" << endl;
	}
	if (!load.mark) {
		cout << "Attention: \"Load\" will use default parameters!" << endl;
	}
	if (!displace.mark) {
		cout << "Attention: \"Displacement\" will use default parameters!" << endl;
	}

	return 1;
}

// 初始化输入对象的默认参数。
int Input::Data_Initialization() {
	mat_para.keywords = "Mat_Parameter";
	mat_para.mark = false;
	mat_para.E = 100;
	mat_para.v = 0.25;

	geom_rve.keywords = "RVE_Geometry";
	geom_rve.mark = false;
	geom_rve.origin[0] = 0.0;
	geom_rve.origin[1] = 0.0;
	geom_rve.origin[2] = 0.0;
	geom_rve.len_x = 1.0;
	geom_rve.wid_y = 1.0;
	geom_rve.hei_z = 0.0;

	element_type.keywords = "Element_Type";
	element_type.mark = false;
	element_type.eletype = EleType::Quad;

	grid_size.keywords = "Grid_Size";
	grid_size.mark = false;
	grid_size.delta_x = 0.1;
	grid_size.delta_y = 0.1;
	grid_size.delta_z = 0.0;

	load.keywords = "Load";
	load.mark = false;
	load.num = 0;

	displace.keywords = "Displacement";
	displace.mark = false;
	displace.num = 0;

	cout << "^_^ Data initialization completed" << endl << endl;
	return 1;
}

// 读取几何参数，可兼容二维和三维输入格式。
int Input::Read_geom_rve(struct Geom_RVE& geom_rve, ifstream& infile) {
	if (geom_rve.mark) {
		cout << "Attention: \"" << geom_rve.keywords << "\" has been input!" << endl;
		return 0;
	}
	geom_rve.mark = true;

	vector<string> tokens = Tokenize(Get_Line(infile));
	if (tokens.size() != 4 && tokens.size() != 6) {
		cout << "Error: the geometry of RVE should contain 4 values for 2D or 6 values for 3D!" << endl;
		return 0;
	}

	geom_rve.origin[0] = ParseDouble(tokens[0]);
	geom_rve.origin[1] = ParseDouble(tokens[1]);
	geom_rve.origin[2] = 0.0;
	geom_rve.len_x = ParseDouble(tokens[2]);
	geom_rve.wid_y = ParseDouble(tokens[3]);
	geom_rve.hei_z = 0.0;
	if (tokens.size() == 6) {
		geom_rve.origin[2] = ParseDouble(tokens[2]);
		geom_rve.len_x = ParseDouble(tokens[3]);
		geom_rve.wid_y = ParseDouble(tokens[4]);
		geom_rve.hei_z = ParseDouble(tokens[5]);
	}

	if (geom_rve.len_x < 0 || geom_rve.wid_y < 0 || geom_rve.hei_z < 0) {
		cout << "Error: the sizes of RVE should be positive!" << endl;
		return 0;
	}

	return 1;
}

// 读取网格步长，可兼容二维和三维输入格式。
int Input::Read_grid_size(struct Grid_size& grid_size, ifstream& infile) {
	if (grid_size.mark) {
		cout << "Attention: \"" << grid_size.keywords << "\" has been input!" << endl;
		return 0;
	}
	grid_size.mark = true;

	vector<string> tokens = Tokenize(Get_Line(infile));
	if (tokens.size() != 2 && tokens.size() != 3) {
		cout << "Error: the grid size should contain 2 values for 2D or 3 values for 3D!" << endl;
		return 0;
	}

	grid_size.delta_x = ParseDouble(tokens[0]);
	grid_size.delta_y = ParseDouble(tokens[1]);
	grid_size.delta_z = 0.0;
	if (tokens.size() == 3) {
		grid_size.delta_z = ParseDouble(tokens[2]);
	}

	if (grid_size.delta_x < 0 || grid_size.delta_y < 0 || grid_size.delta_z < 0) {
		cout << "Error: the size of grid in \"" << grid_size.keywords << "\" is less than 0!" << endl;
		return 0;
	}

	return 1;
}

// 读取单元类型关键字。
int Input::Read_element_type(struct ElementType& element_type, ifstream& infile) {
	if (element_type.mark) {
		cout << "Attention: \"" << element_type.keywords << "\" has been input!" << endl;
		return 0;
	}
	element_type.mark = true;

	string str_temp;
	istringstream istr(Get_Line(infile));
	istr >> str_temp;

	if (str_temp == "Qua4") {
		element_type.eletype = EleType::Quad;
	}
	else if (str_temp == "Tri3") {
		element_type.eletype = EleType::Tria;
	}
	else if (str_temp == "Hex8") {
		element_type.eletype = EleType::Hexa8;
	}
	else if (str_temp == "Read_mesh") {
		element_type.eletype = EleType::Read_Mesh;
	}
	else {
		cout << "Error: the type of element is not defined!" << endl;
		return 0;
	}

	return 1;
}

// 读取材料参数。
int Input::Read_mat_para(struct Mat_para& mat_para, ifstream& infile) {
	if (mat_para.mark) {
		cout << "Attention: \"" << mat_para.keywords << "\" has been input!" << endl;
		return 0;
	}
	mat_para.mark = true;

	istringstream istr(Get_Line(infile));
	istr >> mat_para.E >> mat_para.v;

	if (mat_para.E < 0 || mat_para.v < 0) {
		cout << "Error: the parameters of material in \"" << mat_para.keywords << "\" is less than 0!" << endl;
		return 0;
	}

	return 1;
}

// 读取载荷定义并保存为统一的数据结构。
int Input::Read_load(struct Load& load, ifstream& infile) {
	if (load.mark) {
		cout << "Attention: \"" << load.keywords << "\" has been input!" << endl;
		return 0;
	}
	load.mark = true;

	istringstream istr0(Get_Line(infile));
	istr0 >> load.num;

	for (int i = 0; i < load.num; i++) {
		vector<string> tokens = Tokenize(Get_Line(infile));
		if (tokens.empty()) {
			cout << "Error: the load line is empty!" << endl;
			return 0;
		}

		const string domain_type = tokens[0];
		load.domain_type.push_back(domain_type);

		size_t type_begin = 1;
		while (type_begin < tokens.size() && !IsLoadTypeToken(tokens[type_begin])) {
			++type_begin;
		}

		vector<double> coef;
		for (size_t j = 1; j < type_begin; ++j) {
			coef.push_back(ParseDouble(tokens[j]));
		}
		if (!IsValidLoadDomain(domain_type, coef.size())) {
			cout << "Error: selected type for loaded area is not defined!" << endl;
			return 0;
		}
		load.coef.push_back(coef);

		vector<string> load_type;
		vector<double> value;
		int sign = 0;
		if (type_begin >= tokens.size() || (tokens.size() - type_begin) % 2 != 0) {
			cout << "Error: force conditions are not input or excessive!" << endl;
			return 0;
		}

		for (size_t j = type_begin; j < tokens.size(); j += 2) {
			const string& token = tokens[j];
			if (!IsLoadTypeToken(token)) {
				cout << "Error: the force type is not defined!" << endl;
				return 0;
			}

			const int mask = GetComponentMask(token);
			if ((sign & mask) != 0) {
				cout << "Error: the force type is repeated!" << endl;
				return 0;
			}

			sign |= mask;
			load_type.push_back(token);
			value.push_back(ParseDouble(tokens[j + 1]));
		}

		if (value.empty() || value.size() > 3 || value.size() != load_type.size()) {
			cout << "Error: force conditions are not input or excessive!" << endl;
			return 0;
		}

		load.load_type.push_back(load_type);
		load.value.push_back(value);
	}

	return 1;
}

// 读取位移约束定义并保存为统一的数据结构。
int Input::Read_displacement(struct Displace& displace, ifstream& infile) {
	if (displace.mark) {
		cout << "Attention: \"" << displace.keywords << "\" has been input!" << endl;
		return 0;
	}
	displace.mark = true;

	istringstream istr0(Get_Line(infile));
	istr0 >> displace.num;

	for (int i = 0; i < displace.num; i++) {
		vector<string> tokens = Tokenize(Get_Line(infile));
		if (tokens.empty()) {
			cout << "Error: the displacement line is empty!" << endl;
			return 0;
		}

		const string domain_type = tokens[0];
		displace.domain_type.push_back(domain_type);

		size_t type_begin = 1;
		while (type_begin < tokens.size() && !IsDisplacementTypeToken(tokens[type_begin])) {
			++type_begin;
		}

		vector<double> coef;
		for (size_t j = 1; j < type_begin; ++j) {
			coef.push_back(ParseDouble(tokens[j]));
		}
		if (!IsValidDisplacementDomain(domain_type, coef.size())) {
			cout << "Error: selected type for displacement area is not defined!" << endl;
			return 0;
		}
		displace.coef.push_back(coef);

		vector<string> disp_type;
		vector<double> value;
		int sign = 0;
		if (type_begin >= tokens.size() || (tokens.size() - type_begin) % 2 != 0) {
			cout << "Error: displacement conditions are not input or excessive!" << endl;
			return 0;
		}

		for (size_t j = type_begin; j < tokens.size(); j += 2) {
			const string& token = tokens[j];
			if (!IsDisplacementTypeToken(token)) {
				cout << "Error: selected type for displacement condition is not defined!" << endl;
				return 0;
			}

			if (token == "Pure_shear") {
				if (j != type_begin || tokens.size() != type_begin + 2) {
					cout << "Error: \"Pure_shear\" is not the only displacement condition!" << endl;
					return 0;
				}
				disp_type.push_back(token);
				value.push_back(ParseDouble(tokens[j + 1]));
				break;
			}

			const int mask = GetComponentMask(token);
			if ((sign & mask) != 0) {
				cout << "Error: the displacement type is repeated!" << endl;
				return 0;
			}

			sign |= mask;
			disp_type.push_back(token);
			value.push_back(ParseDouble(tokens[j + 1]));
		}

		if (value.empty() || value.size() > 3 || value.size() != disp_type.size()) {
			cout << "Error: displacement conditions are not input or excessive!" << endl;
			return 0;
		}

		displace.disp_type.push_back(disp_type);
		displace.value.push_back(value);
	}

	return 1;
}

// 读取一行有效内容，并自动跳过注释行。
string Input::Get_Line(ifstream& infile) const {
	string s;
	getline(infile, s);
	while (!infile.eof() && s.substr(0, 1) == "%") {
		getline(infile, s);
	}
	return s;
}
