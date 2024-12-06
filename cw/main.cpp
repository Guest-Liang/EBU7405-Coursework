//Reference GitHub repo: https://github.com/JoeyDeVries/LearnOpenGL
#include <glad/glad.h>  // 需要加载OpenGL扩展
#include <GLFW/glfw3.h>  // 使用GLFW库进行窗口管理
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
float theta[] = { 18.0, 36.0, 0.0 };  // X, Y, Z 轴的旋转角度
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool isDragging = false; // 记录是否正在拖拽
bool isFreeMoveMode = true;  // 切换摄像机模式
bool firstMouse = true;  // 记录是否是第一次移动鼠标
bool keys[GLFW_KEY_LAST + 1] = { false }; // 记录所有按键的状态

int lastX = 0, lastY = 0;  // 上一次鼠标位置

float radius = 3.0f;  // 摄像机到固定视角模式的中心的距离
glm::vec3 camPos = glm::vec3(-2.0f, 1.0f, 2.0f); // 存储摄像机的位置（x, y, z）
glm::vec3 camFront = glm::normalize(glm::vec3(1.0f, -0.5f, -1.0f)); // 摄像机的前方向
glm::vec3 camUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)); // 摄像机的上方向
float pitch = glm::degrees(std::asin(camFront.y)); // pitch是前方向y分量的反正弦值
float yaw = glm::degrees(std::atan2(-camFront.x, -camFront.z)); // yaw是x,z平面的反正切值

glm::mat4 fixViewMatrix;  // 固定视角模式下的视图矩阵


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

            // 自由移动模式：鼠标拖动改变摄像机的朝向
            theta[1] += xoffset;  // 改变水平旋转角度
            theta[0] -= yoffset;  // 改变垂直旋转角度

            // 限制俯仰角，防止翻转
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
            // 旋转模式：基于球坐标来计算摄像机的位置
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
    if (button == GLFW_MOUSE_BUTTON_LEFT) // 监听左键
    {
        if (action == GLFW_PRESS) {
            isDragging = true;  // 按下左键，开始拖拽
        } else if (action == GLFW_RELEASE) {
            isDragging = false;  // 松开左键，停止拖拽
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

    // 监听 M 键，切换自由移动模式和固定视角模式
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keys[GLFW_KEY_M]) {
        printKeyEvent(GLFW_KEY_M, "pressed");
        keys[GLFW_KEY_M] = true; // 标记 M 键按下
        isFreeMoveMode = !isFreeMoveMode; // 切换模式
        std::cout << "FreeMoveMode: " << (isFreeMoveMode ? "Enabled" : "Disabled") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && keys[GLFW_KEY_M]) {
        printKeyEvent(GLFW_KEY_M, "released");
        keys[GLFW_KEY_M] = false; // 标记 M 键释放
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

    // 检查其他按键（遍历所有常见的按键）
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

    // 正面 (蓝色)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // 背面 (红色)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // 上面 (绿色)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // 下面 (黄色)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // 左面 (紫色)
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // 右面 (青色)
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

		glMatrixMode(GL_PROJECTION); // 设置投影矩阵
        glLoadMatrixf(&projection[0][0]);

        glMatrixMode(GL_MODELVIEW); // 设置视图矩阵
        glLoadMatrixf(&view[0][0]);

        // 绘制正方体
        DrawCube();

        // 交换缓冲区并处理事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}