# cmake-learn 2

本文基于官方cmake教程 [cmake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)



## 构建自己的库

这个库将包含我们自己计算一个数字的平方根的计算方法。生成的程序可以使用这个库，而不是由编译器提供的标准平方根函数(`math.h`)。

接下来，我们将把库放到一个名为`MathFunctions`的子目录中,在工程目录下新建`MathFunctions`文件夹。这个文件夹中新建CMakeLists.txt文件，包含以下一行代码:

```cmake
add_library(MathFunctions MySqrt.cpp)
```

然后在这个文件夹中创建源文件`MySqrt.cpp`，它只有一个名为`mysqrt`的函数，与编译器的sqrt函数提供了类似的功能。

为了利用新库，我们在工程根目录下的`CMakeLists.txt`中添加`add_subdirectory(MathFunctions)`来构建我们自己的库。我们还添加了另一个include目录,以便`MathFunctions / MathFunctions.h`可以为函数原型找到头文件,该文件代码如下：

```c++
double mysqrt(double x);
```

然后创建 `MySqrt.cpp` 文件, 内容如下:

```c++
#include "MathFunctions.h"
#include <stdio.h>

// a hack square root calculation using simple operations
double mysqrt(double x) {
  if (x <= 0) {
    return 0;
  }

  double result;
  double delta;
  result = x;

  // do ten iterations
  int i;
  for (i = 0; i < 10; ++i) {
    if (result <= 0) {
      result = 0.1;
    }
    delta = x - (result * result);
    result = result + 0.5 * delta / result;
    fprintf(stdout, "Computing sqrt of %g to be %g\n", x, result);
  }
  return result;
}
```

最后将这个库添加到 根目录下的 `CMakeLists.txt`

```cmake
include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
add_subdirectory (MathFunctions) 
 
target_link_libraries (Tutorial MathFunctions)
```

接下来修改 `CalculateSqrt.cpp`   引入 

```c++
#include "MathFunctions.h"
```

使用新增的计算平方根的函数

```c++
// double outputValue = sqrt(inputValue);
double outputValue = mysqrt(inputValue);
```

编译运行

```shell
./Tutorial 4
Computing sqrt of 4 to be 2.5
Computing sqrt of 4 to be 2.05
Computing sqrt of 4 to be 2.00061
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
The square root of 4 is 2
```



## 构建可选项

当前文件目录如下 (`/build` 省略)

```shell
.
├── CalculateSqrt.cpp
├── CMakeLists.txt
├── MathFunctions
│   ├── CMakeLists.txt
│   ├── MathFunctions.h
│   └── MySqrt.cpp
└── TutorialConfig.h.in
```



`MathFunctions`是我们自己构建的库，有时候我们需要控制这个库是否应该使用，那么可以为使用这个库添加一个开关，在构建大型项目时非常有用。

在项目根目录下的`CMakeLists.txt`文件中添加如下代码：

```
# should we use our own math functions?
option (USE_MYMATH "Use tutorial provided math implementation" ON)
```

在`TutorialConfig.h.in`文件中添加以下内容：

```
#cmakedefine USE_MYMATH
```

修改根目录下`CMakeLists.txt`:

```cmake
# add the MathFunctions library?
if (USE_MYMATH)
  include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
  add_subdirectory (MathFunctions)
  set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)

target_link_libraries (Tutorial  ${EXTRA_LIBS})
```

使用`USE_MYMATH`的设置来确定是否应该编译和使用mathfunction库。使用一个变量(EXTRA_LIBS)来设置可选的库，然后将它们链接到可执行文件中。这是一种常见的方法，用于保持较大的项目具有许多可选组件。 



接下来修改`CalculateSqrt.cpp`  

引入`MathFunctions.h`改为可选

```c++
#ifdef USE_MYMATH
#include "MathFunctions.h"
#endif
```

调用开平方根的方法也改为可选

```c++
#ifdef USE_MYMATH
  double outputValue = mysqrt(inputValue);
#else
  double outputValue = sqrt(inputValue);
#endif
```



编译的时候可以通过添加参数,开启关闭 `USE_MYMATH`

```shell
cmake ..  -DUSE_MYMATH=OFF
cmake ..  -DUSE_MYMATH=ON
```



1. 使用自定义的库(USE_MYMATH=ON)

```
./Tutorial 4
Computing sqrt of 4 to be 2.5
Computing sqrt of 4 to be 2.05
Computing sqrt of 4 to be 2.00061
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
Computing sqrt of 4 to be 2
The square root of 4 is 2
```

2. 不适用自定义的库(USE_MYMATH=OFF)

```
./Tutorial 4
The square root of 4 is 2
```


