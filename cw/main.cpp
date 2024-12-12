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

glm::vec3 Upward    = glm::vec3( 0.0f,   1.0f,   0.0f);
glm::vec3 Downward  = glm::vec3( 0.0f,  -1.0f,   0.0f);
glm::vec3 Leftward  = glm::vec3(-1.0f,   0.0f,   0.0f);
glm::vec3 Rightward = glm::vec3( 1.0f,   0.0f,   0.0f);
glm::vec3 Forward   = glm::vec3( 0.0f,   0.0f,  -1.0f);
glm::vec3 Backward  = glm::vec3( 0.0f,   0.0f,   1.0f);
//-------------Configuration----------------

static std::string GetCurrentTimeString()
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

static void LogInfo() {
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

static void ResetParams()
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
    camPos = 10.0f * glm::vec3(-2.0f, 1.0f, 2.0f);                      // 存储摄像机的位置（x, y, z）
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

static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

static void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
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

            glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
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

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
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

static void PrintKeyEvent(int key, const std::string& eventType)
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

static void ProcessInput(GLFWwindow* window)
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
    float cameraSpeed = static_cast<float>(10 * deltaTime);

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

static glm::vec3 TransRGBtoVec3(float red, float green, float blue)
{
	return glm::vec3(red / 255.0f, green / 255.0f, blue / 255.0f);
}

static glm::vec3 TransHEXtoVec3(int hex)
{
	float red = ((hex >> 16) & 0xFF);
	float green = ((hex >> 8) & 0xFF);
	float blue = (hex & 0xFF);
	return TransRGBtoVec3(red, green, blue);
}

static void DrawUnitCube() {
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

static void DrawCube(const glm::vec3& color, float edgeLength, const glm::vec3& center) {
    float halfEdge = edgeLength / 2.0f;

    glColor3f(color.x, color.y, color.z); // 设置颜色

    glBegin(GL_QUADS);

    // 正面
    glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z + halfEdge);
    glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z + halfEdge);
    glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z + halfEdge);
    glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z + halfEdge);

    // 背面
    glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z - halfEdge);
    glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z - halfEdge);
    glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z - halfEdge);
    glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z - halfEdge);

    // 上面
    glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z - halfEdge);
    glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z + halfEdge);
    glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z + halfEdge);
    glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z - halfEdge);

    // 下面
    glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z - halfEdge);
    glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z - halfEdge);
    glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z + halfEdge);
    glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z + halfEdge);

    // 左面
    glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z - halfEdge);
    glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z + halfEdge);
    glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z + halfEdge);
    glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z - halfEdge);

    // 右面
    glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z - halfEdge);
    glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z - halfEdge);
    glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z + halfEdge);
    glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z + halfEdge);

    glEnd();
}

static void DrawFirefly() {
    // https://www.douyin.com/note/7388460523207134483
	glm::vec3 Origin = glm::vec3(0.0f, 0.0f, 0.0f);
	int Color_Firefly = 0xafe0d1;
    int Color_cha = 0xc8cabd; // 9a9a8a
	int Color_Black = 0x000000;
	int Color_White = 0xffffff;
    int Color_Gray = 0xf2f2ef;

    // line1 
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 2.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 3.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 4.0f * Rightward);

	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 9.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 10.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 11.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 12.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 13.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 14.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 17.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 18.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 19.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 20.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 21.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 22.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 23.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, Origin + 24.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 25.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, Origin + 26.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, Origin + 27.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 28.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 29.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 30.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 31.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 32.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 33.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 34.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 35.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 36.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 37.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 38.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 39.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 40.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 41.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 42.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 43.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 44.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 45.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 46.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 47.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 48.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 49.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 50.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 51.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 52.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 53.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 54.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 55.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 56.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 57.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 58.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 59.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 60.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 61.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 62.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 63.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 64.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 65.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 66.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 67.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 68.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 69.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 70.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 71.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Origin + 72.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 73.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 74.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 75.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 76.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 77.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 78.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 79.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 80.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 81.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 82.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 83.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 84.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 85.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 86.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 87.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Origin + 88.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Origin + 89.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 90.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Origin + 98.0f * Rightward);
    }
    // endline1

    glm::vec3 Line2Origin = Origin + 2.0f * Leftward + 1.0f * Upward;
    // line2
    {
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 2.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 3.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 4.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 5.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 10.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 11.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 12.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 13.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 14.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 15.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 16.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 19.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 20.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 21.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 22.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 23.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 24.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 25.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 26.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, Line2Origin + 27.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, Line2Origin + 28.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, Line2Origin + 29.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 30.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 31.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 32.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 33.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 34.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 35.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 36.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 37.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 38.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 39.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 40.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 41.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 42.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 43.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 44.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 45.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 46.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 47.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 48.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 49.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 50.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 51.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 52.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 53.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 54.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 55.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 56.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 57.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 58.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 59.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 60.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 61.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 62.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 63.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 64.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 65.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 66.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 67.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 68.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 69.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 70.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 71.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 72.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, Line2Origin + 73.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 74.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 75.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 76.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 77.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 78.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 79.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 80.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 81.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 82.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 83.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 84.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 85.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 86.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 87.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 88.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 89.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, Line2Origin + 90.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, Line2Origin + 91.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 92.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, Line2Origin + 100.0f * Rightward);



    }
	// endline2

	glm::vec3 Line3Origin = Line2Origin + 1.0f * Leftward + 1.0f * Upward;
	// line3
    {

    }
	// endline3

} 

static void DrawAll() {
	//DrawUnitCube();
	DrawFirefly();
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

		DrawAll(); // 绘制

        // 交换缓冲区，处理事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}