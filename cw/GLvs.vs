#version 330 core

layout (location = 0) in vec3 aPos;  // 顶点位置
layout (location = 1) in vec3 aColor;  // 顶点颜色

out vec3 fragColor;  // 输出到片段着色器的颜色
out vec3 FragPos;    // 传递到片段着色器的片段位置
out vec3 Normal;     // 传递到片段着色器的法线

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    fragColor = aColor;  // 将顶点颜色传递给片段着色器
    FragPos = vec3(model * vec4(aPos, 1.0));  // 计算片段位置
    Normal = mat3(transpose(inverse(model))) * aPos;  // 计算物体的法线
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
