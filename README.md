# FEM_C++

一个面向教学、小规模算例和个人算法验证的二维有限元程序，使用 C++ 编写。项目当前已经支持基于 CMake 的构建方式，并提供了适合 Windows 用户的 Visual Studio 2022 预设，以及更通用的 Ninja 预设。

## 项目特点

- 从 `data/input.dat` 读取材料、几何、网格、载荷和位移边界条件
- 支持矩形区域上的结构化四边形网格和三角形网格生成
- 支持装配总体刚度矩阵并求解线弹性位移场
- 将网格与位移结果导出为 Tecplot 可读格式
- 提供 `CMakePresets.json`，便于命令行、IDE 和协作者统一构建流程

## 当前支持

### 单元类型

- `Qua4`：四节点四边形单元
- `Tri3`：三节点三角形单元
- `Read_mesh`：从外部网格文件读取网格的入口

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
- `Zone`
- `All_boundaries`

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

因此，如果你后续继续完善外部网格导入功能，建议也将相关网格输入文件放在 `data/` 目录下统一管理。

## 输入文件格式

程序默认读取 `data/input.dat`。以 `%` 开头的行为注释行。

常见输入段落如下：

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

示例：

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

## 输出文件说明

程序当前主要输出以下结果：

- `data/results/result_disp.dat`：位移结果，Tecplot 文本格式
- `data/mesh/mesh_tec.dat`：网格结果，Tecplot 文本格式

这些文件已经放在仓库的 `data/` 目录下，便于统一管理示例输入和示例结果。

## 已知限制

- 当前线性方程求解方式更适合教学和小规模算例，不适合大规模工程计算
- `Read_mesh` 路线的输入格式目前仍较固定
- 当前结果导出以 Tecplot 文本格式为主
- 当前实现主要面向二维问题

## 后续可扩展方向

- 引入更稳定和高效的线性方程求解器
- 增加应力、应变等后处理结果
- 支持更多单元类型
- 支持更通用的网格导入格式
- 增强跨平台构建与自动化测试能力
