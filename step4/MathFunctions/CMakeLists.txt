add_library(MathFunctions MySqrt.cpp ${CMAKE_CURRENT_BINARY_DIR}/Table.h)
add_executable(MakeTable MakeTable.cpp)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Table.h
        COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/Table.h
        DEPENDS MakeTable
)

# add the binary tree directory to the search path for
# include files
include_directories(${CMAKE_CURRENT_BINARY_DIR})