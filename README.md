# Eitor - 一个简单的终端文本编辑器

## 项目简介
Eitor是一个基于C语言开发的轻量级终端文本编辑器，支持基本的文本编辑功能，包括插入、删除、复制粘贴等操作。编辑器采用Gap Buffer数据结构实现，具有高效的文本编辑性能。

## 功能特点
- 支持普通模式（Normal）、插入模式（Insert）和命令模式（Command）
- 使用vim风格的按键操作（hjkl移动光标，i进入插入模式等）
- 支持基本的文件操作（保存、退出）
- 支持行复制粘贴功能
- 跨平台支持（Windows/Linux/macOS）

## 编译环境要求
- CMake 3.10或更高版本
- C编译器（支持C11标准）
  - Windows: Visual Studio 2015或更高版本，或MinGW-w64
  - Linux/macOS: GCC或Clang

## 编译步骤

### Windows平台

#### 使用Visual Studio
1. 打开命令提示符或PowerShell，进入项目目录
2. 创建构建目录并进入：
   ```powershell
   mkdir build
   cd build
   ```
3. 生成Visual Studio项目文件：
   ```powershell
   cmake -G "Visual Studio 16 2019" ..
   ```
4. 使用Visual Studio打开生成的.sln文件，或直接命令行构建：
   ```powershell
   cmake --build . --config Release
   ```

#### 使用MinGW-w64
1. 打开命令提示符或PowerShell，进入项目目录
2. 创建构建目录并进入：
   ```powershell
   mkdir build
   cd build
   ```
3. 生成Makefile并构建：
   ```powershell
   cmake -G "MinGW Makefiles" ..
   cmake --build .
   ```

### Linux/macOS平台
1. 打开终端，进入项目目录
2. 创建构建目录并进入：
   ```bash
   mkdir build
   cd build
   ```
3. 生成Makefile并构建：
   ```bash
   cmake ..
   make
   ```

## 项目结构
```
.
├── CMakeLists.txt    # CMake构建配置文件
├── include/          # 头文件目录
│   ├── buffer.h      # Gap Buffer数据结构声明
│   ├── editor.h      # 编辑器核心功能声明
│   └── terminal.h    # 终端操作接口声明
├── src/             # 源文件目录
│   ├── buffer.c     # Gap Buffer实现
│   ├── editor.c     # 编辑器核心功能实现
│   ├── main.c       # 程序入口
│   └── terminal.c   # 终端操作实现
```

## 使用方法
1. 启动编辑器：
   ```bash
   ./eitor [文件名]
   ```

2. 基本操作：
   - `i`: 进入插入模式
   - `Esc`: 返回普通模式
   - `h/j/k/l`: 左/下/上/右移动光标
   - `x`: 删除当前字符
   - `yy`: 复制当前行
   - `p`: 粘贴
   - `:w`: 保存文件
   - `:q`: 退出（如有未保存修改则不会退出）
   - `:wq`: 保存并退出

## 核心数据结构
编辑器使用Gap Buffer作为核心数据结构，这是一种专门为文本编辑器设计的数据结构。Gap Buffer在当前编辑位置维护一个"间隙"，使得在该位置的插入和删除操作能够高效进行，时间复杂度为O(1)。当光标移动时，间隙会随之移动，这个操作的时间复杂度为O(n)，其中n为移动的距离。

## 贡献
本项目作为课设演示项目，不进行任何维护和支持。仅供学习参考，如果你希望基于此进行改进，请手动下载fork。