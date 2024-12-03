#version 330 core

in vec3 fragColor;  // �Ӷ�����ɫ��������ɫ
in vec3 FragPos;    // �Ӷ�����ɫ������Ƭ��λ��
in vec3 Normal;     // �Ӷ�����ɫ�����շ���
out vec4 FragColor;  // ���������Ƭ����ɫ

uniform vec3 lightPos;   // ��Դλ��
uniform vec3 viewPos;    // �۲���λ��

uniform vec3 ambientColor;  // ��������ɫ
uniform vec3 diffuseColor;  // ���������ɫ
uniform vec3 specularColor; // ���淴�����ɫ
uniform float shininess;    // ���淴��Ĺ����

void main()
{
    // ���������
    vec3 ambient = 0.1 * ambientColor;

    // ����������
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;

    // ���淴������
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * specularColor;

    // ���չ�����ɫ
    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(fragColor * lighting, 1.0);
}
