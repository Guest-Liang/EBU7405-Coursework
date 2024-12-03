#version 330 core

layout (location = 0) in vec3 aPos;  // ����λ��
layout (location = 1) in vec3 aColor;  // ������ɫ

out vec3 fragColor;  // �����Ƭ����ɫ������ɫ
out vec3 FragPos;    // ���ݵ�Ƭ����ɫ����Ƭ��λ��
out vec3 Normal;     // ���ݵ�Ƭ����ɫ���ķ���

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    fragColor = aColor;  // ��������ɫ���ݸ�Ƭ����ɫ��
    FragPos = vec3(model * vec4(aPos, 1.0));  // ����Ƭ��λ��
    Normal = mat3(transpose(inverse(model))) * aPos;  // ��������ķ���
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
