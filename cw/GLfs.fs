#version 330 core

in vec3 fragColor;  // 从顶点着色器接收颜色
in vec3 FragPos;    // 从顶点着色器接收片段位置
in vec3 Normal;     // 从顶点着色器接收法线
out vec4 FragColor;  // 最终输出的片段颜色

uniform vec3 lightPos;   // 光源位置
uniform vec3 viewPos;    // 观察者位置

uniform vec3 ambientColor;  // 环境光颜色
uniform vec3 diffuseColor;  // 漫反射光颜色
uniform vec3 specularColor; // 镜面反射光颜色
uniform float shininess;    // 镜面反射的光泽度

void main()
{
    // 环境光计算
    vec3 ambient = 0.1 * ambientColor;

    // 漫反射光计算
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;

    // 镜面反射光计算
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * specularColor;

    // 最终光照颜色
    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(fragColor * lighting, 1.0);
}
