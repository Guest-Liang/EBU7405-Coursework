cmake_minimum_required(VERSION 3.30)

# 设置项目名称和语言
project(GuestLiangOpenGLcw LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加项目源文件
file(GLOB SOURCES "main.cpp")

# 指定可执行文件的输出名称
add_executable(GuestLiangOpenGLcw ${SOURCES})

# 添加头文件路径
include_directories(
    "includes/glad"
    "includes/GLFW"
    "includes/glm"
)

# 添加 GLAD 库
add_library(glad "src/glad.c")

# 链接库
target_link_libraries(GuestLiangOpenGLcw PRIVATE glad glfw OpenGL::GL)
