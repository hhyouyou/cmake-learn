cmake_minimum_required(VERSION 3.10)

# set the project name and version 设置项目名 版本
project(Tutorial VERSION 2.1)

# should we use our own math functions?
option (USE_MYMATH "Use tutorial provided math implementation" ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable 添加可执行文件
add_executable(Tutorial CalculateSqrt.cpp)

# 配置头文件,来传递版本号给源码
configure_file(TutorialConfig.h.in TutorialConfig.h)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Tutorial PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )



# add the MathFunctions library?
if (USE_MYMATH)
  include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
  add_subdirectory (MathFunctions)
  set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)

target_link_libraries (Tutorial ${EXTRA_LIBS})