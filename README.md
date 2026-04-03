# FEM_C++

一个面向教学、小规模算例和个人算法验证的有限元程序，使用 C++ 编写。项目当前以二维问题为主，同时已经支持三维 `Hex8` 八节点六面体单元的求解流程。仓库提供了基于 CMake 的构建方式，并附带适合 Windows 用户的 Visual Studio 2022 预设，以及更通用的 Ninja 预设。

## 项目特点

- 从 `data/input.dat` 读取材料、几何、网格、载荷和位移边界条件
- 支持二维矩形区域上的结构化四边形网格和三角形网格生成
- 支持三维 `Hex8` 单元，并可通过外部网格文件导入三维网格
- 支持二维与三维问题的总体刚度矩阵装配和位移求解
- 将网格与位移结果导出为 Tecplot 可读格式
- 提供 `CMakePresets.json`，便于命令行、IDE 和协作者统一构建流程

## 当前支持

### 单元类型

- `Qua4`：二维四节点四边形单元
- `Tri3`：二维三节点三角形单元
- `Hex8`：三维八节点六面体单元
- `Read_mesh`：从外部网格文件读取网格的入口

### 网格来源

- 二维 `Qua4`：可自动生成矩形区域结构化四边形网格
- 二维 `Tri3`：可自动生成矩形区域结构化三角形网格
- 三维 `Hex8`：当前通过 `Read_mesh` 路线从外部网格文件导入

这里特别说明一下：当前仓库已经具备三维 `Hex8` 单元求解、三维结果输出、三维载荷和三维位移约束的支持，但三维网格并不是在程序内部自动生成，而是通过读取 `data/mesh_inp.dat` 导入。

### 输入参数

- `Mat_Parameter`
- `RVE_Geometry`
- `Element_Type`
- `Grid_Size`
- `Load`
- `Displacement`

### 约束与加载区域

- `Point`
- `Line`
- `Surface`
- `Zone`
- `All_boundaries`

其中：

- `Line` 主要用于二维问题
- `Surface` 主要用于三维 `Hex8` 网格表面载荷或表面位移约束
- `Zone` 可用于二维区域或三维体区域

## 项目结构

- `main.cpp`：程序入口，串联输入、网格、求解与后处理
- `Input.*`：输入文件解析
- `Mesher.*`：网格生成、网格读取与网格导出
- `shape.*`：单元定义与单元刚度计算
- `Slove.*`：总体刚度组装、边界条件施加与线性方程求解
- `Post.*`：结果导出
- `Matrix.*`：自定义矩阵类
- `Geometry2D.*`：二维几何辅助类
- `Fem2D.*`：节点、单元、边等基础数据结构
- `data/`：输入文件和示例输出目录

## 环境要求

- CMake 3.21 及以上
- 支持 C++17 的编译器
- Windows 下推荐使用 Visual Studio 2022
- 如果使用 Ninja 预设，需要本机可用的 `ninja` 和对应编译器

## 获取项目

```bash
git clone https://github.com/bin1964/FEM-Cpp.git
cd FEM-Cpp
```

## 编译方式

### 方式一：使用 CMake Presets

这是当前仓库最推荐的构建方式。你可以先查看仓库内可用的预设：

```powershell
cmake --list-presets
```

当前主要预设包括：

- `vs2022`：Windows 下使用 Visual Studio 2022 生成器
- `ninja-debug`：使用 Ninja 生成 Debug 构建
- `ninja-release`：使用 Ninja 生成 Release 构建

#### Visual Studio 2022

配置：

```powershell
cmake --preset vs2022
```

编译 Debug：

```powershell
cmake --build --preset vs2022-debug
```

编译 Release：

```powershell
cmake --build --preset vs2022-release
```

#### Ninja

配置 Debug：

```powershell
cmake --preset ninja-debug
```

编译 Debug：

```powershell
cmake --build --preset ninja-debug-build
```

配置 Release：

```powershell
cmake --preset ninja-release
```

编译 Release：

```powershell
cmake --build --preset ninja-release-build
```

### 方式二：通用 CMake 命令

如果你不想使用预设，也可以直接使用标准 CMake 命令：

```powershell
cmake -S . -B build
cmake --build build
```

如果你使用的是多配置生成器，例如 Visual Studio，通常需要显式指定配置：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

## 运行方式

请从仓库根目录运行程序。当前程序默认读取 `data/input.dat`，并将结果写入 `data/mesh/` 和 `data/results/`。

Visual Studio 2022 Debug 构建完成后可运行：

```powershell
.\build\vs2022\Debug\FEM_Cpp.exe
```

Ninja Debug 构建完成后可运行：

```powershell
.\build\ninja-debug\FEM_Cpp.exe
```

## 数据目录说明

当前仓库采用统一的 `data/` 目录组织输入和输出文件：

- `data/input.dat`：程序默认读取的输入文件
- `data/mesh/mesh_tec.dat`：导出的网格 Tecplot 文件
- `data/results/result_disp.dat`：导出的位移结果 Tecplot 文件

如果使用 `Read_mesh` 路线，程序当前还会尝试读取：

- `data/mesh_inp.dat`

因此，如果你使用三维 `Hex8` 算例，通常需要先准备 `data/mesh_inp.dat` 作为外部网格输入文件。

## 输入文件格式

程序默认读取 `data/input.dat`。以 `%` 开头的行为注释行。

常见输入段落如下：

```txt
Mat_Parameter
E v

RVE_Geometry
x0 y0 len_x wid_y
或
x0 y0 z0 len_x wid_y hei_z

Element_Type
Qua4 | Tri3 | Hex8 | Read_mesh

Grid_Size
dx dy
或
dx dy dz

Load
n
...

Displacement
n
...
```

### 二维输入示例

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

上面这个算例表示：

- 左边界固定 `x` 和 `y` 位移
- 右边界施加 `x` 方向位移

### 三维输入说明

三维问题通常配合 `Hex8` 或 `Read_mesh` 使用，输入中会涉及：

- 三维几何参数：`x0 y0 z0 len_x wid_y hei_z`
- 三维网格尺度：`dx dy dz`
- 三维载荷分量：`Force_x`、`Force_y`、`Force_z`
- 三维位移约束：`Fixed_displacement_x`、`Fixed_displacement_y`、`Fixed_displacement_z`
- 三维作用区域：`Point`、`Surface`、`Zone`

示意格式例如：

```txt
Load
1
Surface 1 0 0 -1 Force_z -1000

Displacement
1
Surface 1 0 0 0 Fixed_displacement_x 0 Fixed_displacement_y 0 Fixed_displacement_z 0
```

这里的 `Surface a b c d` 表示平面方程：

```txt
a*x + b*y + c*z + d = 0
```

程序会在 `Hex8` 网格中寻找落在该平面上的单元面，并对这些单元面施加载荷或位移约束。

## 输出文件说明

程序当前主要输出以下结果：

- `data/results/result_disp.dat`：位移结果，Tecplot 文本格式
- `data/mesh/mesh_tec.dat`：网格结果，Tecplot 文本格式

其中：

- 二维结果会输出 `X, Y, Ux, Uy`
- 三维 `Hex8` 结果会输出 `X, Y, Z, Ux, Uy, Uz`

## 三维支持现状

目前三维能力已经覆盖以下环节：

- `Hex8` 八节点六面体单元刚度计算
- 三维总体刚度矩阵装配
- `Force_z` 和 `Fixed_displacement_z`
- 三维 `Surface` 和三维 `Zone` 条件
- 三维网格 Tecplot 导出
- 三维位移结果 Tecplot 导出

当前仍需注意：

- 三维网格当前主要依赖外部 `mesh_inp.dat` 导入
- 仓库内二维流程比三维流程更成熟
- 文档示例目前以二维算例为主，三维示例还可以继续补充

## 已知限制

- 当前线性方程求解方式更适合教学和小规模算例，不适合大规模工程计算
- `Read_mesh` 路线的输入格式目前仍较固定
- 三维网格目前主要通过外部文件导入，而不是程序内部自动剖分
- 当前结果导出以 Tecplot 文本格式为主

## 后续可扩展方向

- 引入更稳定和高效的线性方程求解器
- 增加应力、应变等后处理结果
- 完善三维算例和三维输入模板
- 支持更多单元类型
- 支持更通用的网格导入格式
- 增强跨平台构建与自动化测试能力
