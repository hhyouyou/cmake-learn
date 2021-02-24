# cmake-learn 4

本文基于官方cmake教程 [cmake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)



在程序编译过程中生成文件，等文件生成后，再将该文件添加到构建过程中。

例如，我们在计算平方根程序中，提前计算一个常用平方根表来作为构建过程的一部分，然后将该表编译到我们的应用车给你需中，这样程序就可以通过查询该表来直接输出对应数值。

为了实现这一点，我们首先需要生成一个建立该表（文件）的程序。在`/MathFunctios` 的子目录中新建一个文件`MakeTable.cpp`， 内容如下：

```c++
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    int i;
    double result;

    // make sure we have enough arguments
    if (argc < 2) {
        return 1;
    }

    // open the output file
    FILE *fout = fopen(argv[1], "w");
    if (!fout) {
        return 1;
    }

    // create a source file with a table of square roots
    fprintf(fout, "double sqrtTable[] = {\n");
    for (i = 0; i < 10; ++i) {
        result = sqrt(static_cast<double>(i));
        fprintf(fout, "%g,\n", result);
    }

    // close the table with a zero
    fprintf(fout, "0};\n");
    fclose(fout);
    return 0;
}
```

> 该文件就是一个c++代码，接收一个文件路径的参数，新建文件并写入常用平方根结果，保存文件。

接下来我们将该文件在`CMakeLists.txt`中添加为可执行文件

```cmake
# first we add the executable that generates the table
add_executable(MakeTable MakeTable.cxx)
```

然后自定义一个命令，并让cmake在编译过程中，执行这个命令，即通过`COMMAND`来执行`MakeTable` 并输入参数 `${CMAKE_CURRENT_BINARY_DIR}/Table.h`。

```cmake
# add the command to generate the source code
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Table.h
  COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/Table.h
  DEPENDS MakeTable
  )
```

接下来我们要让`CMake`知道`mysqrt.cpp`依赖于生成的文件表(`Table.h`)。这是通过添加生成的`Table.h`文件到MathFunctions库来实现的。

```cmake
add_library(MathFunctions MySqrt.cpp ${CMAKE_CURRENT_BINARY_DIR}/Table.h)
```

我们还必须将当前的二进制目录添加到包含目录的列表中，因为生成的表在二进制目录中，这样库就可以找到并包含在`mysqrt.cpp`中

```cmake
# add the binary tree directory to the search path for include files
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
```

最后修改一下 `MySqrt.cpp`, 使用我们预先生成的平方根表

```c++
#include <Table.h>
#include <iostream>

// use the table to help find an initial value
if (x >= 1 && x < 10) {
    std::cout << "Use the table to help find an initial value " << std::endl;
    result = sqrtTable[static_cast<int>(x)];
}
```



接下来就是编译执行。

```
cmake ..
make
./Tutorial 3
Use the table to help find an initial value 
Computing sqrt of 3 to be 1.73205
The square root of 3 is 1.73205
```

也可以查看`/build/MathFunctions/Table.h`   这个文件

```shell
cat MathFunctions/Table.h
double sqrtTable[] = {
0,
1,
1.41421,
1.73205,
2,
2.23607,
2.44949,
2.64575,
2.82843,
3,
0};
```

















