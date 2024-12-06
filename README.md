# EBU7405-Coursework
`cw/includes`、`cw/src`、`cw/lib`均来自GitHub Repo: [LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL)

# 仅在windows下使用Visual Studio 2022 Community编译通过，其他平台因为无设备，无法测试
# 推荐使用Visual Studio 2022 Community打开sln文件

### GLFW up 3.3 to 3.4
[GitHub Release](https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip)   
```text
glfw 开发目的是用于替代 glut 的
它是一个轻量级的，开源的，跨平台的 library
支持 OpenGL 及 OpenGL ES ，用来管理窗口，读取输入，处理事件等。
```

### GLM up 0.9.9.3 to 1.0.1
[GitHub Release](https://github.com/g-truc/glm/releases/download/1.0.1/glm-1.0.1-light.zip)   
release 是1.0.1，但是detail/setup.hpp中的版本号是1.0.0，参考此[issue](https://github.com/g-truc/glm/issues/1302)
```text
OpenGL Mathematics（GLM）是一个广泛使用的几何数学库，
它基于OpenGL Shading Language（GLSL）规范，
专门为C++图形编程提供支持。
```
