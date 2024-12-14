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
float CameraSpeed;						    // 摄像机移动速度

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
	CameraSpeed = 30.0f;												// 摄像机移动速度

    isDragging = false;                                                 // 记录是否正在拖拽
    isFreeMoveMode = true;                                              // 切换摄像机模式
    firstMouse = true;                                                  // 是否是第一次移动鼠标
    // keys[GLFW_KEY_LAST + 1] = { false };                             // 记录所有按键的状态

    lastX = 0, lastY = 0;                                               // 上一次鼠标位置
    radius = 4.0f;                                                      // 摄像机到固定模式的中心的距离
    camPos = 10.0f * glm::vec3(-2.0f, 1.0f, 2.0f) + 60.0f * Upward;     // 存储摄像机的位置（x, y, z）
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
    float camSpeed = static_cast<float>(CameraSpeed * deltaTime);

    if (isFreeMoveMode) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPos += camSpeed * camFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPos -= camSpeed * camFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camPos -= glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camPos += glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
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
    int Color_Gray1 = 0xa9a798;
	int Color_Orange = 0xd68c4c;
    int Color_Orange1 = 0xc37f4e;
    int Color_Brown = 0x423129;
    int Color_Brown1 = 0x4a372b;
	int Color_Brown2 = 0x998274;
    int Color_Gold = 0xebd160;
    int Color_Gold1 = 0xd3b157;
    int Color_Pink = 0xe5d5c5;
    int Color_Pink1 = 0xffafc8;
    int Color_IndigoBlue = 0x635ca2;
    int Color_BlueGray = 0x71bbc8;

    // line1 done
    glm::vec3 Line1Origin = Origin;
	glm::vec3 nowDrawingPoint = Line1Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
	    DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		for (int i = 0; i < 8; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		}
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 8.0f * Rightward);
    }
    // endline1

    // line2 done
    glm::vec3 Line2Origin = Origin + 2.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line2Origin;
    {
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 8.0f * Rightward);
    }
	// endline2

	// line3 done
	glm::vec3 Line3Origin = Line2Origin + 1.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line3Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
    }
	// endline3

	// line4 done
	glm::vec3 Line4Origin = Line3Origin + 1.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line4Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline4

	// line5 done
	glm::vec3 Line5Origin = Line4Origin + 1.0f * Upward;
	nowDrawingPoint = Line5Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline5

	// line6 done
	glm::vec3 Line6Origin = Line5Origin + 1.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line6Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline6

	// line7 done
	glm::vec3 Line7Origin = Line6Origin + 1.0f * Upward;
	nowDrawingPoint = Line7Origin;
	{
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline7

	// line8 done
	glm::vec3 Line8Origin = Line7Origin + 1.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line8Origin;
	{
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline8

	// line9 done
	glm::vec3 Line9Origin = Line8Origin + 1.0f * Upward;
	nowDrawingPoint = Line9Origin;
	{
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline9

	// line10 done
	glm::vec3 Line10Origin = Line9Origin + 1.0f * Upward;
	nowDrawingPoint = Line10Origin;
	{
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline10

	// line11 done
	glm::vec3 Line11Origin = Line10Origin + 1.0f * Upward;
	nowDrawingPoint = Line11Origin;
	{
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward); // 图里是空的，这里填充一个cha色
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline11

	// line12 done
	glm::vec3 Line12Origin = Line11Origin + 1.0f * Upward;
	nowDrawingPoint = Line12Origin;
	{
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline12

	// line13 done
	glm::vec3 Line13Origin = Line12Origin + 1.0f * Upward + 1.0f * Rightward;
    nowDrawingPoint = Line13Origin;
    {
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline13

	// line14 done
	glm::vec3 Line14Origin = Line13Origin + 1.0f * Upward;
    nowDrawingPoint = Line14Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
		for (int i = 0; i < 7; i++) {
			DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
			DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		}
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline14

	// line15 done
	glm::vec3 Line15Origin = Line14Origin + 1.0f * Upward;
	nowDrawingPoint = Line15Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline15

	// line16 done
	glm::vec3 Line16Origin = Line15Origin + 1.0f * Upward + 1.0f * Rightward;
    nowDrawingPoint = Line16Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline16

	// line17 done
	glm::vec3 Line17Origin = Line16Origin + 1.0f * Upward;
    nowDrawingPoint = Line17Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline17

	// line18 done
	glm::vec3 Line18Origin = Line17Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line18Origin;
    {

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); } // 原图留空，给他补一个
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline18

	// line19 done
	glm::vec3 Line19Origin = Line18Origin + 1.0f * Upward;
    nowDrawingPoint = Line19Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gold),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline19

    // line20 done
    glm::vec3 Line20Origin = Line19Origin + 1.0f * Upward + 1.0f * Rightward;
    nowDrawingPoint = Line20Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
    // endline20

    // line21 done
    glm::vec3 Line21Origin = Line20Origin + 1.0f * Upward + 1.0f * Rightward;
    nowDrawingPoint = Line21Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
    // endline21

    // line22 done
    glm::vec3 Line22Origin = Line21Origin + 1.0f * Upward;
    nowDrawingPoint = Line22Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
    // endline22

	// line23 done
	glm::vec3 Line23Origin = Line22Origin + 1.0f * Upward + 1.0f * Leftward;
    nowDrawingPoint = Line23Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline23

    // line24 done
	glm::vec3 Line24Origin = Line23Origin + 1.0f * Upward + 1.0f * Leftward;
	nowDrawingPoint = Line24Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown2),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown2),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline24

    // line25 done
	glm::vec3 Line25Origin = Line24Origin + 1.0f * Upward + 1.0f * Leftward;
    nowDrawingPoint = Line25Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline25

	// line26 done
	glm::vec3 Line26Origin = Line25Origin + 1.0f * Upward;
	nowDrawingPoint = Line26Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline26

	// line27 done
	glm::vec3 Line27Origin = Line26Origin + 1.0f * Upward + 1.0f * Leftward;
	nowDrawingPoint = Line27Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline27

	// line28 done
	glm::vec3 Line28Origin = Line27Origin + 1.0f * Upward;
	nowDrawingPoint = Line28Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline28

	// line29 done
	glm::vec3 Line29Origin = Line28Origin + 1.0f * Upward;
	nowDrawingPoint = Line29Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline29

	// line30 done
	glm::vec3 Line30Origin = Line29Origin + 1.0f * Upward;
	nowDrawingPoint = Line30Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 8.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline30

	// line31 done
	glm::vec3 Line31Origin = Line30Origin + 1.0f * Upward;
	nowDrawingPoint = Line31Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline31

	// line32 done
	glm::vec3 Line32Origin = Line31Origin + 1.0f * Upward;
	nowDrawingPoint = Line32Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 8.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline32

	// line33 done
	glm::vec3 Line33Origin = Line32Origin + 1.0f * Upward + 1.0F * Rightward;
	nowDrawingPoint = Line33Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 24; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline33

	// line34 done
	glm::vec3 Line34Origin = Line33Origin + 1.0f * Upward;
	nowDrawingPoint = Line34Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 29; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline34

	// line35 done
	glm::vec3 Line35Origin = Line34Origin + 1.0f * Upward;
	nowDrawingPoint = Line35Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 30; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline35

	// line36 done
	glm::vec3 Line36Origin = Line35Origin + 1.0f * Upward + 1.0F * Rightward;
	nowDrawingPoint = Line36Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
    }
	// endline36

	// line37 done
	glm::vec3 Line37Origin = Line36Origin + 1.0f * Upward;
	nowDrawingPoint = Line37Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline37

	// line38 done
	glm::vec3 Line38Origin = Line37Origin + 1.0f * Upward + 1.0F * Rightward;
	nowDrawingPoint = Line38Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 36; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline38

	// line39 done
	glm::vec3 Line39Origin = Line38Origin + 1.0f * Upward;
	nowDrawingPoint = Line39Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 32; i++) { DrawCube(TransHEXtoVec3(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline39

	// line40 done
	glm::vec3 Line40Origin = Line39Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line40Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 32; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline40

	// line41 done
	glm::vec3 Line41Origin = Line40Origin + 1.0f * Upward;
	nowDrawingPoint = Line41Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 20; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline41

	// line42 done
	glm::vec3 Line42Origin = Line41Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line42Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 36; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline42

	// line43 done
	glm::vec3 Line43Origin = Line42Origin + 1.0f * Upward;
	nowDrawingPoint = Line43Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 38; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline43

	// line44 done
	glm::vec3 Line44Origin = Line43Origin + 1.0f * Upward;
	nowDrawingPoint = Line44Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 40; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline44

	// line45 done
	glm::vec3 Line45Origin = Line44Origin + 1.0f * Upward;
	nowDrawingPoint = Line45Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 21; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline45

	// line46 done
	glm::vec3 Line46Origin = Line45Origin + 1.0f * Upward;
	nowDrawingPoint = Line46Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec3(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline46

	// line47 done
	glm::vec3 Line47Origin = Line46Origin + 1.0f * Upward;
	nowDrawingPoint = Line47Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline47

	// line48 done
	glm::vec3 Line48Origin = Line47Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line48Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline48

	// line49 done
	glm::vec3 Line49Origin = Line48Origin + 1.0f * Upward;
	nowDrawingPoint = Line49Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_BlueGray), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline49

	// line50 done
	glm::vec3 Line50Origin = Line49Origin + 1.0f * Upward;
	nowDrawingPoint = Line50Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline50

    // line51 done
	glm::vec3 Line51Origin = Line50Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line51Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline51

	// line52 done
	glm::vec3 Line52Origin = Line51Origin + 1.0f * Upward;
	nowDrawingPoint = Line52Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline52

	// line53 done
	glm::vec3 Line53Origin = Line52Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line53Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline53

	// line54 done
	glm::vec3 Line54Origin = Line53Origin + 1.0f * Upward;
	nowDrawingPoint = Line54Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline54

	// line55 done
	glm::vec3 Line55Origin = Line54Origin + 1.0f * Upward;
	nowDrawingPoint = Line55Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline55

	// line56 done
	glm::vec3 Line56Origin = Line55Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line56Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec3(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_Black),        1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline56

	// line57 done
	glm::vec3 Line57Origin = Line56Origin + 1.0f * Upward;
	nowDrawingPoint = Line57Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec3(Color_Black),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline57

	// line58 done
	glm::vec3 Line58Origin = Line57Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line58Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline58

	// line59 done
	glm::vec3 Line59Origin = Line58Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line59Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline59

	// line60 done
	glm::vec3 Line60Origin = Line59Origin + 1.0f * Upward;
	nowDrawingPoint = Line60Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline60

	// line61 done
	glm::vec3 Line61Origin = Line60Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line61Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline61

	// line62 done
	glm::vec3 Line62Origin = Line61Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line62Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline62

	// line63 done
	glm::vec3 Line63Origin = Line62Origin + 1.0f * Upward;
	nowDrawingPoint = Line63Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline63

	// line64 done
	glm::vec3 Line64Origin = Line63Origin + 1.0f * Upward;
	nowDrawingPoint = Line64Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline64

	// line65 done
	glm::vec3 Line65Origin = Line64Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line65Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline65

	// line66 done
	glm::vec3 Line66Origin = Line65Origin + 1.0f * Upward;
	nowDrawingPoint = Line66Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 20; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline66

	// line67 done
	glm::vec3 Line67Origin = Line66Origin + 1.0f * Upward;
	nowDrawingPoint = Line67Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 21; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline67

	// line68 done
	glm::vec3 Line68Origin = Line67Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line68Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 21; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline68

	// line69 done
	glm::vec3 Line69Origin = Line68Origin + 1.0f * Upward;
	nowDrawingPoint = Line69Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 21; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline69

	// line70 done
    glm::vec3 Line70Origin = Line69Origin + 1.0f * Upward;
	nowDrawingPoint = Line70Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline70

	// line71 done
	glm::vec3 Line71Origin = Line70Origin + 1.0f * Upward;
	nowDrawingPoint = Line71Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline71

	// line72 done
	glm::vec3 Line72Origin = Line71Origin + 1.0f * Upward;
	nowDrawingPoint = Line72Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline72

	// line73 done
	glm::vec3 Line73Origin = Line72Origin + 1.0f * Upward;
	nowDrawingPoint = Line73Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline73

	// line74 done
	glm::vec3 Line74Origin = Line73Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line74Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_White),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline74

	// line75 done
	glm::vec3 Line75Origin = Line74Origin + 1.0f * Upward;
	nowDrawingPoint = Line75Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline75

	// line76 done
	glm::vec3 Line76Origin = Line75Origin + 1.0f * Upward;
	nowDrawingPoint = Line76Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_White),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline76

	// line77 done
	glm::vec3 Line77Origin = Line76Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line77Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_White),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline77

	// line78 done
	glm::vec3 Line78Origin = Line77Origin + 1.0f * Upward;
	nowDrawingPoint = Line78Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline78

	// line79 done
	glm::vec3 Line79Origin = Line78Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line79Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec3(Color_White),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline79

	// line80 done
	glm::vec3 Line80Origin = Line79Origin + 1.0f * Upward;
	nowDrawingPoint = Line80Origin;
    {
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec3(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec3(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec3(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec3(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline80


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
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
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