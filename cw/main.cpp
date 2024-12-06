//Reference GitHub repo: https://github.com/JoeyDeVries/LearnOpenGL
#include <glad/glad.h>  // 需要加载OpenGL扩展
#include <GLFW/glfw3.h>  // 使用GLFW库进行窗口管理
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//End of GitHub repo

#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif
//-------------Configuration----------------
constexpr double M_PI = 3.14159265358979323846;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float fov;                                  // Field of View
float deltaTime;	                        // time between current frame and last frame
float lastFrame;                            // Time of last frame

bool isDragging;                            // 记录是否正在拖拽
bool isFreeMoveMode;                        // 切换摄像机模式
bool firstMouse;                            // 是否是第一次移动鼠标
bool keys[GLFW_KEY_LAST + 1] = { false };   // 记录所有按键的状态

int lastX, lastY;                           // 上一次鼠标位置
float radius;                               // 摄像机到固定模式的中心的距离
glm::vec3 camPos;                           // 存储摄像机的位置（x, y, z）
glm::vec3 camFront;                         // 摄像机的前方向
glm::vec3 camUp;                            // 摄像机的上方向
float freeModePitch;                        // freeModePitch 前方向y分量的反正弦值
float freeModeYaw;                          // freeModeYaw x,z平面的反正切值

float fixedYaw;                             // 固定模式下的水平旋转角度
float fixedPitch;                           // 固定模式下的垂直旋转角度

glm::mat4 fixViewMatrix;
//-------------Configuration----------------

std::string GetCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    struct tm tm_info;

    errno_t err = localtime_s(&tm_info, &now_time_t);
    if (err)
    {
        std::cerr << "Failed to get local time." << std::endl;
        return "[unknown time]";
    }

    std::ostringstream oss;
    oss << "["
        << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << now_ms.count()
        << "]";

    return oss.str();
}

void LogInfo() {
    std::string timeStr = GetCurrentTimeString();
    std::cout << timeStr << std::endl;

    const int width = 20; // 列宽

    std::cout << std::fixed << std::setprecision(3); // 浮点数精度

    std::cout << std::left << std::setw(width) << "Parameter"
        << std::left << std::setw(width) << "Value" << std::endl; // 表头

    std::cout << std::string(width * 2, '-') << std::endl; // 分割线

    // 打印各个参数及其值
    std::cout << std::left << std::setw(width) << "FreeMoveMode"
        << std::left << std::setw(width) << (isFreeMoveMode ? "Enabled" : "Disabled") << std::endl;

    std::cout << std::left << std::setw(width) << "FreeMovePitch"
        << std::left << std::setw(width) << freeModePitch << std::endl;

    std::cout << std::left << std::setw(width) << "FreeMoveYaw"
        << std::left << std::setw(width) << freeModeYaw << std::endl;

    std::cout << std::left << std::setw(width) << "FixedPitch"
        << std::left << std::setw(width) << fixedPitch << std::endl;

    std::cout << std::left << std::setw(width) << "FixedYaw"
        << std::left << std::setw(width) << fixedYaw << std::endl;

    std::cout << std::left << std::setw(width) << "fov"
		<< std::left << std::setw(width) << fov << std::endl;

    // 格式化 camPos
    std::ostringstream camPosStr;
    camPosStr << std::fixed << std::setprecision(3)
        << "(" << camPos.x << ", " << camPos.y << ", " << camPos.z << ")";
    std::cout << std::left << std::setw(width) << "camPos"
        << std::left << std::setw(width) << camPosStr.str() << std::endl;

    // 格式化 camFront
    std::ostringstream camFrontStr;
    camFrontStr << std::fixed << std::setprecision(3)
        << "(" << camFront.x << ", " << camFront.y << ", " << camFront.z << ")";
    std::cout << std::left << std::setw(width) << "camFront"
        << std::left << std::setw(width) << camFrontStr.str() << std::endl;

    // 格式化 camUp
    std::ostringstream camUpStr;
    camUpStr << std::fixed << std::setprecision(3)
        << "(" << camUp.x << ", " << camUp.y << ", " << camUp.z << ")";
    std::cout << std::left << std::setw(width) << "camUp"
        << std::left << std::setw(width) << camUpStr.str() << std::endl;
}

void ResetParams()
{
    fov = 45.0f;
    deltaTime = 0.0f;	                                                // time between current frame and last frame
    lastFrame = 0.0f;                                                   // Time of last frame

    isDragging = false;                                                 // 记录是否正在拖拽
    isFreeMoveMode = true;                                              // 切换摄像机模式
    firstMouse = true;                                                  // 是否是第一次移动鼠标
    // keys[GLFW_KEY_LAST + 1] = { false };                             // 记录所有按键的状态

    lastX = 0, lastY = 0;                                               // 上一次鼠标位置
    radius = 4.0f;                                                      // 摄像机到固定模式的中心的距离
    camPos = glm::vec3(-2.0f, 1.0f, 2.0f);                              // 存储摄像机的位置（x, y, z）
    camFront = glm::normalize(glm::vec3(1.0f, -0.5f, -1.0f));           // 摄像机的前方向
    camUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));                // 摄像机的上方向
    freeModePitch = glm::degrees(std::asin(camFront.y));                // freeModePitch 前方向y分量的反正弦值
    freeModeYaw = glm::degrees(std::atan2(-camFront.x, -camFront.z));   // freeModeYaw x,z平面的反正切值

    fixedYaw = 135.0f;                                                  // 固定模式下的水平旋转角度
    fixedPitch = 18.0f;                                                 // 固定模式下的垂直旋转角度

    // 初始化视图矩阵为固定模式
    fixViewMatrix = glm::lookAt(
        glm::vec3(radius * cos(glm::radians(fixedYaw)) * cos(glm::radians(fixedPitch)),
            radius * sin(glm::radians(fixedPitch)),
            radius * sin(glm::radians(fixedYaw)) * cos(glm::radians(fixedPitch))),
        glm::vec3(0.0f, 0.0f, 0.0f),
        camUp);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (isDragging) {
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

        float sensitivity = 0.1f; // 视角转动灵敏度
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        if (isFreeMoveMode) {
            freeModeYaw += xoffset;
            freeModePitch += yoffset;

            if (freeModePitch > 89.0f) freeModePitch = 89.0f;
            if (freeModePitch < -89.0f) freeModePitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(freeModeYaw)) * cos(glm::radians(freeModePitch));
            front.y = sin(glm::radians(freeModePitch));
            front.z = sin(glm::radians(freeModeYaw)) * cos(glm::radians(freeModePitch));
            camFront = glm::normalize(front);
        }
        else {
            fixedYaw += xoffset;
            fixedPitch += -yoffset;

            if (fixedPitch > 89.0f)
                fixedPitch = 89.0f;
            if (fixedPitch < -89.0f)
                fixedPitch = -89.0f;

            // 基于球坐标来计算摄像机的位置
            float camX = radius * cos(glm::radians(fixedYaw)) * cos(glm::radians(fixedPitch));
            float camY = radius * sin(glm::radians(fixedPitch));
            float camZ = radius * sin(glm::radians(fixedYaw)) * cos(glm::radians(fixedPitch));

            fixViewMatrix = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 0.0f, 0.0f), camUp);
        }
    }
    else {
        firstMouse = true;
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)       // 监听左键
    {
        if (action == GLFW_PRESS) {
            isDragging = true;                  // 按下左键，开始拖拽
        } else if (action == GLFW_RELEASE) {
            isDragging = false;                 // 松开左键，停止拖拽
        }
    }
}

void PrintKeyEvent(int key, const std::string& eventType)
{
    std::string timeStr = GetCurrentTimeString();

    const char* keyName = glfwGetKeyName(key, 0);
    if (keyName) {
        std::cout << timeStr << " " << keyName << " key " << eventType << std::endl;
    }
    else {
        std::cout << timeStr << " Unknown key with code " << key << " " << eventType << std::endl;
    }
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    // 监听 M 键，切换自由移动模式和固定模式
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keys[GLFW_KEY_M]) {
        PrintKeyEvent(GLFW_KEY_M, "pressed");
        keys[GLFW_KEY_M] = true; // 标记 M 键按下
        isFreeMoveMode = !isFreeMoveMode; // 切换模式
        std::string timeStr = GetCurrentTimeString();
        std::cout << timeStr << " FreeMoveMode: " << (isFreeMoveMode ? "Enabled" : "Disabled") << std::endl;
        firstMouse = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && keys[GLFW_KEY_M]) {
        PrintKeyEvent(GLFW_KEY_M, "released");
        keys[GLFW_KEY_M] = false; // 标记 M 键释放
    }

    // 监听 L 键，按一次就LogInfo
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !keys[GLFW_KEY_L]) {
        PrintKeyEvent(GLFW_KEY_L, "pressed");
        keys[GLFW_KEY_L] = true; // 标记 L 键按下
        LogInfo();
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && keys[GLFW_KEY_L]) {
        PrintKeyEvent(GLFW_KEY_L, "released");
        keys[GLFW_KEY_L] = false; // 标记 L 键释放
    }
    
    // 监听 R 键，按一次就复原所有参数
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !keys[GLFW_KEY_R]) {
        PrintKeyEvent(GLFW_KEY_R, "pressed");
        keys[GLFW_KEY_R] = true; // 标记 R 键按下
        ResetParams();
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && keys[GLFW_KEY_R]) {
        PrintKeyEvent(GLFW_KEY_R, "released");
        keys[GLFW_KEY_R] = false; // 标记 L 键释放
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
            PrintKeyEvent(key, "pressed");
        }

        if (glfwGetKey(window, key) == GLFW_RELEASE && keys[key]) {
            keys[key] = false;
            PrintKeyEvent(key, "released");
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
	ResetParams();

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

	glfwSetScrollCallback(window, ScrollCallback);            // 滚轮回调
	glfwSetCursorPosCallback(window, MouseCallback);          // 鼠标移动回调
	glfwSetMouseButtonCallback(window, MouseButtonCallback);  // 鼠标按键回调

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);                        // 启用深度测试，避免遮挡关系 
                                                    // Enable depth testing for 3D rendering

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);                       // 处理键盘输入
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view;
        if (isFreeMoveMode) {
            view = glm::lookAt(camPos, camPos + camFront, camUp);
        } else {
            view = fixViewMatrix;
        }

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);    // Set the background color to sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);                // 设置投影矩阵
        glLoadMatrixf(&projection[0][0]);

        glMatrixMode(GL_MODELVIEW);                 // 设置视图矩阵
        glLoadMatrixf(&view[0][0]);

        // 绘制正方体
        DrawCube();

        // 交换缓冲区，处理事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}