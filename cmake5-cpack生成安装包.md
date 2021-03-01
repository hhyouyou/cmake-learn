# cmake-learn 5

本文基于官方cmake教程 [cmake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)



假如想将我们的项目分发给其他人，以便他们可以安装并使用它。我们希望在各种平台上提供可执行文件或者源文件，构建支持二进制安装和包管理特性的安装包，如cygwin、debian、rpm等。为了实现这一点，我们将使用CPack来创建与CPack在章节包装中描述的平台特定安装程序。具体来说，我们需要在根目录CMakeLists.txt的底部添加几行代码：

```cmake
# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE  
     "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${Tutorial_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${Tutorial_VERSION_MINOR}")
include (CPack)
```

首先要加入`InstallRequiredSystemLibraries`。这个模块将包括当前平台项目所需要的任何运行时库。接下来，我们将一些CPack变量,如该项目的许可和版本信息。最后，我们将加入CPack模块，它将使用刚才定义的变量变量和系统的一些其他属性来设置安装程序。 我们执行外部构建并查看生成的文件：

```shell
tree -L 1
.
├── CMakeCache.txt
├── CMakeFiles
├── CPackConfig.cmake
├── CPackSourceConfig.cmake
├── CTestTestfile.cmake
├── Makefile
├── MathFunctions
├── TutorialConfig.h
└── cmake_install.cmake
```

注意其中两个生成的文件`CPackConfig.cmake`,`CPackSourceConfig.cmake`。

下一步是按照一般的方式构建项目，然后在其上运行CPack。要构建一个二进制版本，可以运行:

```
cpack --config CPackConfig.cmake
```



查看一下目录,可以看到生成了两个打包的文件`Tutorial-2.1-Linux.sh`,`Tutorial-2.1-Linux.tar.gz`。

```
tree -L 1
.
├── CMakeCache.txt
├── CMakeFiles
├── cmake_install.cmake
├── CPackConfig.cmake
├── _CPack_Packages
├── CPackSourceConfig.cmake
├── install_manifest.txt
├── Makefile
├── MathFunctions
├── Tutorial
├── Tutorial-2.1-Linux.sh
├── Tutorial-2.1-Linux.tar.gz
├── Tutorial-2.1-Linux.tar.Z
└── TutorialConfig.h
```

这个是已经编译好的问答压缩而成的，解压后可以直接运行。



要构建一个源代码版本，可以运行:

```
cpack --config CPackSourceConfig.cmake
```

多了一组`Tutorial-2.1-Source.tar.gz`, 解压后可以看到源码。

```shell
tar -tf Tutorial-2.1-Source.tar.gz
Tutorial-2.1-Source/CalculateSqrt.cpp
Tutorial-2.1-Source/TutorialConfig.h.in
Tutorial-2.1-Source/License.txt
Tutorial-2.1-Source/build/
Tutorial-2.1-Source/build/CMakeCache.txt
...
```

