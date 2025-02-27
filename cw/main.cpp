//Reference GitHub repo: https://github.com/JoeyDeVries/LearnOpenGL
#include <glad/glad.h>  // 需要加载OpenGL扩展
#include <GLFW/glfw3.h>  // 使用GLFW库进行窗口管理
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//End of GitHub repo

#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>
#include <thread>

// Texture header
#include "Texture.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif
//-------------Configuration----------------
int mathPI = 3.14159265358979323846;

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
bool isFirstRun = true;                     // 是否第一次运行

int lastX, lastY;                           // 上一次鼠标位置
float camRadius;                            // 摄像机到固定模式的中心的距离
glm::vec3 camPos;                           // 存储摄像机的位置（x, y, z）
glm::vec3 camFront;                         // 摄像机的前方向
glm::vec3 camUp;                            // 摄像机的上方向
float freeModePitch;                        // freeModePitch 前方向y分量的反正弦值
float freeModeYaw;                          // freeModeYaw x,z平面的反正切值

float fixedYaw;                             // 固定模式下的水平旋转角度
float fixedPitch;                           // 固定模式下的垂直旋转角度

glm::mat4 fixViewMatrix;

glm::vec3 Origin    = glm::vec3( 0.0f,   0.0f,   0.0f);
glm::vec3 Upward    = glm::vec3( 0.0f,   1.0f,   0.0f);
glm::vec3 Downward  = glm::vec3( 0.0f,  -1.0f,   0.0f);
glm::vec3 Leftward  = glm::vec3(-1.0f,   0.0f,   0.0f);
glm::vec3 Rightward = glm::vec3( 1.0f,   0.0f,   0.0f);
glm::vec3 Forward   = glm::vec3( 0.0f,   0.0f,  -1.0f);
glm::vec3 Backward  = glm::vec3( 0.0f,   0.0f,   1.0f);
glm::vec3 PlaneOrigin = Origin + glm::vec3(50.0f, -0.5f, 149.5f);

int Color_Firefly       = 0xafe0d1ff;
int Color_cha           = 0xc8cabdff; // 9a9a8aff
int Color_Black         = 0x000000ff;
int Color_White         = 0xffffffff;
int Color_Gray          = 0xf2f2efff;
int Color_Gray1         = 0xa9a798ff;
int Color_Gray_alpha    = 0xa9a798;
int Color_Gray2         = 0x59544eff;
int Color_Gray3         = 0xccccccff;
int Color_Gray4         = 0x4f4d4eff;
int Color_Green         = 0x7e987fff;
int Color_Orange        = 0xd68c4cff;
int Color_Orange1       = 0xc37f4eff;
int Color_Brown         = 0x423129ff;
int Color_Brown1        = 0x4a372bff;
int Color_Brown2        = 0x998274ff;
int Color_Gold          = 0xebd160ff;
int Color_Gold1         = 0xd3b157ff;
int Color_Pink          = 0xfff1deff; //e5d5c5ff
int Color_Pink1         = 0xffafc8ff;
int Color_IndigoBlue    = 0x635ca2ff;
int Color_BlueGray      = 0x71bbc8ff;
int Color_Ground        = 0x1f3341ff;
int Color_Roof          = 0x66554bff;
int Color_PinkRed       = 0xcd7971ff;

glm::vec3 GuardrailPositionArray[8] = {
    glm::vec3(10.0f, 0.0f, 70.0f),
    glm::vec3(-10.0f, 0.0f, 70.0f),
    glm::vec3(-20.0f, 0.0f, 80.0f),
    glm::vec3(-20.0f, 0.0f, 100.0f),
    glm::vec3(-10.0f, 0.0f, 110.0f),
    glm::vec3(10.0f, 0.0f, 110.0f),
    glm::vec3(20.0f, 0.0f, 100.0f),
    glm::vec3(20.0f, 0.0f, 80.0f),
};

glm::vec3 GuardrailRotationArray[8] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 180.0f, 0.0f),
    glm::vec3(0.0f, 90.0f, 0.0f),
    glm::vec3(0.0f, 270.0f, 0.0f),
    glm::vec3(0.0f, 180.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 270.0f, 0.0f),
    glm::vec3(0.0f, 90.0f, 0.0f),
};

glm::vec3 GuardrailChangeArray[8] = {
    glm::vec3(0.0f, 90.0f, 0.0f),
    glm::vec3(0.0f, -90.0f, 0.0f),
    glm::vec3(0.0f, 90.0f, 0.0f),
    glm::vec3(0.0f, -90.0f, 0.0f),
    glm::vec3(0.0f, 90.0f, 0.0f),
    glm::vec3(0.0f, -90.0f, 0.0f),
    glm::vec3(0.0f, 90.0f, 0.0f),
    glm::vec3(0.0f, -90.0f, 0.0f),
};

int CurrentADFrame = 0;

struct AnimatedObject {
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f }; // 角度制，已在旋转中处理为弧度制
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
    glm::vec3 direction = Leftward;
    float speed = 1.0f; // 暂定每秒一个单位
    float alpha = 1.0f;
    float RotateSpeed = 1.0f;

    AnimatedObject() = default;
};

AnimatedObject obj, GuardrailProps[8], Vehicle_S, HHand, MHand, SHand;

struct GuardrailState {
    bool isAnimating = false;       // 是否正在动画
    int currentStep = 0;            // 当前步骤
    float rotationTimer = 0.0f;     // 动画计时器
    bool RotateFlag = true;         // 栏杆当前旋转标记

	GuardrailState() = default;
};
GuardrailState guardrailState;

//-------------Configuration----------------
static std::string GetCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    struct tm tm_info;

    // Windows 使用 localtime_s，Linux/Unix 使用 localtime_r
#if defined(_MSC_VER) || defined(__MINGW32__) // Windows 平台
    if (localtime_s(&tm_info, &now_time_t) != 0)
    {
        std::cerr << "Failed to get local time." << std::endl;
        return "[unknown time]";
    }
#else // Linux/Unix 平台
    if (localtime_r(&now_time_t, &tm_info) == nullptr)
    {
        std::cerr << "Failed to get local time." << std::endl;
        return "[unknown time]";
    }
#endif

    // 格式化输出时间
    std::ostringstream oss;
    oss << "[" << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << now_ms.count()
        << "]";

    return oss.str();
}

static void GetClockHandAngles(float& hourAngle, float& minuteAngle, float& secondAngle) {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    struct tm tm_info;

#if defined(_MSC_VER) || defined(__MINGW32__) // Windows 平台
    if (localtime_s(&tm_info, &now_time_t) != 0) {
        std::cerr << "Failed to get local time." << std::endl;
        hourAngle = minuteAngle = secondAngle = 0.0f;
        return;
    }
#else // Linux/Unix 平台
    if (localtime_r(&now_time_t, &tm_info) == nullptr) {
        std::cerr << "Failed to get local time." << std::endl;
        hourAngle = minuteAngle = secondAngle = 0.0f;
        return;
    }
#endif

    // 获取当前时间
    int hours = tm_info.tm_hour % 12;  // 12小时制
    int minutes = tm_info.tm_min;
    int seconds = tm_info.tm_sec;

    // 计算角度
    hourAngle = (hours + minutes / 60.0f) * 30.0f;       // 时针：每小时30度，每分钟0.5度
    minuteAngle = (minutes + seconds / 60.0f) * 6.0f;    // 分针：每分钟6度，每秒0.1度
    secondAngle = seconds * 6.0f;                        // 秒针：每秒6度
}

static GLuint LoadTexture(const char* filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID); // 生成纹理对象
    glBindTexture(GL_TEXTURE_2D, textureID); // 绑定纹理对象

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0); // 加载图片
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, nrChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data); // 将图片上传为纹理数据
        glGenerateMipmap(GL_TEXTURE_2D); // 自动生成所有需要的多级渐远纹理

        // 设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // 设置纹理在S方向上的重复方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // 设置纹理在T方向上的重复方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 缩小纹理时的过滤模式，使用线性插值和mipmap插值
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大纹理时的过滤模式，线性插值

        std::cout << GetCurrentTimeString() << " Texture loaded: " << filepath << " (" << width << "x" << height << ")" << std::endl;
    }
    else {
        std::cerr << GetCurrentTimeString() << " Failed to load texture: " << filepath << std::endl;
    }
    stbi_image_free(data); // 释放图片数据

    return textureID;
}

static GLuint LoadTextureFromMemory(const unsigned char* data, unsigned int size) {
    int width, height, channels;

    // 使用 stbi_load_from_memory 加载图像数据
    unsigned char* imageData = stbi_load_from_memory(data, size, &width, &height, &channels, 4);
    if (!imageData) {
        std::cerr << GetCurrentTimeString() << " Failed to load texture from memory: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 上传图像数据到 OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); // 将图片上传为纹理数据
    glGenerateMipmap(GL_TEXTURE_2D); // 自动生成所有需要的多级渐远纹理

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // 设置纹理在S方向上的重复方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // 设置纹理在T方向上的重复方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 缩小纹理时的过滤模式，使用线性插值和mipmap插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大纹理时的过滤模式，线性插值

    std::cout << GetCurrentTimeString() << " Texture loaded. " << " (" << width << "x" << height << "x" << channels << ")" << std::endl;

    stbi_image_free(imageData); // 释放加载的图像数据

    return textureID;
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

static void PrintHelp() {
    std::cout << "Use W, A, S, D, (Space and C) to move the camera" << std::endl;
    std::cout << "Use mouse to rotate the camera" << std::endl;
    std::cout << "Use mouse wheel to zoom in/out" << std::endl;
    std::cout << "Press R to reset the camera params" << std::endl;
    std::cout << "Press M to change to rotate mode" << std::endl;
    std::cout << "Press L to print log info" << std::endl;
    std::cout << "Press H to print this help message" << std::endl;
    std::cout << "Press Esc to exit the program" << std::endl;
}

static void PrintSystemInfo() {
    std::cout << GetCurrentTimeString() << " Program Running..." << std::endl;
    std::cout << GetCurrentTimeString() << " GLFW Version: " << glfwGetVersionString() << std::endl;
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
	isFirstRun = true;                                                  // 是否是第一次运行

    lastX = 0, lastY = 0;                                               // 上一次鼠标位置
    camRadius = 200.0f;                                                 // 摄像机到固定模式的中心的距离
    //camPos = 10.0f * glm::vec3(-2.0f, 1.0f, 2.0f) + 80.0f * Upward;     // 存储摄像机的位置（x, y, z）
    camPos = glm::vec3(-100.436f, 109.194f, 257.340f);
    camFront = glm::normalize(glm::vec3(1.0f, -0.5f, -1.0f));           // 摄像机的前方向
    camUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));                // 摄像机的上方向
    freeModePitch = glm::degrees(std::asin(camFront.y));                // freeModePitch 前方向y分量的反正弦值
    freeModeYaw = glm::degrees(std::atan2(-camFront.x, -camFront.z));   // freeModeYaw x,z平面的反正切值

    fixedYaw = 135.0f;                                                  // 固定模式下的水平旋转角度
    fixedPitch = 18.0f;                                                 // 固定模式下的垂直旋转角度

    // 初始化视图矩阵为固定模式
    fixViewMatrix = glm::lookAt(
        glm::vec3(camRadius * cos(glm::radians(fixedYaw)) * cos(glm::radians(fixedPitch)),
                  camRadius * sin(glm::radians(fixedPitch)),
                  camRadius * sin(glm::radians(fixedYaw)) * cos(glm::radians(fixedPitch))),
        PlaneOrigin,
        camUp);



    // 其他变量初始化
	guardrailState = GuardrailState();

    obj = AnimatedObject();
    Vehicle_S = AnimatedObject();
    HHand = AnimatedObject();
    MHand = AnimatedObject();
    SHand = AnimatedObject();
    for (int i = 0; i < 8; ++i) {
        GuardrailProps[i] = AnimatedObject();
        GuardrailProps[i].position = GuardrailPositionArray[i];
        GuardrailProps[i].rotation = GuardrailRotationArray[i];
    }
    SHand.RotateSpeed = 6.0f;
    MHand.RotateSpeed = 0.1f;
    HHand.RotateSpeed = 0.00833333f;

    GetClockHandAngles(HHand.rotation.x, MHand.rotation.x, SHand.rotation.x);
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
            float camX = camRadius * cos(glm::radians(fixedYaw)) * cos(glm::radians(fixedPitch));
            float camY = camRadius * sin(glm::radians(fixedPitch));
            float camZ = camRadius * sin(glm::radians(fixedYaw)) * cos(glm::radians(fixedPitch));

            fixViewMatrix = glm::lookAt(glm::vec3(camX, camY, camZ), PlaneOrigin, camUp);
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

    int scancode = glfwGetKeyScancode(key);

    const char* keyName = glfwGetKeyName(key, scancode);
    if (keyName) {
        std::cout << timeStr << " " << keyName << " key (scancode: " << scancode << ") " << eventType << std::endl;
    }
    else {
        std::cout << timeStr << " Unknown key with code " << key << " (scancode: " << scancode << ") " << eventType << std::endl;
    }
}

static void ProcessInput(GLFWwindow* window, float deltaTime)
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

    // 监听 L 键，输出LogInfo
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !keys[GLFW_KEY_L]) {
        PrintKeyEvent(GLFW_KEY_L, "pressed");
        keys[GLFW_KEY_L] = true; // 标记 L 键按下
        LogInfo();
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && keys[GLFW_KEY_L]) {
        PrintKeyEvent(GLFW_KEY_L, "released");
        keys[GLFW_KEY_L] = false; // 标记 L 键释放
    }
    
    // 监听 R 键，复原所有参数
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !keys[GLFW_KEY_R]) {
        PrintKeyEvent(GLFW_KEY_R, "pressed");
        keys[GLFW_KEY_R] = true; // 标记 R 键按下
        ResetParams();
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && keys[GLFW_KEY_R]) {
        PrintKeyEvent(GLFW_KEY_R, "released");
        keys[GLFW_KEY_R] = false; // 标记 L 键释放
    }

	// 监听 H 键盘，打印帮助信息
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !keys[GLFW_KEY_H]) {
        PrintKeyEvent(GLFW_KEY_H, "pressed");
        keys[GLFW_KEY_H] = true; // 标记 H 键按下
        PrintHelp();
    }
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE && keys[GLFW_KEY_H]) {
		PrintKeyEvent(GLFW_KEY_H, "released");
		keys[GLFW_KEY_H] = false; // 标记 H 键释放
	}

    float camSpeed = static_cast<float>(CameraSpeed * deltaTime);

    if (isFreeMoveMode) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPos += camSpeed * camFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPos -= camSpeed * camFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camPos -= glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camPos += glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camPos += camSpeed * camUp;
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) camPos -= camSpeed * camUp;
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

static glm::vec4 TransHEXtoVec4WithSeparateAlpha(int hex, float alpha = 1.0f) {
    float red = ((hex >> 24) & 0xFF);
    float green = ((hex >> 16) & 0xFF);
    float blue = ((hex >> 8) & 0xFF);
    return glm::vec4(red / 255.0f, green / 255.0f, blue / 255.0f, alpha);
}

static glm::vec4 TransHEXtoVec4WithAlpha(int hex) {
    float red = ((hex >> 24) & 0xFF);
    float green = ((hex >> 16) & 0xFF);
    float blue = ((hex >> 8) & 0xFF);
    float alpha = (hex & 0xFF) / 255.0f;
    return glm::vec4(red / 255.0f, green / 255.0f, blue / 255.0f, alpha);
}



GLuint Texture_Fuxuan, Texture_LuminaSquare, Texture_QUALITYTEA, 
Texture_141, Texture_LSS, Texture_JC, 
Texture_CWGitHubLink, Texture_Banner, Texture_Clock,
Texture_AD, AD[45];
static void InitTextures() {
    Texture_Fuxuan = LoadTextureFromMemory(fuxuan_jpg, fuxuan_jpg_len);
    Texture_LuminaSquare = LoadTextureFromMemory(Texture_LuminaSquare_png, Texture_LuminaSquare_png_len);
    Texture_QUALITYTEA = LoadTextureFromMemory(Texture_QUALITYTEA_png, Texture_QUALITYTEA_png_len);
    Texture_141 = LoadTextureFromMemory(Texture_141_png, Texture_141_png_len);
    Texture_LSS = LoadTextureFromMemory(Texture_LSS_png, Texture_LSS_png_len);
    Texture_JC = LoadTextureFromMemory(Texture_JC_png, Texture_JC_png_len);
    Texture_CWGitHubLink = LoadTextureFromMemory(Texture_CWGitHubLink_png, Texture_CWGitHubLink_png_len);
    Texture_Banner = LoadTextureFromMemory(Texture_Banner_png, Texture_Banner_png_len);
    Texture_Clock = LoadTextureFromMemory(Texture_Clock_png, Texture_Clock_png_len);
    for (int i = 0; i < 45; i++) {
        AD[i] = LoadTextureFromMemory(BV1EZ421u7NG_data[i], BV1EZ421u7NG_data_len[i]);
        if (AD[i] == 0) {
            std::cerr << GetCurrentTimeString() << " Failed to load texture from memory: AD[" << i << "]" << std::endl;
        }
    }
}

static void DrawTexturedCube(const glm::vec3& center, float edgeLength, GLuint textureID) {
    float halfEdge = edgeLength / 2.0f;

    // 启用纹理映射
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);

    // 正面 (Z+)
    glTexCoord2f(1.0f, 1.0f); glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z + halfEdge);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z + halfEdge);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z + halfEdge);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z + halfEdge);

    // 背面 (Z-)
    glTexCoord2f(1.0f, 1.0f); glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z - halfEdge);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z - halfEdge);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z - halfEdge);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z - halfEdge);

    // 左面 (X-)
    glTexCoord2f(1.0f, 1.0f); glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z - halfEdge);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z + halfEdge);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z + halfEdge);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z - halfEdge);

    // 右面 (X+)
    glTexCoord2f(1.0f, 1.0f); glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z - halfEdge);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z + halfEdge);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z + halfEdge);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z - halfEdge);

    // 上面 (Y+)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z - halfEdge);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z - halfEdge);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(center.x + halfEdge, center.y + halfEdge, center.z + halfEdge);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(center.x - halfEdge, center.y + halfEdge, center.z + halfEdge);

    // 下面 (Y-)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z - halfEdge);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z - halfEdge);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(center.x + halfEdge, center.y - halfEdge, center.z + halfEdge);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(center.x - halfEdge, center.y - halfEdge, center.z + halfEdge);

    glEnd();

    glDisable(GL_TEXTURE_2D); // 关闭纹理映射
}

static void DrawTexturedPlane(const glm::vec3& center, float width, float height, GLuint textureID, const glm::vec3& normal, bool flipTexture = false, bool rotateTexture90 = false) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    // 归一化法向量
    glm::vec3 n = glm::normalize(normal);

    // 找到一个与法向量不平行的向量，计算局部坐标系
    glm::vec3 up = (fabs(n.x) < 0.99f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 tangent = glm::normalize(glm::cross(up, n));
    glm::vec3 bitangent = glm::cross(n, tangent);

    glEnable(GL_TEXTURE_2D); // 启用纹理映射
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);

    // 计算平面顶点
    glm::vec3 v0 = center - halfWidth * tangent - halfHeight * bitangent; // 左下
    glm::vec3 v1 = center + halfWidth * tangent - halfHeight * bitangent; // 右下
    glm::vec3 v2 = center + halfWidth * tangent + halfHeight * bitangent; // 右上
    glm::vec3 v3 = center - halfWidth * tangent + halfHeight * bitangent; // 左上

    if (rotateTexture90) {
        if (flipTexture) {
            glTexCoord2f(1.0f, 0.0f); glVertex3f(v0.x, v0.y, v0.z);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(v1.x, v1.y, v1.z);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(v2.x, v2.y, v2.z);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(v3.x, v3.y, v3.z);
        }
        else {
            glTexCoord2f(0.0f, 1.0f); glVertex3f(v0.x, v0.y, v0.z);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(v1.x, v1.y, v1.z);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(v2.x, v2.y, v2.z);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(v3.x, v3.y, v3.z);
        }
    }
    else {
        if (flipTexture) {
            glTexCoord2f(0.0f, 0.0f); glVertex3f(v0.x, v0.y, v0.z);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(v1.x, v1.y, v1.z);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(v2.x, v2.y, v2.z);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(v3.x, v3.y, v3.z);
        }
        else {
            glTexCoord2f(1.0f, 1.0f); glVertex3f(v0.x, v0.y, v0.z);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(v1.x, v1.y, v1.z);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(v2.x, v2.y, v2.z);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(v3.x, v3.y, v3.z);
        }
    }

    glEnd();

    glDisable(GL_TEXTURE_2D); // 禁用纹理映射
}

static void DrawTextureCircle(GLuint textureID, const glm::vec3& center, float radius, const glm::vec3& normal,
    float startAngle, float endAngle, int slices, bool flipTexture = false, float rotateTexture = 0.0f) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 计算法向量的正交基
    glm::vec3 n = glm::normalize(normal);
    glm::vec3 up = (fabs(n.x) < 0.999f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 tangent = glm::normalize(glm::cross(up, n));
    glm::vec3 bitangent = glm::cross(n, tangent);

    // 转换角度为弧度
    float startRad = glm::radians(startAngle);
    float endRad = glm::radians(endAngle);
    float deltaAngle = (endRad - startRad) / slices;

    // 纹理旋转角度（弧度）
    float textureRotationRad = glm::radians(rotateTexture);

    glBegin(GL_TRIANGLE_FAN);

    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(center.x, center.y, center.z);

    for (int i = 0; i <= slices; ++i) {
        float currentAngle = startRad + i * deltaAngle;

        glm::vec3 point = center + radius * (cos(currentAngle) * tangent + sin(currentAngle) * bitangent);

        float u = cos(currentAngle);
        float v = sin(currentAngle);

        if (flipTexture) {
            u = -u;
            v = -v;
        }

        float rotatedU = 0.5f + 0.5f * (u * cos(textureRotationRad) - v * sin(textureRotationRad));
        float rotatedV = 0.5f + 0.5f * (u * sin(textureRotationRad) + v * cos(textureRotationRad));

        glTexCoord2f(rotatedU, rotatedV);
        glVertex3f(point.x, point.y, point.z);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
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

static void DrawCube(const glm::vec4& color, float edgeLength, const glm::vec3& center) {
    float halfEdge = edgeLength / 2.0f;

    glColor4f(color.x, color.y, color.z, color.a); // 设置颜色

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
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // 重置颜色为白色
}

static void DrawCylinder(const glm::vec4& color, const glm::vec3& center, float radius, float height, const glm::vec3& normal, float startAngle, float endAngle, int segments) {
    glm::vec3 n = glm::normalize(normal); // 归一化法向量

    // 找到一个与法向量不平行的向量，计算局部坐标系
    glm::vec3 up = (fabs(n.x) < 0.99f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 tangent = glm::normalize(glm::cross(up, n));
    glm::vec3 bitangent = glm::cross(n, tangent);

    glColor4f(color.r, color.g, color.b, color.a); // 设置颜色

    float startRad = glm::radians(startAngle); // 将角度转换为弧度
    float endRad = glm::radians(endAngle);

    float deltaAngle = (endRad - startRad) / segments; // 每个段的角度增量

    glBegin(GL_QUADS);
    for (int i = 0; i < segments; ++i) {
        float currentAngle = startRad + i * deltaAngle;
        float nextAngle = currentAngle + deltaAngle;

        // 计算当前段和下一段的圆周顶点
        glm::vec3 p1 = center + radius * (cos(currentAngle) * tangent + sin(currentAngle) * bitangent);
        glm::vec3 p2 = center + radius * (cos(nextAngle) * tangent + sin(nextAngle) * bitangent);

        // 上圆和下圆顶点
        glm::vec3 topP1 = p1 + n * height;
        glm::vec3 topP2 = p2 + n * height;

        // 绘制侧面四边形
        glVertex3f(p1.x, p1.y, p1.z);       // 底部当前点
        glVertex3f(p2.x, p2.y, p2.z);       // 底部下一点
        glVertex3f(topP2.x, topP2.y, topP2.z); // 顶部下一点
        glVertex3f(topP1.x, topP1.y, topP1.z); // 顶部当前点
    }
    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // 重置颜色为白色
}

static void DrawCircle(const glm::vec4& color, const glm::vec3& center, float radius, const glm::vec3& normal,
    float startAngle, float endAngle, int slices) {
    glColor4f(color.r, color.g, color.b, color.a);

    glm::vec3 n = glm::normalize(normal);

    glm::vec3 up = (fabs(n.x) < 0.999f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 tangent = glm::normalize(glm::cross(up, n));
    glm::vec3 bitangent = glm::cross(n, tangent);

    float startRad = glm::radians(startAngle);
    float endRad = glm::radians(endAngle);

    float deltaAngle = (endRad - startRad) / slices;

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(center.x, center.y, center.z);

    for (int i = 0; i <= slices; ++i) {
        float currentAngle = startRad + i * deltaAngle;
        glm::vec3 point = center + radius * (cos(currentAngle) * tangent + sin(currentAngle) * bitangent);
        glVertex3f(point.x, point.y, point.z);
    }

    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

static void DrawSphere(const glm::vec4& color, const glm::vec3& center, float radius, int slices = 360, int stacks = 400) { //slices:经度精细度，stacks:纬度精细度
    glColor4f(color.r, color.g, color.b, color.a);

    glBegin(GL_TRIANGLE_STRIP);

    for (int stack = 0; stack < stacks; ++stack) {
        // 当前stacks和下一stacks的纬度角（弧度）
        float phi1 = glm::pi<float>() * (-0.5f + static_cast<float>(stack) / stacks);
        float phi2 = glm::pi<float>() * (-0.5f + static_cast<float>(stack + 1) / stacks);

        float z1 = radius * sin(phi1); // 当前stacks的 Z 坐标
        float z2 = radius * sin(phi2); // 下一stacks的 Z 坐标

        float r1 = radius * cos(phi1); // 当前stacks的水平半径
        float r2 = radius * cos(phi2); // 下一stacks的水平半径

        for (int slice = 0; slice <= slices; ++slice) {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(slice) / slices;

            float x = cos(theta);
            float y = sin(theta);

            glm::vec3 p1 = center + glm::vec3(r1 * x, r1 * y, z1);
            glm::vec3 p2 = center + glm::vec3(r2 * x, r2 * y, z2);

            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
        }
    }

    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

static void DrawPlane(const glm::vec4& color, const float width, const float height, const glm::vec3& center, const glm::vec3& normal) {
    glColor4f(color.x, color.y, color.z, color.a);

    glm::vec3 n = glm::normalize(normal); // 归一化法向量

    // 找到一个与法向量不平行的向量，计算第一个基向量
    glm::vec3 up = (fabs(n.x) < 0.999f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 tangent = glm::normalize(glm::cross(up, n)); // 平面上的第一个基向量
    glm::vec3 bitangent = glm::cross(n, tangent);          // 平面上的第二个基向量

    // 计算平面顶点
    glm::vec3 halfTangent = tangent * (width / 2.0f);
    glm::vec3 halfBitangent = bitangent * (height / 2.0f);

    glm::vec3 v0 = center - halfTangent - halfBitangent; // 左下
    glm::vec3 v1 = center + halfTangent - halfBitangent; // 右下
    glm::vec3 v2 = center + halfTangent + halfBitangent; // 右上
    glm::vec3 v3 = center - halfTangent + halfBitangent; // 左上

    // 绘制平面
    glBegin(GL_QUADS);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v3.x, v3.y, v3.z);
    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // 重置颜色为白色
}

static void DrawTriangle(const glm::vec4& color, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
    glColor4f(color.x, color.y, color.z, color.a);

    glBegin(GL_TRIANGLES);
    glVertex3f(p0.x, p0.y, p0.z);
    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
    glEnd();
   
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // 重置颜色为白色
}

static void DrawCubeLWH(const glm::vec4& color, const float length, const float width, const float height, const glm::vec3& center) {
    float halfLength = length / 2.0f;
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    // 正面（XY 平面）
    glm::vec3 frontCenter = glm::vec3(center.x, center.y, center.z + halfLength);
    DrawPlane(color, height, width, frontCenter, Backward);

    // 背面（XY 平面）
    glm::vec3 backCenter = glm::vec3(center.x, center.y, center.z - halfLength);
    DrawPlane(color, height, width, backCenter, Backward);

    // 顶面（XZ 平面）
    glm::vec3 topCenter = glm::vec3(center.x, center.y + halfHeight, center.z);
    DrawPlane(color, length, width, topCenter, Upward);

    // 底面（XZ 平面）
    glm::vec3 bottomCenter = glm::vec3(center.x, center.y - halfHeight, center.z);
    DrawPlane(color, length, width, bottomCenter, Upward);

    // 左面（YZ 平面）
    glm::vec3 leftCenter = glm::vec3(center.x - halfWidth, center.y, center.z);
    DrawPlane(color, length, height, leftCenter, Rightward);

    // 右面（YZ 平面）
    glm::vec3 rightCenter = glm::vec3(center.x + halfWidth, center.y, center.z);
    DrawPlane(color, length, height, rightCenter, Rightward);
}



static void DrawFirefly() {
    // https://www.douyin.com/note/7388460523207134483
	glm::vec3 Origin = glm::vec3(0.0f, 0.0f, 0.0f);

    // line1 done
    glm::vec3 Line1Origin = Origin;
	glm::vec3 nowDrawingPoint = Line1Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

	    DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
	    DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
	    DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	    DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		for (int i = 0; i < 8; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		}
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 8.0f * Rightward);
    }
    // endline1

    // line2 done
    glm::vec3 Line2Origin = Origin + 2.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line2Origin;
    {
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 8.0f * Rightward);
    }
	// endline2

	// line3 done
	glm::vec3 Line3Origin = Line2Origin + 1.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line3Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
    }
	// endline3

	// line4 done
	glm::vec3 Line4Origin = Line3Origin + 1.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line4Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline4

	// line5 done
	glm::vec3 Line5Origin = Line4Origin + 1.0f * Upward;
	nowDrawingPoint = Line5Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline5

	// line6 done
	glm::vec3 Line6Origin = Line5Origin + 1.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line6Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline6

	// line7 done
	glm::vec3 Line7Origin = Line6Origin + 1.0f * Upward;
	nowDrawingPoint = Line7Origin;
	{
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline7

	// line8 done
	glm::vec3 Line8Origin = Line7Origin + 1.0f * Leftward + 1.0f * Upward;
	nowDrawingPoint = Line8Origin;
	{
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline8

	// line9 done
	glm::vec3 Line9Origin = Line8Origin + 1.0f * Upward;
	nowDrawingPoint = Line9Origin;
	{
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline9

	// line10 done
	glm::vec3 Line10Origin = Line9Origin + 1.0f * Upward;
	nowDrawingPoint = Line10Origin;
	{
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline10

	// line11 done
	glm::vec3 Line11Origin = Line10Origin + 1.0f * Upward;
	nowDrawingPoint = Line11Origin;
	{
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward); // 图里是空的，这里填充一个cha色
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline11

	// line12 done
	glm::vec3 Line12Origin = Line11Origin + 1.0f * Upward;
	nowDrawingPoint = Line12Origin;
	{
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
	}
	// endline12

	// line13 done
	glm::vec3 Line13Origin = Line12Origin + 1.0f * Upward + 1.0f * Rightward;
    nowDrawingPoint = Line13Origin;
    {
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline13

	// line14 done
	glm::vec3 Line14Origin = Line13Origin + 1.0f * Upward;
    nowDrawingPoint = Line14Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
		for (int i = 0; i < 7; i++) {
			DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
			DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		}
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline14

	// line15 done
	glm::vec3 Line15Origin = Line14Origin + 1.0f * Upward;
	nowDrawingPoint = Line15Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
		DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline15

	// line16 done
	glm::vec3 Line16Origin = Line15Origin + 1.0f * Upward + 1.0f * Rightward;
    nowDrawingPoint = Line16Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline16

	// line17 done
	glm::vec3 Line17Origin = Line16Origin + 1.0f * Upward;
    nowDrawingPoint = Line17Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline17

	// line18 done
	glm::vec3 Line18Origin = Line17Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line18Origin;
    {

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Orange),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); } // 原图留空，给他补一个
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline18

	// line19 done
	glm::vec3 Line19Origin = Line18Origin + 1.0f * Upward;
    nowDrawingPoint = Line19Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline19

    // line20 done
    glm::vec3 Line20Origin = Line19Origin + 1.0f * Upward + 1.0f * Rightward;
    nowDrawingPoint = Line20Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
    // endline20

    // line21 done
    glm::vec3 Line21Origin = Line20Origin + 1.0f * Upward + 1.0f * Rightward;
    nowDrawingPoint = Line21Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
    // endline21

    // line22 done
    glm::vec3 Line22Origin = Line21Origin + 1.0f * Upward;
    nowDrawingPoint = Line22Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
    // endline22

	// line23 done
	glm::vec3 Line23Origin = Line22Origin + 1.0f * Upward + 1.0f * Leftward;
    nowDrawingPoint = Line23Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline23

    // line24 done
	glm::vec3 Line24Origin = Line23Origin + 1.0f * Upward + 1.0f * Leftward;
	nowDrawingPoint = Line24Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline24

    // line25 done
	glm::vec3 Line25Origin = Line24Origin + 1.0f * Upward + 1.0f * Leftward;
    nowDrawingPoint = Line25Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline25

	// line26 done
	glm::vec3 Line26Origin = Line25Origin + 1.0f * Upward;
	nowDrawingPoint = Line26Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline26

	// line27 done
	glm::vec3 Line27Origin = Line26Origin + 1.0f * Upward + 1.0f * Leftward;
	nowDrawingPoint = Line27Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline27

	// line28 done
	glm::vec3 Line28Origin = Line27Origin + 1.0f * Upward;
	nowDrawingPoint = Line28Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown1),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline28

	// line29 done
	glm::vec3 Line29Origin = Line28Origin + 1.0f * Upward;
	nowDrawingPoint = Line29Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline29

	// line30 done
	glm::vec3 Line30Origin = Line29Origin + 1.0f * Upward;
	nowDrawingPoint = Line30Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 8.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline30

	// line31 done
	glm::vec3 Line31Origin = Line30Origin + 1.0f * Upward;
	nowDrawingPoint = Line31Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline31

	// line32 done
	glm::vec3 Line32Origin = Line31Origin + 1.0f * Upward;
	nowDrawingPoint = Line32Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 8.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline32

	// line33 done
	glm::vec3 Line33Origin = Line32Origin + 1.0f * Upward + 1.0F * Rightward;
	nowDrawingPoint = Line33Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 24; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline33

	// line34 done
	glm::vec3 Line34Origin = Line33Origin + 1.0f * Upward;
	nowDrawingPoint = Line34Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 29; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline34

	// line35 done
	glm::vec3 Line35Origin = Line34Origin + 1.0f * Upward;
	nowDrawingPoint = Line35Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 30; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline35

	// line36 done
	glm::vec3 Line36Origin = Line35Origin + 1.0f * Upward + 1.0F * Rightward;
	nowDrawingPoint = Line36Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 5; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
    }
	// endline36

	// line37 done
	glm::vec3 Line37Origin = Line36Origin + 1.0f * Upward;
	nowDrawingPoint = Line37Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline37

	// line38 done
	glm::vec3 Line38Origin = Line37Origin + 1.0f * Upward + 1.0F * Rightward;
	nowDrawingPoint = Line38Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 4; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 36; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline38

	// line39 done
	glm::vec3 Line39Origin = Line38Origin + 1.0f * Upward;
	nowDrawingPoint = Line39Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 5.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
            DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 32; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline39

	// line40 done
	glm::vec3 Line40Origin = Line39Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line40Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 32; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline40

	// line41 done
	glm::vec3 Line41Origin = Line40Origin + 1.0f * Upward;
	nowDrawingPoint = Line41Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 20; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline41

	// line42 done
	glm::vec3 Line42Origin = Line41Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line42Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 36; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline42

	// line43 done
	glm::vec3 Line43Origin = Line42Origin + 1.0f * Upward;
	nowDrawingPoint = Line43Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 38; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline43

	// line44 done
	glm::vec3 Line44Origin = Line43Origin + 1.0f * Upward;
	nowDrawingPoint = Line44Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 40; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline44

	// line45 done
	glm::vec3 Line45Origin = Line44Origin + 1.0f * Upward;
	nowDrawingPoint = Line45Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 21; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline45

	// line46 done
	glm::vec3 Line46Origin = Line45Origin + 1.0f * Upward;
	nowDrawingPoint = Line46Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline46

	// line47 done
	glm::vec3 Line47Origin = Line46Origin + 1.0f * Upward;
	nowDrawingPoint = Line47Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline47

	// line48 done
	glm::vec3 Line48Origin = Line47Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line48Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline48

	// line49 done
	glm::vec3 Line49Origin = Line48Origin + 1.0f * Upward;
	nowDrawingPoint = Line49Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline49

	// line50 done
	glm::vec3 Line50Origin = Line49Origin + 1.0f * Upward;
	nowDrawingPoint = Line50Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline50

    // line51 done
	glm::vec3 Line51Origin = Line50Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line51Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline51

	// line52 done
	glm::vec3 Line52Origin = Line51Origin + 1.0f * Upward;
	nowDrawingPoint = Line52Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),    1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline52

	// line53 done
	glm::vec3 Line53Origin = Line52Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line53Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline53

	// line54 done
	glm::vec3 Line54Origin = Line53Origin + 1.0f * Upward;
	nowDrawingPoint = Line54Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_BlueGray),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline54

	// line55 done
	glm::vec3 Line55Origin = Line54Origin + 1.0f * Upward;
	nowDrawingPoint = Line55Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray),        1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline55

	// line56 done
	glm::vec3 Line56Origin = Line55Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line56Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_IndigoBlue),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),           1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),        1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),      1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),         1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),         1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),          1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),       1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline56

	// line57 done
	glm::vec3 Line57Origin = Line56Origin + 1.0f * Upward;
	nowDrawingPoint = Line57Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline57

	// line58 done
	glm::vec3 Line58Origin = Line57Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line58Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline58

	// line59 done
	glm::vec3 Line59Origin = Line58Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line59Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline59

	// line60 done
	glm::vec3 Line60Origin = Line59Origin + 1.0f * Upward;
	nowDrawingPoint = Line60Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline60

	// line61 done
	glm::vec3 Line61Origin = Line60Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line61Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline61

	// line62 done
	glm::vec3 Line62Origin = Line61Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line62Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline62

	// line63 done
	glm::vec3 Line63Origin = Line62Origin + 1.0f * Upward;
	nowDrawingPoint = Line63Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline63

	// line64 done
	glm::vec3 Line64Origin = Line63Origin + 1.0f * Upward;
	nowDrawingPoint = Line64Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Pink),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown2),  1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline64

	// line65 done
	glm::vec3 Line65Origin = Line64Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line65Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline65

	// line66 done
	glm::vec3 Line66Origin = Line65Origin + 1.0f * Upward;
	nowDrawingPoint = Line66Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 20; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline66

	// line67 done
	glm::vec3 Line67Origin = Line66Origin + 1.0f * Upward;
	nowDrawingPoint = Line67Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 21; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline67

	// line68 done
	glm::vec3 Line68Origin = Line67Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line68Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 21; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline68

	// line69 done
	glm::vec3 Line69Origin = Line68Origin + 1.0f * Upward;
	nowDrawingPoint = Line69Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 21; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline69

	// line70 done
    glm::vec3 Line70Origin = Line69Origin + 1.0f * Upward;
	nowDrawingPoint = Line70Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline70

	// line71 done
	glm::vec3 Line71Origin = Line70Origin + 1.0f * Upward;
	nowDrawingPoint = Line71Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline71

	// line72 done
	glm::vec3 Line72Origin = Line71Origin + 1.0f * Upward;
	nowDrawingPoint = Line72Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline72

	// line73 done
	glm::vec3 Line73Origin = Line72Origin + 1.0f * Upward;
	nowDrawingPoint = Line73Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline73

	// line74 done
	glm::vec3 Line74Origin = Line73Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line74Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline74

	// line75 done
	glm::vec3 Line75Origin = Line74Origin + 1.0f * Upward;
	nowDrawingPoint = Line75Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline75

	// line76 done
	glm::vec3 Line76Origin = Line75Origin + 1.0f * Upward;
	nowDrawingPoint = Line76Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline76

	// line77 done
	glm::vec3 Line77Origin = Line76Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line77Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline77

	// line78 done
	glm::vec3 Line78Origin = Line77Origin + 1.0f * Upward;
	nowDrawingPoint = Line78Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 9; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline78

	// line79 done
	glm::vec3 Line79Origin = Line78Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line79Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline79

	// line80 done
	glm::vec3 Line80Origin = Line79Origin + 1.0f * Upward;
	nowDrawingPoint = Line80Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline80

	// line81 done
	glm::vec3 Line81Origin = Line80Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line81Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline81

	// line82 done
	glm::vec3 Line82Origin = Line81Origin + 1.0f * Upward;
	nowDrawingPoint = Line82Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline82

	// line83 done
	glm::vec3 Line83Origin = Line82Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line83Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 20; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Brown), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline83

	// line84 done
	glm::vec3 Line84Origin = Line83Origin + 1.0f * Upward;
	nowDrawingPoint = Line84Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 20; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline84

	// line85 done
	glm::vec3 Line85Origin = Line84Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line85Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline85

	// line86 done
	glm::vec3 Line86Origin = Line85Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line86Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline86

	// line87 done
	glm::vec3 Line87Origin = Line86Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line87Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 25; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 4.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline87

	// line88 done
	glm::vec3 Line88Origin = Line87Origin + 1.0f * Upward;
	nowDrawingPoint = Line88Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 25; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
    }
	// endline88

	// line89 done
	glm::vec3 Line89Origin = Line88Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line89Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 26; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 2.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline89

	// line90 done
	glm::vec3 Line90Origin = Line89Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line90Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 26; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_White),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 3.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline90

	// line91 done
	glm::vec3 Line91Origin = Line90Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line91Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 23; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_White),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 5.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline91

	// line92 done
	glm::vec3 Line92Origin = Line91Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line92Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 20; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 6.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline92

	// line93 done
	glm::vec3 Line93Origin = Line92Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line93Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_cha),     1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 7.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline93

	// line94 done
	glm::vec3 Line94Origin = Line93Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line94Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gold1),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 10; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Brown),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),     1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 8.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline94

	// line95 done
	glm::vec3 Line95Origin = Line94Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line95Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),      1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 12; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black),    1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),       1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 9.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline95

	// line96 done
	glm::vec3 Line96Origin = Line95Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line96Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        for (int i = 0; i < 19; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 25; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 10.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline96

	// line97 done
	glm::vec3 Line97Origin = Line96Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line97Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 22; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 11.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Firefly), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black),   1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline97

	// line98 done
	glm::vec3 Line98Origin = Line97Origin + 1.0f * Upward + 2.0f * Rightward;
	nowDrawingPoint = Line98Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 14; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 12.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline98

	// line99 done
	glm::vec3 Line99Origin = Line98Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line99Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 12.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline99

	// line100 done
	glm::vec3 Line100Origin = Line99Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line100Origin;
    {

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 12.0f * Rightward);
    }
	// endline100

	// line101 done
	glm::vec3 Line101Origin = Line100Origin + 1.0f * Upward + 2.0f * Rightward;
	nowDrawingPoint = Line101Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 17; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 8; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Gray1), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline101

	// line102 done
	glm::vec3 Line102Origin = Line101Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line102Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 16; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
    }
	// endline102

	// line103 done
	glm::vec3 Line103Origin = Line102Origin + 1.0f * Upward + 2.0f * Rightward;
	nowDrawingPoint = Line103Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 36; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline103

	// line104 done
	glm::vec3 Line104Origin = Line103Origin + 1.0f * Upward + 1.0f * Rightward;
	nowDrawingPoint = Line104Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 18; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 13; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline104

	// line105 done
	glm::vec3 Line105Origin = Line104Origin + 1.0f * Upward + 2.0f * Rightward;
	nowDrawingPoint = Line105Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 15; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 7; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline105

	// line106 done
	glm::vec3 Line106Origin = Line105Origin + 1.0f * Upward + 2.0f * Rightward;
	nowDrawingPoint = Line106Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 11; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),  1.0f, nowDrawingPoint += 1.0f * Rightward); }
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward);
        for (int i = 0; i < 6; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline106

	// line107 done
	glm::vec3 Line107Origin = Line106Origin + 1.0f * Upward + 3.0f * Rightward;
	nowDrawingPoint = Line107Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 4; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_cha),   1.0f, nowDrawingPoint += 1.0f * Rightward); }
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 2.0f * Rightward);
        for (int i = 0; i < 5; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }

        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 3.0f * Rightward);
        for (int i = 0; i < 2; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline107

	// line108 done
	glm::vec3 Line108Origin = Line107Origin + 1.0f * Upward + 5.0f * Rightward;
	nowDrawingPoint = Line108Origin;
    {
        DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint);
        for (int i = 0; i < 3; i++) { DrawCube(TransHEXtoVec4WithAlpha(Color_Black), 1.0f, nowDrawingPoint += 1.0f * Rightward); }
    }
	// endline108
} 

static void DrawBuildings() {
    // Lumina Square
    {
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray1), 30.0f, 110.0f, PlaneOrigin + glm::vec3(-95.0f, 15.0f, 70.0f), Backward);
        DrawCylinder(TransHEXtoVec4WithAlpha(Color_cha), PlaneOrigin + glm::vec3(-40.0f, 0.0f, 50.0f), 20, 30, Upward, 0, 90, 360);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray1), 110.0f, 30.0f, PlaneOrigin + glm::vec3(-20.0f, 15.0f, -5.0f), Rightward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray1), 130.0f, 30.0f, PlaneOrigin + glm::vec3(-150.0f, 15.0f, 5.0f), Rightward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray1), 30.0f, 130.0f, PlaneOrigin + glm::vec3(-85.0f, 15.0f, -60.0f), Backward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Roof), 130.0f, 130.0f, PlaneOrigin + glm::vec3(-85.0f, 30.0f, 5.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Roof), 130.0f, 130.0f, PlaneOrigin + glm::vec3(-85.0f, 40.0f, 5.0f), Upward);
        DrawTexturedPlane(PlaneOrigin + glm::vec3(-85.0f, 35.0f, 70.0f), 10.0f, 130.0f, Texture_LuminaSquare, Forward, false, true);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray2), 10.0f, 130.0f, PlaneOrigin + glm::vec3(-85.0f, 35.0f, -60.0f), Backward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray2), 130.0f, 10.0f, PlaneOrigin + glm::vec3(-150.0f, 35.0f, 5.0f), Rightward);
        DrawTexturedPlane(PlaneOrigin + glm::vec3(-20.0f, 35.0f, 5.0f),  130.0f, 10.0f, Texture_LuminaSquare, Leftward, false, false);
    }
    // end Lumina Square

    // QUALITY TEA
    {
        DrawCubeLWH(TransHEXtoVec4WithAlpha(Color_Gray1), 10.0f, 10.0f, 10.0f, PlaneOrigin + glm::vec3(-25.0f, 5.0f, 125.0f));
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Roof), 15.0f, 15.0f, PlaneOrigin + glm::vec3(-25.0f, 10.0f, 125.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Roof), 15.0f, 15.0f, PlaneOrigin + glm::vec3(-25.0f, 12.0f, 125.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Roof), 15.0f, 2.0f,  PlaneOrigin + glm::vec3(-17.5f, 11.0f, 125.0f), Rightward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Roof), 15.0f, 2.0f,  PlaneOrigin + glm::vec3(-32.5f, 11.0f, 125.0f), Rightward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Roof), 2.0f, 15.0f,  PlaneOrigin + glm::vec3(-25.0f, 11.0f, 132.5f), Backward);
        DrawTexturedPlane(PlaneOrigin + glm::vec3(-25.0f, 11.0f, 117.5f), 2.0f, 15.0f, Texture_QUALITYTEA, Backward, true, true);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Black), 7.0f, 8.0f, PlaneOrigin + glm::vec3(-25.0f, 3.5f, 119.9f), Backward);
    }
    // end QUALITY TEA

    DrawTexturedPlane(PlaneOrigin + glm::vec3(-30.0f, 50.0f, 60.0f), 20.0f, 40.0f, Texture_AD, Leftward + Forward, false, true);

    // 141,LSS,JC
    {
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray3), 35.0f, 10.0f, PlaneOrigin + glm::vec3(20.0f, 5.0f, 132.5f), Leftward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray3), 10.0f, 40.0f, PlaneOrigin + glm::vec3(40.0f, 5.0f, 115.0f), Backward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray3), 10.0f, 14.142f, PlaneOrigin + glm::vec3(65.0f, 5.0f, 110.0f), Forward + Leftward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray3), 10.0f, 20.0f, PlaneOrigin + glm::vec3(80.0f, 5.0f, 105.0f), Backward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray3), 45.0f, 10.0f, PlaneOrigin + glm::vec3(90.0f, 5.0f, 127.5f), Leftward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray3), 10.0f, 70.0f, PlaneOrigin + glm::vec3(55.0f, 5.0f, 150.0f), Backward);

        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray1), 35.0f, 70.0f, PlaneOrigin + glm::vec3(55.0f, 10.0f, 132.5f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray1), 10.0f, 20.0f, PlaneOrigin + glm::vec3(80.0f, 10.0f, 110.0f), Upward);
        DrawTriangle(TransHEXtoVec4WithAlpha(Color_Gray1),
            PlaneOrigin + glm::vec3(60.0f, 10.0f, 115.0f), 
            PlaneOrigin + glm::vec3(70.0f, 10.0f, 115.0f), 
            PlaneOrigin + glm::vec3(70.0f, 10.0f, 105.0f));

        DrawPlane(TransHEXtoVec4WithAlpha(Color_Black), 8.0f, 11.314f, PlaneOrigin + glm::vec3(64.95f, 4.0f, 110.0f), Forward + Leftward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray4), 8.0f, 18.0f, PlaneOrigin + glm::vec3(80.0f, 4.0f, 104.9f), Backward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gray4), 8.0f, 38.0f, PlaneOrigin + glm::vec3(40.0f, 4.0f, 114.9f), Backward);

        DrawTexturedPlane(PlaneOrigin + glm::vec3(40.0f, 11.0f, 115.0f), 2.0f, 38.0f, Texture_141, Backward, true, true);
        DrawTexturedPlane(PlaneOrigin + glm::vec3(65.0f, 11.0f, 110.0f), 2.0f, 14.141f, Texture_LSS, Backward + Rightward, true, true);
        DrawTexturedPlane(PlaneOrigin + glm::vec3(80.0f, 11.0f, 105.0f), 2.0f, 18.0f, Texture_JC, Backward, true, true);
    }
    // end 141,LSS,JC
}

static void DrawRoads() {
    // 0.1f 不被地面遮挡
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 140.0f, PlaneOrigin + glm::vec3(-80.0f, 0.1f, 100.0f), Upward);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 140.0f, PlaneOrigin + glm::vec3( 80.0f, 0.1f, 100.0f), Upward);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 50.0f, 1.0f,  PlaneOrigin + glm::vec3(-10.0f, 0.1f, 125.0f), Upward);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 50.0f, 1.0f,  PlaneOrigin + glm::vec3( 10.0f, 0.1f, 125.0f), Upward);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 1.0f,   PlaneOrigin + glm::vec3(-10.0f, 0.1f, 100.0f), Upward); // 修补残缺部分
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 1.0f,   PlaneOrigin + glm::vec3( 10.0f, 0.1f, 100.0f), Upward); // 修补残缺部分

    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 140.0f, PlaneOrigin + glm::vec3(-80.0f, 0.1f, 80.0f), Upward);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 140.0f, PlaneOrigin + glm::vec3( 80.0f, 0.1f, 80.0f), Upward);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 230.0f, 1.0f, PlaneOrigin + glm::vec3(-10.0f, 0.1f, -35.0f), Upward);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 230.0f, 1.0f, PlaneOrigin + glm::vec3( 10.0f, 0.1f, -35.0f), Upward);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 1.0f,   PlaneOrigin + glm::vec3(-10.0f, 0.1f, 80.0f), Upward); // 修补残缺部分
    DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 1.0f,   PlaneOrigin + glm::vec3( 10.0f, 0.1f, 80.0f), Upward); // 修补残缺部分

    // 斑马线crosswalk
    {
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3(-7.0f, 0.1f, 75.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3(-4.0f, 0.1f, 75.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3(-1.0f, 0.1f, 75.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3( 2.0f, 0.1f, 75.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3( 5.0f, 0.1f, 75.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3( 8.0f, 0.1f, 75.0f), Upward);

        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3(-8.0f, 0.1f, 105.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3(-5.0f, 0.1f, 105.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3(-2.0f, 0.1f, 105.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3( 1.0f, 0.1f, 105.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3( 4.0f, 0.1f, 105.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3( 7.0f, 0.1f, 105.0f), Upward);

        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3(-15.0f, 0.1f, 82.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3(-15.0f, 0.1f, 85.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3(-15.0f, 0.1f, 88.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3(-15.0f, 0.1f, 91.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3(-15.0f, 0.1f, 94.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3(-15.0f, 0.1f, 97.0f), Upward);

        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3( 15.0f, 0.1f, 83.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3( 15.0f, 0.1f, 86.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3( 15.0f, 0.1f, 89.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3( 15.0f, 0.1f, 92.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3( 15.0f, 0.1f, 95.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3( 15.0f, 0.1f, 98.0f), Upward);
    }
    //end斑马线crosswalk

    //双黄+停止线
    {
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gold), 215.0f, 0.3f, PlaneOrigin + glm::vec3(-0.35f, 0.1f, -42.5f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gold), 215.0f, 0.3f, PlaneOrigin + glm::vec3( 0.35f, 0.1f, -42.5f), Upward);

        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gold), 35.0f, 0.3f, PlaneOrigin + glm::vec3(-0.35f, 0.1f, 132.5f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gold), 35.0f, 0.3f, PlaneOrigin + glm::vec3( 0.35f, 0.1f, 132.5f), Upward);

        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gold), 0.3f, 125.0f, PlaneOrigin + glm::vec3(-87.5f, 0.1f, 89.65f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gold), 0.3f, 125.0f, PlaneOrigin + glm::vec3(-87.5f, 0.1f, 90.35f), Upward);

        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gold), 0.3f, 125.0f, PlaneOrigin + glm::vec3(87.5f, 0.1f, 89.65f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_Gold), 0.3f, 125.0f, PlaneOrigin + glm::vec3(87.5f, 0.1f, 90.35f), Upward);

        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3(-5.0f, 0.1f, 65.5f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 1.0f, 10.0f, PlaneOrigin + glm::vec3( 5.0f, 0.1f, 114.5f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3(-24.5f, 0.1f, 95.0f), Upward);
        DrawPlane(TransHEXtoVec4WithAlpha(Color_White), 10.0f, 1.0f, PlaneOrigin + glm::vec3( 24.5f, 0.1f, 85.0f), Upward);
    }
    //end双黄+停止线
}

static void DrawClock() {
	DrawCylinder(TransHEXtoVec4WithAlpha(Color_Firefly), PlaneOrigin + glm::vec3(120.0f, 30.0f, 0.0f), 25.0f, 5.0f, Leftward, 0.0f, 360.0f, 360);
	DrawTextureCircle(Texture_Clock, PlaneOrigin + glm::vec3(117.5f, 30.0f, 0.0f), 25.0f, Rightward, 0.0f, 360.0f, 360, true, 0.0);
    DrawCircle(TransHEXtoVec4WithAlpha(Color_Firefly), PlaneOrigin + glm::vec3(120.0f, 30.0f, 0.0f), 25.0f, Leftward, 0.0f, 360.0f, 360);
    DrawCylinder(TransHEXtoVec4WithAlpha(Color_Firefly), PlaneOrigin + glm::vec3(120.0f, 30.0f, 0.0f), 0.5f, 5.0f, Leftward, 0.0f, 360.0f, 360); // 表柱
    DrawCircle(TransHEXtoVec4WithAlpha(Color_Firefly), PlaneOrigin + glm::vec3(115.0f, 30.0f, 0.0f), 0.5f, Leftward, 0.0f, 360.0f, 360);
}

static void DrawAllStatic() {
    DrawPlane(TransHEXtoVec4WithAlpha(Color_Ground), 300.0f, 300.0f, PlaneOrigin, Upward); // 地板
    DrawFirefly();
    DrawRoads();
    DrawBuildings();
    DrawClock();
}



static void DrawAniCubeObj() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, obj.position); // 平移

    // 旋转：先绕 X 轴，后绕 Y 轴，最后绕 Z 轴
    model = glm::rotate(model, glm::radians(obj.rotation.x), Rightward); // 绕 X 轴旋转
    model = glm::rotate(model, glm::radians(obj.rotation.y), Upward); // 绕 Y 轴旋转
    model = glm::rotate(model, glm::radians(obj.rotation.z), Backward); // 绕 Z 轴旋转
    model = glm::scale(model, obj.scale); // 缩放

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(&model[0][0]);

    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, obj.alpha);

    DrawTexturedCube(glm::vec3(97.0f, 85.0f, 0.0f), 0.2f, Texture_CWGitHubLink);

    glPopAttrib(); // 恢复之前状态
    glPopMatrix();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

static void DrawSingleGuardrail(const AnimatedObject& obj, const glm::vec3& rotationCenter) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, rotationCenter);
    model = glm::rotate(model, glm::radians(obj.rotation.y), Upward);

    model = glm::translate(model, obj.position - rotationCenter);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(&model[0][0]);

    // 绘制护栏
    DrawCylinder(TransHEXtoVec4WithAlpha(Color_Firefly), PlaneOrigin, 0.5f, 3.0f, Upward, 0.0f, 360.0f, 360);
    DrawCircle(TransHEXtoVec4WithAlpha(Color_Firefly), PlaneOrigin + 3.0f * Upward, 0.5f, Upward, 0.0f, 360.0f, 360);
    DrawPlane(TransHEXtoVec4WithAlpha(Color_IndigoBlue), 2.5f, 8.0f, PlaneOrigin + 1.5f * Upward + 4.0f * Leftward, Forward);

    glPopMatrix();
}

static void DrawAniGuardrails() {
    for (int i = 0; i < 8; i++) {
        if (isFirstRun) {
            GuardrailProps[i].position = GuardrailPositionArray[i];
            GuardrailProps[i].rotation = GuardrailRotationArray[i];
            GuardrailProps[i].scale = glm::vec3(1.0f, 1.0f, 1.0f);
            GuardrailProps[i].alpha = 1.0f;
        }
        else {
            break;
        }
    }
	isFirstRun = false;
    for (int i = 0; i < 8; i++) {
        DrawSingleGuardrail(GuardrailProps[i], PlaneOrigin + GuardrailPositionArray[i]);
    }
}

static void DrawAniVehicle_Small() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, Vehicle_S.position);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(&model[0][0]);

    const float WheelRadius = 0.8f;
    glm::vec3 S_Origin = PlaneOrigin + glm::vec3(30.0f, 1.0f + WheelRadius, 83.0f);
    // Body 车身
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            DrawCube(TransHEXtoVec4WithAlpha(Color_PinkRed), 2.0f, S_Origin + glm::vec3(2.0f * j, 0.0f, 2.0f * i));
        }
    }
    DrawCubeLWH(TransHEXtoVec4WithAlpha(Color_Brown), 4.0f, 6.0f, 2.0f, S_Origin + glm::vec3(3.0f, 2.0f, 2.0f));

    // Wheel 轮子
    DrawCylinder(TransHEXtoVec4WithAlpha(Color_Black), S_Origin + glm::vec3(0.0f, -1.0f, 5.0f), WheelRadius, 0.2f, Backward, 0.0f, 360.0f, 360);
    DrawCircle(TransHEXtoVec4WithAlpha(Color_Gold1), S_Origin + glm::vec3(0.0f, -1.0f, 5.2f), WheelRadius, Backward, 0.0f, 360.0f, 360);
    DrawCylinder(TransHEXtoVec4WithAlpha(Color_Black), S_Origin + glm::vec3(6.0f, -1.0f, 5.0f), WheelRadius, 0.2f, Backward, 0.0f, 360.0f, 360);
    DrawCircle(TransHEXtoVec4WithAlpha(Color_Gold1), S_Origin + glm::vec3(6.0f, -1.0f, 5.2f), WheelRadius, Backward, 0.0f, 360.0f, 360);
    DrawCylinder(TransHEXtoVec4WithAlpha(Color_Black), S_Origin + glm::vec3(0.0f, -1.0f, -1.0f), WheelRadius, 0.2f, Forward, 0.0f, 360.0f, 360);
    DrawCircle(TransHEXtoVec4WithAlpha(Color_Gold1), S_Origin + glm::vec3(0.0f, -1.0f, -1.2f), WheelRadius, Forward, 0.0f, 360.0f, 360);
    DrawCylinder(TransHEXtoVec4WithAlpha(Color_Black), S_Origin + glm::vec3(6.0f, -1.0f, -1.0f), WheelRadius, 0.2f, Forward, 0.0f, 360.0f, 360);
    DrawCircle(TransHEXtoVec4WithAlpha(Color_Gold1), S_Origin + glm::vec3(6.0f, -1.0f, -1.2f), WheelRadius, Forward, 0.0f, 360.0f, 360);

    glPopMatrix();
}

static void DrawAniHand_H(const AnimatedObject& obj, const glm::vec3& rotationCenter) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, rotationCenter);
    model = glm::rotate(model, glm::radians(obj.rotation.x), Rightward);
    model = glm::translate(model, obj.position - rotationCenter);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(&model[0][0]);

    DrawCylinder(TransHEXtoVec4WithAlpha(Color_Gold1), PlaneOrigin + glm::vec3(117.0f, 30.0f, 0.0f), 0.2f, 10.0f, Upward, 0.0f, 360.0f, 360); // 时针Hour hand

    glPopMatrix();
}

static void DrawAniHand_M(const AnimatedObject& obj, const glm::vec3& rotationCenter) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, rotationCenter);
    model = glm::rotate(model, glm::radians(obj.rotation.x), Rightward);
    model = glm::translate(model, obj.position - rotationCenter);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(&model[0][0]);

    DrawCylinder(TransHEXtoVec4WithAlpha(Color_Gold), PlaneOrigin + glm::vec3(116.0f, 30.0f, 0.0f), 0.2f, 20.0f, Upward, 0.0f, 360.0f, 360); // 分针Minute hand

    glPopMatrix();
}

static void DrawAniHand_S(const AnimatedObject& obj, const glm::vec3& rotationCenter) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, rotationCenter);
    model = glm::rotate(model, glm::radians(obj.rotation.x), Rightward);
    model = glm::translate(model, obj.position - rotationCenter);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(&model[0][0]);

    DrawCylinder(TransHEXtoVec4WithAlpha(Color_PinkRed), PlaneOrigin + glm::vec3(115.5f, 30.0f, 0.0f), 0.1f, 20.0f, Upward, 0.0f, 360.0f, 360); // 秒针Second hand

    glPopMatrix();
}



static void DrawAllAnimate() {
    DrawAniCubeObj();
    DrawAniGuardrails();
    DrawAniVehicle_Small();
    DrawAniHand_H(HHand, PlaneOrigin + glm::vec3(120.0f, 30.0f, 0.0f));
    DrawAniHand_M(MHand, PlaneOrigin + glm::vec3(120.0f, 30.0f, 0.0f));
    DrawAniHand_S(SHand, PlaneOrigin + glm::vec3(120.0f, 30.0f, 0.0f));
}

static void UpdateAnimation(float deltaTime) {
    static float time = 0.0f;               // 时间累加器，用于周期性效果
    static const float Slice = 180.0f;      // 分片数，控制动画的平滑程度
    static float ADtime = 0.0f;

    ADtime += deltaTime;
    if (ADtime >= 0.1f) {
        Texture_AD = AD[CurrentADFrame];
		CurrentADFrame = (CurrentADFrame + 1) % 44;
        ADtime -= 0.1f;
    }

    time += deltaTime;
    obj.alpha = (sin(time * 2.0f) + 1.0f) / 2.0f;


    // 栏杆动画
    if (guardrailState.isAnimating) {
        if (guardrailState.currentStep < Slice) {
            for (int i = 0; i < 8; i++) {
                float rotationDelta = GuardrailChangeArray[i].y / Slice; // 每帧旋转量
                GuardrailProps[i].rotation.y += guardrailState.RotateFlag ? rotationDelta : -rotationDelta;
            }
            guardrailState.currentStep++;
        }
        else {
            // 动画完成，重置状态
            guardrailState.isAnimating = false;
            guardrailState.currentStep = 0;
            guardrailState.RotateFlag = !guardrailState.RotateFlag; // 切换方向
        }
    }
    else {
        guardrailState.rotationTimer += deltaTime;
        if (guardrailState.rotationTimer >= 6.0f) {
            guardrailState.isAnimating = true;
            guardrailState.rotationTimer -= 6.0f;
        }
    }


    // 车s
    if (Vehicle_S.direction == Leftward && glm::length(Vehicle_S.position.x - 0.0f) <= 0.1f) { // 从右往左到达路口←
        Vehicle_S.speed = guardrailState.RotateFlag ? 0.0f : 1.0f;
    }
    if (Vehicle_S.direction == Rightward && glm::length(Vehicle_S.position.x - -65.0f) <= 0.1f) { // 从左往右到达路口→
        Vehicle_S.speed = guardrailState.RotateFlag ? 0.0f : 1.0f;
    }
    if (Vehicle_S.position.x <= -180.0f) { // 到达左侧边界
        Vehicle_S.position += glm::vec3(0.0f, 0.0f, 10.0f);
        Vehicle_S.direction = Rightward;
    }
    if (Vehicle_S.position.x >= 120.0f) { // 到达右侧边界
        Vehicle_S.position += glm::vec3(0.0f, 0.0f, -10.0f);
        Vehicle_S.direction = Leftward;
    }
    Vehicle_S.position += Vehicle_S.direction * Vehicle_S.speed * deltaTime * 10.0f; // 6s通过路口，乘个系数

    GetClockHandAngles(HHand.rotation.x, MHand.rotation.x, SHand.rotation.x);
}



int main() {
    PrintSystemInfo();

	ResetParams();

    if (!glfwInit()) {
        std::cerr << GetCurrentTimeString() << " Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GuestLiang 3D Scene - Firefly + Lumina Square", nullptr, nullptr);
    if (window == NULL) {
        std::cerr << GetCurrentTimeString() << " Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

	glfwSetScrollCallback(window, ScrollCallback);              // 滚轮回调
	glfwSetCursorPosCallback(window, MouseCallback);            // 鼠标移动回调
	glfwSetMouseButtonCallback(window, MouseButtonCallback);    // 鼠标按键回调

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << GetCurrentTimeString() << " Failed to initialize GLAD" << std::endl;
        return -1;
    }

	InitTextures();		   									    // 初始化纹理

    glEnable(GL_DEPTH_TEST);                                    // 启用深度测试，避免遮挡关系
	glEnable(GL_BLEND); 									    // 启用混合
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		    // 设置混合因子


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window, deltaTime);                        // 处理键盘输入
        UpdateAnimation(deltaTime);                             // 更新动画

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view;
        if (isFreeMoveMode) {
            view = glm::lookAt(camPos, camPos + camFront, camUp);
        } else {
            view = fixViewMatrix;
        }

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);                // Set the background color to sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);                            // 设置投影矩阵
        glLoadMatrixf(&projection[0][0]);

        glMatrixMode(GL_MODELVIEW);                             // 设置视图矩阵
        glLoadMatrixf(&view[0][0]);

		DrawAllStatic(); // 绘制静态物体
		DrawAllAnimate(); // 绘制动画物体

        // 交换缓冲区，处理事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}