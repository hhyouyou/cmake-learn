cmake_minimum_required(VERSION 3.10)

# 设置项目名称 版本号
project(CalculateSqrt VERSION 1.0)

option(USE_MYMATH "Use Tutorial provided math implementation" ON)

# configure_file 的作用是将一份文件拷贝到另一个位置并修改它的内容，时的在代码中使用Cmake中定义的变量
configure_file(CalculateSqrtConfig.h.in CalculateSqrtConfig.h)

# specify the C++ statndard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the MathFunctions library, Add a subdirectory to the build , 将新加的子目录用于build

if(USE_MYMATH)
	add_subdirectory(MathFunctions)
	list(APPEND EXTRA_LIBS MathFunctions)
	list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/MathFunctions")
endif(USE_MYMATH)


# add the executable
add_executable(CalculateSqrt calculateSqrt.cpp)

# 该指令的作用为将目标文件与库文件进行链接
target_link_libraries(CalculateSqrt PUBLIC MathFunctions)


# 指定项目编译时需要include的文件路径， PROJECT_BINARY_DIR变量为编译发生的目录，也就是make执行的目录， PROJECT_SOURCE_DIR为工程所在的目录
target_include_directories(CalculateSqrt PUBLIC 
							"${PROJECT_BINARY_DIR}"
							"${PROJECT_SOURCE_DIR}/MathFunctions"
							)
