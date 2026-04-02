# FEM_C++

一个面向教学和小规模算例的二维有限元程序，使用 C++ 编写，当前工程以 Visual Studio / MSBuild 为主要构建方式。

程序目前可以完成以下流程：

- 读取 `input.dat` 中的材料、几何、网格、载荷和位移约束参数
- 生成矩形区域的结构化四边形或三角形网格
- 构造单元对象并组装总体刚度矩阵
- 施加位移边界条件和外载荷
- 求解线弹性二维问题的位移场
- 将结果导出为 Tecplot 可读格式

## 1. 当前功能

### 支持的单元

- `Qua4`：四节点四边形单元
- `Tri3`：三节点三角形单元

### 支持的网格方式

- 自动生成矩形区域四边形网格
- 自动生成矩形区域三角形网格
- 从文件读取网格（当前代码中保留了 `Read_mesh` 入口）

### 支持的输入参数

- 材料参数 `Mat_Parameter`
- 几何区域 `RVE_Geometry`
- 单元类型 `Element_Type`
- 网格尺寸 `Grid_Size`
- 外载荷 `Load`
- 位移约束 `Displacement`

### 支持的约束与载荷区域

- `Point`
- `Line`
- `Zone`
- `All_boundaries`（用于纯剪切边界）

## 2. 项目结构

- `main.cpp`：主程序入口，串联输入、网格、求解与后处理
- `Input.*`：输入文件解析
- `Mesher.*`：网格生成与网格导出
- `shape.*`：单元类定义与单元刚度计算
- `Slove.*`：总体刚度组装、边界条件施加与线性方程求解
- `Post.*`：结果导出
- `Matrix.*`：自定义矩阵类
- `Geometry2D.*`：二维几何辅助类
- `Fem2D.*`：节点、单元、边等基础数据结构

## 3. 编译方式

推荐使用 Windows + Visual Studio 2022。

### 方式一：Visual Studio

1. 打开 `FEM_C++.sln`
2. 选择 `Debug | x64`
3. 直接生成并运行

### 方式二：VS Code

仓库内已提供 `.vscode/launch.json` 和 `.vscode/tasks.json`，可直接使用 VS Code 的“运行和调试”按钮。

默认流程为：

- 先调用 `MSBuild` 编译 `Debug x64`
- 再启动 `x64\Debug\FEM_C++.exe`

### 方式三：命令行

```powershell
"C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" FEM_C++.vcxproj /p:Configuration=Debug /p:Platform=x64
```

编译成功后运行：

```powershell
.\x64\Debug\FEM_C++.exe
```

## 4. 输入文件说明

程序默认读取根目录下的 `input.dat`。

注释行以 `%` 开头。

当前常用关键字格式如下：

```txt
Mat_Parameter
E v

RVE_Geometry
x0 y0 len_x wid_y

Element_Type
Qua4 | Tri3 | Read_mesh

Grid_Size
dx dy

Load
n
...

Displacement
n
...
```

### 示例

下面是一个三角形网格拉伸算例示例：

```txt
Mat_Parameter
2E11 0.333

RVE_Geometry
0 0 1 1

Element_Type
Tri3

Grid_Size
0.05 0.05

Displacement
2
Line 0 0 0 1 Fixed_displacement_x 0.0 Fixed_displacement_y 0.0
Line 1 0 1 1 Fixed_displacement_x 0.01
```

说明：

- 左边界固定 `x`、`y` 位移
- 右边界施加 `x` 方向位移
- 若未设置 `Load`，程序会提示使用默认值

## 5. 输出文件

程序当前会输出以下结果：

- `result_disp.dat`：位移结果，Tecplot 格式
- `mesh\mesh_tec.dat`：网格结果，Tecplot 格式

说明：

- 若需要导出网格文件，请确保工程目录下存在 `mesh` 文件夹

## 6. 已知限制

- 当前求解器使用显式矩阵求逆 `gK.inverse() * gF`，更适合教学和小规模问题，不适合大规模工程计算
- `Read_mesh()` 的读取格式较固定，当前实现主要面向特定网格文件格式
- 结果导出目前以 Tecplot 文本格式为主
- 代码当前面向二维问题

## 7. 后续可扩展方向

- 引入更稳定和高效的线性方程求解器
- 增加应力、应变后处理输出
- 支持更多单元类型
- 支持更通用的网格导入格式
- 增加 CMake 构建支持，方便跨平台使用

## 8. 说明

这个项目目前更适合：

- 有限元课程学习
- 小型数值实验
- 个人算法验证
- 后续继续扩展成更完整的二维有限元程序
