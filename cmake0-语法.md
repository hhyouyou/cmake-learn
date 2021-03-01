#### cmake文件格式

本节讲的命令格式遵循如下语法：

| 格式        | 注释           |
| ----------- | -------------- |
| `<command>` | 必须填写的     |
| `[command]` | 可写也可不写的 |
| `a|b`       | a或者b均可以   |

cmake能识别CMakeLists.txt和*.cmake格式的文件。cmake能够以三种方式 来组织文件：

| 类别                  | 文件格式         |
| --------------------- | ---------------- |
| `Dierctory`（文件夹） | `CMakeLists.txt` |
| `Script`（脚本）      | `.cmake`         |
| `Module`（模块）      | `.cmake`         |

本系列主要以CMakeLists.txt的语法为主要内容。

##### 1. Directory

当CMake处理一个项目时，入口点是一个名为`CMakeLists.txt`的源文件，这个一定是根目录下的`CMakeLists.txt`。这个文件包含整个工程的构建规范，当我们有多个子文件夹需要编译时，使用`add_subdirectory()`命令来为构建添加子目录。添加的每个子目录也必须包含一个`CMakeLists.txt`文件作为该子目录的入口点。每个子目录的CMakeLists.txt文件被处理时，CMake在构建树中生成相应的目录作为默认的工作和输出目录。记住这一点非常关键，这样我们就可以使用**外部构建**了，而不必每次都使用蛋疼的内部构建，然后删除一堆文件才能从新构建。

##### 2. Script

一个单独的`.cmake`源文件可以使用cmake命令行工具`cmake -P .cmake`选项来执行脚本。脚本模式只是在给定的文件中运行命令，并且不生成构建系统。它不允许CMake命令定义或执行构建目标。

##### 3. Module

在Directory或Script中，CMake代码可以使用include()命令来加载.cmake。cmake内置了许多模块用来帮助我们构建工程，前边文章中提到的`CheckFunctionExists`。也可以提供自己的模块，并在`CMAKE_MODULE_PATH`变量中指定它们的位置。

#### cmake基本编写格式

先看一下定义的方式

| 名称               | 表达式                                       | 我认为的                              | 例子     |
| ------------------ | -------------------------------------------- | ------------------------------------- | -------- |
| space              | ``                                           | 任意个空格或者tab                     | a b      |
| newline            | ``                                           | 换行符                                | a\nb     |
| line_comment       | `'#' `                                       | 以'#"开头，不在'[]'块中，不包含换行符 | #bus     |
| separation         | `space\|newline`                             | 空格或者换行                          | a b=a\nb |
| lineending         | `linecomment?newline`                        | 从注释开头到换行符都不执行            |          |
| command_invocation | `space* identifier space* '(' arguments ')'` |                                       |          |
| quoted_argument    | `"quoted_element* "`                         | 用引号包裹的参数                      | "a"      |

文档看起来很蛋疼，我直接写一个最简单的

```cmake
add_executable(hello world.c foo.c) #这是一个注释
```

也等于

```cmake
add_executable(hello 
                world.c 
                foo.c)
#这是一个注释
```

就是这么easy。注意参数这一块，可以用引号包裹起来，这代表一个参数，假如一行不能写完，则用`\\`符号来表示连接成一行,也可以不用引号，但是假如参数带有分隔符，则会被认为是多个参数。

#### 定义变量

定义变量常用的函数是`set(KEY VALUE)`，取消定义变量是`unset(KEY)`。 它们的值始终是string类型的，有些命令可能将字符串解释为其他类型的值。变量名是区分大小写的，可能包含任何文本，但是我们建议只使用字母数字字符加上_和-这样的名称。

变量的作用域和java基本一致，不多做讲解。

变量引用的形式为`${variable_name}`，并在引用的参数或未引用的参数中进行判断。变量引用被变量的值替换，或者如果变量没有设置，则由空字符串替换。变量引用可以嵌套，并从内部进行替换，例如`${outer_${inner_variable}veriable}`。 环境变量引用的形式为$ENV{VAR}，并在相同的上下文中作为正常变量引用。

#### cmake构建系统

这算是进入正文了。扯淡的介绍就不多讲了，直接干货。

可执行文件和库是使用add_executable()和add_library()命令定义的。生成的二进制文件有合适的前缀、后缀和针对平台的扩展。二进制目标之间的依赖关系使用target_link_libraries()命令表示。

```cmake
add_library(archive archive.cpp zip.cpp lzma.cpp)
add_executable(zipapp zipapp.cpp)
target_link_libraries(zipapp archive)
```

在构建c程序的时候，因为要生成可执行文件，`add_executable`是必须的；构建安卓动态库的时候，`add_library`是必须的，因为jni需要调用动态库。

#### 外部构建

前边提到过cmake的构建命令`cmake .`,也就是在当前目录构建工程，这样会生成一系列的缓存文件在当前目录，假如我们需要重新构建需要删除这些文件，其中CMakeCache.txt是必须删除的，否则不会构建最新的程序。 看一下我们在工程根目录下执行`cmake`后是什么情况：

```shell
 ~/Desktop/Tutorial/Step1/ tree -L 1
.
├── CMakeCache.txt
├── CMakeFiles
├── CMakeLists.txt
├── Makefile
├── Tutorial
├── TutorialConfig.h
├── TutorialConfig.h.in
├── cmake_install.cmake
└── tutorial.cxx
```

生成的文件与源文件交叉在一起，相当混乱。我们可以采用外部构建来分隔源文件与生成的文件，当我们需要清空缓存重新构建项目时，就可以删除这个文件夹下的所有内容，重新运行构建命令，保持源文件的整洁，从而更容易管理项目。

首先新建一个文件夹`build`。这个文件夹就是我们用来存放生成的文件的目录，然后进入该目录，执行构建命令。

```shell
mkdir build # 创建build目录
cd build # 进入build目录
cmake .. # 因为程序入口构建文件在项目根目录下，采用相对路径上级目录来使用根目录下的构建文件
```

此时可以看到生成的文件全部在build文件夹下了，构建完全没问题。

```shell
 ~/Desktop/Tutorial/Step1/ tree -L 2
.
├── CMakeLists.txt
├── TutorialConfig.h.in
├── build
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   ├── Makefile
│   ├── TutorialConfig.h
│   └── cmake_install.cmake
└── tutorial.cxx
```

以后的项目讲解中将全部使用外部构建。

下面是我专门为讲解一些基本指令编写的代码

```cmake
cmake_minimum_required (VERSION 2.6)
project (Tutorial)
message(STATUS ${PROJECT_NAME})
message(STATUS ${PROJECT_SOURCE_DIR})
message(STATUS ${PROJECT_BINARY_DIR})
message(STATUS ${Tutorial_SOURCE_DIR})
message(STATUS ${Tutorial_BINARY_DIR})
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
  "${PROJECT_BINARY_DIR}/TutorialConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# add the executable
add_executable(Tutorial tutorial.cxx)
```

- 首先看message用法 message类似于一个向控制台输出日志的工具，但是功能又稍微强大一些，在一些模式下能够终止程序构建。

```cmake
message([<mode>] "message to display" ...)
```

mode有以下几个模式

| 模式           | 作用                                                         |
| -------------- | ------------------------------------------------------------ |
| (none)         | Important information                                        |
| STATUS         | Incidental information                                       |
| WARNING        | CMake Warning, continue processing                           |
| AUTHOR_WARNING | CMake Warning (dev), continue processing                     |
| SEND_ERROR     | CMake Error, continue processing,but skip generation         |
| FATAL_ERROR    | CMake Error, stop processing and generation                  |
| DEPRECATION    | CMake Deprecation Error or Warning if variable CMAKE_ERROR_DEPRECATED or CMAKE_WARN_DEPRECATED is enabled, respectively, else no message. |

STATUS我们经常用到。

- 来看第一行：

```
cmake_minimum_required (VERSION 2.6)
复制代码
```

为一个项目设置cmake的最低要求版本，并更新策略设置以匹配给定的版本(策略设置我永远也不会讲了)。无论是构建项目还是构建库，都需要这个命令。 它的语法是这样的

```cmake
cmake_minimum_required(VERSION major.minor[.patch[.tweak]]
                       [FATAL_ERROR])
```

版本号必须指定主次代号，后边的可选，请忽略可选项`[FATAL_ERROR]`，完全没用。

假如你指定的版本号大于你安装的cmake版本，将会停止构建并抛出一个错误:

```cmake
CMake Error at CMakeLists.txt:1 (cmake_minimum_required):
  CMake 3.11 or higher is required.  You are running version 3.10.2


-- Configuring incomplete, errors occurred!
```

**`cmake_minimum_required`必须在项目根目录下的最开始调用，也就是`project()`之前。在function()中调用该指令也可以，作用域将局限在函数内，但是必须以不影响全局使用为前提**

- 来看第二行

```cmake
project (Tutorial)
```

指定项目的名称为`Tutorial`，构建项目必须使用这个命令，构建库可以不指定。文档如下：

```cmake
project(<PROJECT-NAME> [LANGUAGES] [<language-name>...])
project(<PROJECT-NAME>
        [VERSION <major>[.<minor>[.<patch>[.<tweak>]]]]
        [DESCRIPTION <project-description-string>]
        [LANGUAGES <language-name>...])
```

设置项目名称并将该名称存储在`PROJECT_NAME`变量中。同时也指定了四个变量：

```cmake
PROJECT_SOURCE_DIR, <PROJECT-NAME>_SOURCE_DIR
PROJECT_BINARY_DIR, <PROJECT-NAME>_BINARY_DIR
```

但是我们一般只使用前一个，这样更容易更改。在上边的代码中我们用message输出了这些变量的信息，执行构建命令后日志输出：

```cmake
-- Tutorial
-- /Users/user/Desktop/Tutorial/Step1
-- /Users/user/Desktop/Tutorial/Step1/build
-- /Users/user/Desktop/Tutorial/Step1
-- /Users/user/Desktop/Tutorial/Step1/build
```

可以看到这几个变量确实输出了正确的值。

我们也可以在指定项目名称时直接指定版本号，假如没有指定，则版本号为空。 版本号存储在下边几个变量中:

```cmake
PROJECT_VERSION, <PROJECT-NAME>_VERSION
PROJECT_VERSION_MAJOR, <PROJECT-NAME>_VERSION_MAJOR
PROJECT_VERSION_MINOR, <PROJECT-NAME>_VERSION_MINOR
PROJECT_VERSION_PATCH, <PROJECT-NAME>_VERSION_PATCH
PROJECT_VERSION_TWEAK, <PROJECT-NAME>_VERSION_TWEAK
```

通常我们推荐使用前一个。现在测试一下，在CMakeLists.txt文件中修改代码：

```cmake
project (Tutorial
	VERSION 1.2.3
	DESCRIPTION "this is description"
	LANGUAGES CXX)
message(STATUS ${PROJECT_VERSION})
message(STATUS ${PROJECT_VERSION_MAJOR})
message(STATUS ${PROJECT_VERSION_MINOR})
message(STATUS ${PROJECT_VERSION_PATCH})
message(STATUS ${PROJECT_VERSION_TWEAK})
message(STATUS ${PROJECT_DESCRIPTION})
```

输出日志如下：

```shell
-- 1.2.3
-- 1
-- 2
-- 3
-- 
-- this is description
```

在这设置版本号和用set设置版本号效果一样，取最后一次设置的值。由于我们没有指定tweak版本，所以为空，同时看到description被存储到`PROJECT_DESCRIPTION`这个变量中了。

可以通过设置`LANGUAGES`来指定编程语言是C、CXX(即c++)或者Fortran等，如果没有设置此项，默认启用C和CXX。设置为NONE，或者只写`LANGUAGES`关键字而不写具体源语言，可以跳过启用任何语言。一般都是用cmake来编译c或者c++程序，所以用默认的就可以了。

- 来看重要的一行`configure_file`

该命令的作用是复制文件到另一个地方并修改文件内容。语法如下：

```cmake
configure_file(<input> <output>
               [COPYONLY] [ESCAPE_QUOTES] [@ONLY]
               [NEWLINE_STYLE [UNIX|DOS|WIN32|LF|CRLF] ])
```

将文件复制到文件中，并在输入文件内容中替换`@VAR@`或`${VAR}`的变量值。每个变量引用将被替换为变量的当前值，如果变量的值未被定义，则为空字符串。（VAR必须与cmakelist.txt中的变量保持一直，否则会生成注释）

input文件的定义形式为:

```cmake
#cmakedefine VAR ..
```

经过configure后生成的文件内容被替换为：

```cmake
#define VAR ...  //替换成功
/* #undef VAR */ //未定义的变量
```

生成的文件将会保留在'#'与'cmakedefine'之间的空格和制表符。

> 此处有一点需说明，现在clion默认使用cmake来构建程序，但是在clion中不支持`cmakedefine`关键字，所以可以直接使在input文件中填写`#define VAR ...`来编写宏定义，生成的结果与上边完全一样。clion有一个问题，就是直接用`cmakedefine`定义宏的时候假如#与cmakedefine之间有空格则不会替换`cmakedefine`为`define`，后边的变量会替换，但是不能编译成功，所以假如在clion中使用，要注意这几点，直接使用`#define`或者`#cmakedefine`，尽量不要加空格。

介绍其中的选项: input和output假如不指定绝对路径，则会被默认设置为`CMAKE_CURRENT_SOURCE_DIR`和`CMAKE_CURRENT_BINARY_DIR`，也就是项目根目录和构建的目录；

`COPYONLY`则只是复制文件，不替换任何东西，不能和`NEWLINE_STYLE `一起使用。

`ESCAPE_QUOTES`禁止为`"`转义。这个很蛋疼，不加这个命令的话假如变量中有`a\"b`，则在生成的文件中会直接使用转义后的字符`a"b`，加上指令后则按原来的文字显示`a\"b`；

`@ONLY`只允许替换`@VAR@`包裹的变量`${VAR}`则不会被替换；

`NEWLINE_STYLE `设置换行符格式

现在举个例子： foo.h.in文件如下

```cmake
#cmakedefine FOO_ENABLE
#cmakedefine FOO_STRING "@FOO_STRING@"
```

CMakeLists.txt中添加代码来设置一个开关，下边会执行if中的语句：

```cmake
option(FOO_ENABLE "Enable Foo" ON)
if(FOO_ENABLE)
  set(FOO_STRING "foo")
endif()
configure_file(foo.h.in foo.h @ONLY)
```

生成的文件foo.h

```c++
#define FOO_ENABLE
#define FOO_STRING "foo"
```

假如设置为off，`option(FOO_ENABLE "Enable Foo" ON)`,则不会执行if中的语句，生成的文件如下：

```cmake
/* #undef FOO_ENABLE */
/* #undef FOO_STRING */
```

- `include_directories`这一行

这句话的意思将当前的二进制目录添加到编译器搜索include目录中，这样就可以直接使用上一步生成的头文件了。

```cmake
include_directories([AFTER|BEFORE] [SYSTEM] dir1 [dir2 ...])
```

将给定的目录添加到编译器用来搜索包含文件的目录。相对路径为相对于当前根目录。

括号中的目录被添加到当前CMakeLists文件的`INCLUDE_DIRECTORIES`目录属性中。它们也被添加到当前CMakeLists文件中的每个目标的INCLUDE_DIRECTORIES目标属性中。。

默认情况下，指定的目录被追加到当前的include目录列表中。通过将`CMAKE_INCLUDE_DIRECTORIES_BEFORE`设置为ON，可以更改此默认行为。通过明确使用AFTER或BEFORE，您可以选择添加和预先设置。

如果给出SYSTEM选项，那么编译器会被告知这些目录在某些平台上是指系统包含的目录。



