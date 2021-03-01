# cmake-learn 3

本文基于官方cmake教程 [cmake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)



## 设置安装规则

安装规则相当简单。对于mathfunction库，我们设添加了这个库，通过将以下两行添加到mathfunction的CMakeLists.txt中来安装头文件和静态库:

```cmake
install (TARGETS MathFunctions DESTINATION bin)
install (FILES MathFunctions.h DESTINATION include)
```

然后根目录下的CMakeLusts.txt文件中添加如下行,用来安装可执行文件和配置文件：

```cmake
# add the install targets
install (TARGETS Tutorial DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h" DESTINATION include)
```

注意上边install的第一个参数和第三个参数。

`TARGETS`包含六种形式：`ARCHIVE`, `LIBRARY`, `RUNTIME`, `OBJECTS`, `FRAMEWORK`,  `BUNDLE`。Mathfunction安装的是`LIBRARY`，而根目录下的可执行文件是`RUNTIME`类型。

`FILE` 将给定的文件复制到指定目录。如果没有给定权限参数，则由该表单安装的文件默认为`OWNER_WRITE`、`OWNER_READ`、`GROUP_READ`和`WORLD_READ`。

TARGETS和FILE可指定为相对目录和绝对目录。

`DESTINATION`在这里是一个相对路径，取默认值。在unix系统中指向 `/usr/local` 在windows上`c:/Program Files/${PROJECT_NAME}`。

也可以通过设置`CMAKE_INSTALL_PREFIX`这个变量来设置安装的路径，那么安装位置不指向`/usr/local`，而指向你所指定的目录。

```
cmake ..
make
makeinstall

[ 50%] Built target MathFunctions
[100%] Built target Tutorial
Install the project...
-- Install configuration: ""
-- Installing: /usr/local/bin/Tutorial
-- Installing: /usr/local/include/TutorialConfig.h
-- Installing: /usr/local/bin/libMathFunctions.a
-- Installing: /usr/local/include/MathFunctions.h
```



##  添加测试

添加测试也非常简单，在根目录下的`CMakeLists.txt` 文件最后添加如下代码来测试输入参数后产生的结果是否正确

```cmake
enable_testing()

# does the application run
add_test(NAME Runs COMMAND Tutorial 25)

# does the usage message work?
add_test(NAME Usage COMMAND Tutorial)
set_tests_properties(Usage
  PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number"
  )

# define a function to simplify adding tests
function(do_test target arg result)
  add_test(NAME Comp${arg} COMMAND ${target} ${arg})
  set_tests_properties(Comp${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction(do_test)

# do a bunch of result based tests
do_test(Tutorial 4 "4 is 2")
do_test(Tutorial 9 "9 is 3")
do_test(Tutorial 5 "5 is 2.236")
do_test(Tutorial 7 "7 is 2.645")
do_test(Tutorial 25 "25 is 5")
do_test(Tutorial -25 "-25 is [-nan|nan|0]")
do_test(Tutorial 0.0001 "0.0001 is 0.01")
```

构建项目完成后，可以运行`ctest`命令行工具来运行测试。

第一个测试只用于验证应用程序是否正常运行，没有发生崩溃，返回值是0，这是CTest测试的基本形式。

接下来的几个测试都使用`PASS_REGULAR_EXPRESSION`测试属性来验证测试的输出是否包含特定的字符串。

编译后执行 `ctest` 即可

```shell
Test project /home/user/Desktop/cmakeTutorial/step3/build
    Start 1: Runs
1/7 Test #1: Runs .............................   Passed    0.00 sec
    Start 2: Usage
2/7 Test #2: Usage ............................   Passed    0.00 sec
    Start 3: Comp4
3/7 Test #3: Comp4 ............................   Passed    0.00 sec
    Start 4: Comp7
4/7 Test #4: Comp7 ............................   Passed    0.00 sec
    Start 5: Comp25
5/7 Test #5: Comp25 ...........................   Passed    0.00 sec
    Start 6: Comp-25
6/7 Test #6: Comp-25 ..........................   Passed    0.00 sec
    Start 7: Comp0.0001
7/7 Test #7: Comp0.0001 .......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 7

Total Test time (real) =   0.01 sec
```



## 添加系统自检

接下来向项目中添加一些代码，这些代码取决于目标平台可能不具备的特性。

这边使用`log()`和`exp()`函数。虽然，几乎每个平台都有这功能，我们在测试的时候先假设它不常见。

如果平台有`log()`和`exp()`，那么我们将使用它们来计算`mysqrt`函数中的平方根。我们在`MathFunctions/CMakeLists.txt`中使用`CheckFunctionExists`模块来测试这些函数的可用性文件. 在某些平台上，我们需要链接到`m`库。如果没有找到`log()`和`exp()`，则使用`m`库并重试。

```cmake
include(CheckFunctionExists)
check_function_exists(log "math.h" HAVE_LOG)
check_function_exists(exp "math.h" HAVE_EXP)
if(NOT (HAVE_LOG AND HAVE_EXP))
	unset(HAVE_LOG CACHE)
	unset(HAVE_EXP CACHE)
	set(CMAKE_REQUIRED_LIBRARIES "m")
	check_function_exists(log "math.h" HAVE_LOG)
	check_function_exists(exp "math.h" HAVE_EXP)
	if(HAVE_LOG AND HAVE_EXP)
		target_link_libraries(MathFunctions PRIVATE m)
	endif()
endif()

if(HAVE_LOG AND HAVE_EXP)
  target_compile_definitions(MathFunctions PRIVATE "HAVE_LOG" "HAVE_EXP")
endif()
```

修改`MySqrt.cpp`

```c++
#include <cmath>
#include <iostream>

#if defined(HAVE_LOG) && defined(HAVE_EXP)
  double result = exp(log(x) * 0.5);
  std::cout << "Computing sqrt of " << x << " to be " << result
            << " using log and exp" << std::endl;
#else
  double result = x;
....
#endif
```

编译验证通过

```shell
./Tutorial 10
Computing sqrt of 10 to be 3.16228 using log and exp
The square root of 10 is 3.16228
```









