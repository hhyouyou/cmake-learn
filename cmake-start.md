# cmake-learn 1 

本文基于官方cmake教程 [cmake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)



## 基本语法

一个最基础的`CmakeLists.txt` 文件至少需要包含一下三行

```cmake
cmake_minimum_required(VERSION 3.10)

# set the project name
project(Tutorial)

# add the executable 添加可执行文件
add_executable(Tutorial tutorial.cxx)
```

> ps:  不区分大小写
>
> cmake_minimum_required == CMAKE_MINIMUM_REQUIRED



创建一个`CalculateSqrt.cpp`文件,用于计算平方根

```c++
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 

int main(int argc, char* argv[]){ 
    if(argc<2){
        fprintf(stdout, "Uage: %s number\n", argv[0]); 
        return 1; 
    } 
    double inputValue = atof(argv[1]);
    double outputValue = sqrt(inputValue);
    fprintf(stdout, "The square root of %g is %g\n",inputValue, outputValue); 
    return 0; 
} 
```

## 构建程序

接下来新建一个文件夹 `build` 在该目录下编译运行

```shell
mkdir build
cd build
cmake ..
make
```

此时在`/build`目录下生成了可执行文件 `Tutorial`

```shell
./Tutorial 4                                                  
The square root of 4 is 2
```



## 添加版本号和配置头文件

首先修改 `CMakeLists.txt` 文件,使用 `project()` 命令设置项目名称和版本号

```cmake
# set the project name and version
project(Tutorial VERSION 1.0)
```

然后, 配置头文件以将版本号传递给源代码

```cmake
configure_file(TutorialConfig.h.in TutorialConfig.h)
```

指定项目编译的时候需要include的文件路径，`PROJECT_BINARY_DIR`变量为编译发生的目录，也就是make执行的目录，`PROJECT_SOURCE_DIR`为工程所在的目录 


```cmake
target_include_directories(Tutorial PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
```



在源码同级目录创建 `TutorialConfig.h.in` 文件, 内容如下:

```c++
// the configured options and settings for Tutorial
// @@引用的变量可以通过CMakeLists.txt来设置
#define Tutorial_VERSION_MAJOR @Tutorial_VERSION_MAJOR@
#define Tutorial_VERSION_MINOR @Tutorial_VERSION_MINOR@
```

同时需要在源代码中引入`#include`配置生成的头文件，即`TutorialConfig.h`

最后在`CalculateSqrt.cpp` 中加入打印刚才配置的版本号的代码

```c++
if (argc < 2){
    fprintf(stdout,"%s Version %d.%d\n",
            argv[0],
            Tutorial_VERSION_MAJOR,
            Tutorial_VERSION_MINOR);
    fprintf(stdout,"Usage: %s number\n",argv[0]);
    return 1;
}
```



然后执行

```shell
cmake ..
make 
./Tutorial
```

可以看到

```shell
./Tutorial Version 1.0
Usage: ./Tutorial number
```













