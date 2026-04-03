#include "shape.h"
#include "Matrix.h"
#include <fstream>
#include <string>
#include <sstream>
#include "Mesher.h"
#include "Input.h"
#include "Slove.h"
#include "Post.h"
#include "Geometry2D.h"

using namespace std;

int main (){
	Input input_data;
	Mesher mesh_data;
	vector<unique_ptr<ShapeSpace::ShapeInterface>> shape;	//存放单元对象的指针
	Slove slove_data;
	Post pot_data;

	cout << "------------有限元分析程序开始运行-----------" << endl;
	cout << "------------读取输入文件并生成网格-----------" << endl;
	const char* inputfile = "data\\input.dat";
	ifstream in_file(inputfile);
	if (!in_file.is_open()) {
		cout << "Error: input.dat文件无法打开!" << endl;
		return 0;
	}
	input_data.Data_Initialization();//初始化成功，会输出信息
	if(input_data.Read_Infile(in_file) == 0) return 0;
	if(mesh_data.Generate_Mesh(input_data.element_type, input_data.geom_rve, input_data.grid_size) == 0) return 0;

	cout << "------------构造单元对象-----------" << endl;
	if(ShapeSpace::Generate_Element_Classes(shape, input_data.mat_para, mesh_data) == 0) return 0;

	cout << "------------生成刚度矩阵并施加约束-----------" << endl;
	if(slove_data.Fixed_Displacement_Constraints(input_data.displace, mesh_data.nodes) == 0) return 0;
	if(slove_data.Assemble_Global_Stiff(shape) == 0) return 0;
	if(slove_data.Apply_Load_to_Force_Vector(input_data.load, mesh_data.nodes, mesh_data.elements) == 0) return 0;
	if(slove_data.Apply_Constraint_to_Stiff() == 0) return 0;
	if(slove_data.Slove_Linear_System() == 0) return 0;

	cout << "------------输出结果-----------" << endl;
	if(pot_data.Output_Disp_Tecplot(mesh_data.nodes, mesh_data.elements, slove_data.u) == 0) return 0;

	cout << "------------有限元分析程序运行结束-----------" << endl;
	return 0;
}
