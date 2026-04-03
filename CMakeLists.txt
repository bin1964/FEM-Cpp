cmake_minimum_required(VERSION 3.16)

project(FEM_Cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(FEM_Cpp
    Fem2D.cpp
    Geometry2D.cpp
    Input.cpp
    main.cpp
    Matrix.cpp
    Mesher.cpp
    Post.cpp
    shape.cpp
    Slove.cpp
)

target_include_directories(FEM_Cpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_options(FEM_Cpp PRIVATE /W3 /utf-8)
else()
    target_compile_options(FEM_Cpp PRIVATE -Wall -Wextra -pedantic)
endif()
