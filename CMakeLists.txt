cmake_minimum_required(VERSION 3.10)
project(eitor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


add_executable(eitor
    src/main.c
    src/buffer.c
    src/editor.c
    src/terminal.c
)


target_include_directories(eitor PRIVATE include)

if(MSVC)
    target_compile_options(eitor PRIVATE /W4)
else()
    target_compile_options(eitor PRIVATE -Wall -Wextra)
endif()