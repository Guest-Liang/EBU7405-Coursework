//Reference GitHub repo: https://github.com/JoeyDeVries/LearnOpenGL
#include <glad/glad.h>  // ��Ҫ����OpenGL��չ
#include <GLFW/glfw3.h>  // ʹ��GLFW����д��ڹ���
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader_m.h>
//End of GitHub repo
#include <cmath>
#include <vector>
#include <iostream>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif
constexpr double M_PI = 3.14159265358979323846;

// �Ƕȱ�����������ת�ӽ�
GLfloat theta[] = { 18.0, 36.0, 0.0 };  // X, Y, Z �����ת�Ƕ�
int lastX = 0, lastY = 0;  // ��һ�����λ��
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

bool isDragging = false; // ��¼�Ƿ�������ק
bool isFreeMoveMode = true;  // �л������ģʽ��Ĭ��Ϊ�����ƶ�ģʽ
bool firstMouse = true;  // ��¼�Ƿ��ǵ�һ���ƶ����
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float CusScale = 1.0f;
float radius = 900.0f * CusScale;  // �����ӵ�İ뾶
float CubeSize = 100.0f * CusScale;
float GroundSize = 500.0f * CusScale;
float movementSpeed = 10.0f; // �ƶ��ٶ�
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f); // �洢�������λ�ã�x, y, z��
glm::vec3 camFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)); // �������ǰ����
glm::vec3 camUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)); // ��������Ϸ���

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (isDragging)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (isFreeMoveMode) {
            // �����ƶ�ģʽ������϶��ı�������ĳ���
            theta[1] += xoffset;  // �ı�ˮƽ��ת�Ƕ�
            theta[0] -= yoffset;  // �ı䴹ֱ��ת�Ƕ�
        }
        else {
            // ��תģʽ���ı��ӽ�
            theta[0] += yoffset;
            theta[1] += xoffset;
        }

        // ���Ƹ����ǣ���ֹ��ת
        if (theta[0] > 89.0f) theta[0] = 89.0f;
        if (theta[0] < -89.0f) theta[0] = -89.0f;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camFront = glm::normalize(front);
    }
    else {
        lastX = SCR_WIDTH/2;
		lastY = SCR_HEIGHT/2;
		firstMouse = true;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) // �������
    {
        if (action == GLFW_PRESS) {
            isDragging = true;  // �����������ʼ��ק
        } else if (action == GLFW_RELEASE) {
            isDragging = false;  // �ɿ������ֹͣ��ק
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPos += cameraSpeed * camFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPos -= cameraSpeed * camFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camPos -= glm::normalize(glm::cross(camFront, camUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camPos += glm::normalize(glm::cross(camFront, camUp)) * cameraSpeed;
    

    // ��������������������г����İ�����
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS) {
            const char* keyName = glfwGetKeyName(key, 0);  // ��ȡ��������
            if (keyName) {
                std::cout << keyName << " key pressed " << key << std::endl;
            }
            else {
                std::cout << "Unknown key with code " << key << " pressed" << std::endl;
            }
        }
    }
}


int main() {
    std::cout << glfwGetVersionString() << std::endl;
    glfwSwapInterval(0);  // ���� V-Sync

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // �������汾��
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  // ���ôΰ汾��
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // ʹ�ú���ģʽ

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Scene", nullptr, nullptr);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Load OpenGL functions using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);  // Enable depth testing for 3D rendering

    // ---------------------------������ɫ��
    // build and compile our shader program
    Shader GLShader("GLvs.vs", "GLfs.fs");
    float vertices[] = {
        // ���� (��ɫ)
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // ����
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // ����
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // ����
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // ����
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // ����
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // ����

        // ǰ�� (��ɫ)
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // ����
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // ����
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // ����
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // ����
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // ����
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // ����

        // ���� (��ɫ)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // ����
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // ǰ��
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // ǰ��
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // ǰ��
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // ����
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // ����

        // ���� (��ɫ)
         0.5f, -0.5f, -0.5f,  0.2f, 0.2f, 0.2f,  // ����
         0.5f,  0.5f, -0.5f,  0.2f, 0.2f, 0.2f,  // ǰ��
         0.5f,  0.5f,  0.5f,  0.2f, 0.2f, 0.2f,  // ǰ��
         0.5f,  0.5f,  0.5f,  0.2f, 0.2f, 0.2f,  // ǰ��
         0.5f, -0.5f,  0.5f,  0.2f, 0.2f, 0.2f,  // ����
         0.5f, -0.5f, -0.5f,  0.2f, 0.2f, 0.2f,  // ����

         // ���� (��ɫ)
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // ��ǰ
          0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // ��ǰ
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // �Һ�
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // �Һ�
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // ���
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // ��ǰ

         // ���� (��ɫ)
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // ��ǰ
          0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // ��ǰ
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  // �Һ�
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  // �Һ�
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  // ���
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // ��ǰ
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);  // λ��
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));  // ��ɫ
    glEnableVertexAttribArray(1);
    glBindVertexArray(VAO);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Set the background color to sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLShader.use();


        //glm::vec3 lightPos(1.0f, 1.0f, 1.0f);  // ���ù�Դ��λ��
        //GLShader.setVec3("lightPos", lightPos);  // ���ݹ�Դλ�ø���ɫ��
        GLShader.setVec3("ambientColor", glm::vec3(20.0f, 20.0f, 20.0f));  // ���û�����
        GLShader.setVec3("diffuseColor", glm::vec3(0.8f, 0.8f, 0.8f));  // �����������
        GLShader.setVec3("specularColor", glm::vec3(1.0f, 1.0f, 1.0f));  // ���þ��淴���
        GLShader.setFloat("shininess", 50.0f);  // ���þ��淴��Ĺ����
        // ����ͶӰ����
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        GLShader.setMat4("projection", projection);
        // ������ͼ����
        glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);
        GLShader.setMat4("view", view);
		// ����ģ�;���
        glm::mat4 model = glm::mat4(1.0f);
        GLShader.setMat4("model", model);

        glBindVertexArray(VAO);  // ��VAO
        glDrawArrays(GL_TRIANGLES, 0, 36);  // ����������

        glfwSwapBuffers(window);  // Swap the buffer to display the rendered image
        glfwPollEvents();  // Poll events (e.g., keyboard/mouse)
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}