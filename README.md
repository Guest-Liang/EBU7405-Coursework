# EBU7405-Coursework
`cw/includes`、`cw/src`、`cw/lib`均来自GitHub Repo: [LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL)

# Ubuntu
在Ubuntu下编译时，需要先[下载GLFW3.4源代码](https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip)手动编译一下Ubuntu系统所需的glfw库(.a文件)，说明文档[Compiling GLFW](https://www.glfw.org/docs/latest/compile.html)   
或者可以直接用lib文件夹下我编译好的libglfw3.a文件   
如果自己编译GLFW，记得将编译好的libglfw3.a文件放到lib文件夹下   

更新一下
```bash
sudo apt update
```
安装所需库
```bash
sudo apt install -y build-essential cmake libgl1-mesa-dev libxinerama-dev libwayland-dev libxkbcommon-dev libxcursor-dev libxi-dev
```
进入项目文件夹
```bash
cd cw
```
创建build文件夹
```bash
mkdir build
```
编译
```bash
g++ main.cpp ./src/glad.c -std=c++20 -Iincludes -Llib -lglfw3 -lGL -ldl -o build/GuestLiangOpenGLcw
```
产物在cw/build文件夹下

# Windows
## 推荐使用Visual Studio 2022 Community打开sln文件
选择Release x64模式，生成即可   
产物在cw/x64/Release文件夹下

# 介绍

### GLFW update from 3.3 to 3.4
[GLFW3.4 Windows x64 版本](https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip)   

```text
glfw 开发目的是用于替代 glut 的
它是一个轻量级的，开源的，跨平台的 library
支持 OpenGL 及 OpenGL ES ，用来管理窗口，读取输入，处理事件等。
```

### GLM update from 0.9.9.3 to 1.0.1
[GLM 1.0.1 Release](https://github.com/g-truc/glm/releases/download/1.0.1/glm-1.0.1-light.zip)   
release 是1.0.1，但是detail/setup.hpp中的版本号是1.0.0，参考此[issue](https://github.com/g-truc/glm/issues/1302)
```text
OpenGL Mathematics（GLM）是一个广泛使用的几何数学库，
它基于OpenGL Shading Language（GLSL）规范，
专门为C++图形编程提供支持。
```
