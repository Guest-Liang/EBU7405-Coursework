//Reference GitHub repo: https://github.com/JoeyDeVries/LearnOpenGL
#include <glad/glad.h>  // ��Ҫ����OpenGL��չ
#include <GLFW/glfw3.h>  // ʹ��GLFW����д��ڹ���
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <learnopengl/shader_m.h>
//End of GitHub repo

#include <cmath>
#include <iostream>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif
constexpr double M_PI = 3.14159265358979323846;

const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 720;

float angle = 0.0f;
float fov = 45.0f;
float theta[] = { 18.0, 36.0, 0.0 };  // X, Y, Z �����ת�Ƕ�
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool isDragging = false; // ��¼�Ƿ�������ק
bool isFreeMoveMode = true;  // �л������ģʽ
bool firstMouse = true;  // ��¼�Ƿ��ǵ�һ���ƶ����
bool keys[GLFW_KEY_LAST + 1] = { false }; // ��¼���а�����״̬

int lastX = 0, lastY = 0;  // ��һ�����λ��

float radius = 3.0f;  // ��������̶��ӽ�ģʽ�����ĵľ���
glm::vec3 camPos = glm::vec3(-2.0f, 1.0f, 2.0f); // �洢�������λ�ã�x, y, z��
glm::vec3 camFront = glm::normalize(glm::vec3(1.0f, -0.5f, -1.0f)); // �������ǰ����
glm::vec3 camUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)); // ��������Ϸ���
float pitch = glm::degrees(std::asin(camFront.y)); // pitch��ǰ����y�����ķ�����ֵ
float yaw = glm::degrees(std::atan2(-camFront.x, -camFront.z)); // yaw��x,zƽ��ķ�����ֵ

glm::mat4 fixViewMatrix;  // �̶��ӽ�ģʽ�µ���ͼ����


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (isDragging) {
        if (isFreeMoveMode) {
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

            // �����ƶ�ģʽ������϶��ı�������ĳ���
            theta[1] += xoffset;  // �ı�ˮƽ��ת�Ƕ�
            theta[0] -= yoffset;  // �ı䴹ֱ��ת�Ƕ�

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
            // ��תģʽ�������������������������λ��
            float camX = camPos.x + radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            float camY = camPos.y + radius * sin(glm::radians(pitch));
            float camZ = camPos.z + radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

            fixViewMatrix = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 0.0f, 0.0f), camUp);
        }
    }
    else {
        lastX = SCR_WIDTH / 2;
        lastY = SCR_HEIGHT / 2;
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

void printKeyEvent(int key, const std::string& eventType)
{
    const char* keyName = glfwGetKeyName(key, 0);
    if (keyName) {
        std::cout << keyName << " key " << eventType << std::endl;
    }
    else {
        std::cout << "Unknown key with code " << key << " " << eventType << std::endl;
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    // ���� M �����л������ƶ�ģʽ�͹̶��ӽ�ģʽ
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keys[GLFW_KEY_M]) {
        printKeyEvent(GLFW_KEY_M, "pressed");
        keys[GLFW_KEY_M] = true; // ��� M ������
        isFreeMoveMode = !isFreeMoveMode; // �л�ģʽ
        std::cout << "FreeMoveMode: " << (isFreeMoveMode ? "Enabled" : "Disabled") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && keys[GLFW_KEY_M]) {
        printKeyEvent(GLFW_KEY_M, "released");
        keys[GLFW_KEY_M] = false; // ��� M ���ͷ�
    }
    
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);

    if (isFreeMoveMode) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPos += cameraSpeed * camFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPos -= cameraSpeed * camFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camPos -= glm::normalize(glm::cross(camFront, camUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camPos += glm::normalize(glm::cross(camFront, camUp)) * cameraSpeed;
    }

    // ��������������������г����İ�����
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS && !keys[key]) {
            keys[key] = true;
            printKeyEvent(key, "pressed");
        }

        if (glfwGetKey(window, key) == GLFW_RELEASE && keys[key]) {
            keys[key] = false;
            printKeyEvent(key, "released");
        }
    }
}


void DrawCube() {
    glBegin(GL_QUADS);

    // ���� (��ɫ)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // ���� (��ɫ)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // ���� (��ɫ)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // ���� (��ɫ)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // ���� (��ɫ)
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // ���� (��ɫ)
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    glEnd();
}

int main() {
    std::cout << glfwGetVersionString() << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Scene", nullptr, nullptr);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

	glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  // Enable depth testing for 3D rendering


    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view;
        if (isFreeMoveMode) {
            view = glm::lookAt(camPos, camPos + camFront, camUp);
        } else {
            view = fixViewMatrix;
        }

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Set the background color to sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION); // ����ͶӰ����
        glLoadMatrixf(&projection[0][0]);

        glMatrixMode(GL_MODELVIEW); // ������ͼ����
        glLoadMatrixf(&view[0][0]);

        // ����������
        DrawCube();

        // �����������������¼�
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}