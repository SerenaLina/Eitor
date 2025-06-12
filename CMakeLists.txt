cmake_minimum_required(VERSION 3.10)
project(eitor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加源文件
add_executable(eitor
    src/main.c
    src/buffer.c
    src/editor.c
    src/terminal.c
)

# 包含头文件目录
target_include_directories(eitor PRIVATE include)

# 设置编译选项
if(MSVC)
    target_compile_options(eitor PRIVATE /W4)
else()
    target_compile_options(eitor PRIVATE -Wall -Wextra)
endif()