//Reference GitHub repo: https://github.com/JoeyDeVries/LearnOpenGL
#include <glad/glad.h>  // ��Ҫ����OpenGL��չ
#include <GLFW/glfw3.h>  // ʹ��GLFW����д��ڹ���
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

bool isDragging;                            // ��¼�Ƿ�������ק
bool isFreeMoveMode;                        // �л������ģʽ
bool firstMouse;                            // �Ƿ��ǵ�һ���ƶ����
bool keys[GLFW_KEY_LAST + 1] = { false };   // ��¼���а�����״̬

int lastX, lastY;                           // ��һ�����λ��
float radius;                               // ��������̶�ģʽ�����ĵľ���
glm::vec3 camPos;                           // �洢�������λ�ã�x, y, z��
glm::vec3 camFront;                         // �������ǰ����
glm::vec3 camUp;                            // ��������Ϸ���
float freeModePitch;                        // freeModePitch ǰ����y�����ķ�����ֵ
float freeModeYaw;                          // freeModeYaw x,zƽ��ķ�����ֵ

float fixedYaw;                             // �̶�ģʽ�µ�ˮƽ��ת�Ƕ�
float fixedPitch;                           // �̶�ģʽ�µĴ�ֱ��ת�Ƕ�

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

    const int width = 20; // �п�

    std::cout << std::fixed << std::setprecision(3); // ����������

    std::cout << std::left << std::setw(width) << "Parameter"
        << std::left << std::setw(width) << "Value" << std::endl; // ��ͷ

    std::cout << std::string(width * 2, '-') << std::endl; // �ָ���

    // ��ӡ������������ֵ
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

    // ��ʽ�� camPos
    std::ostringstream camPosStr;
    camPosStr << std::fixed << std::setprecision(3)
        << "(" << camPos.x << ", " << camPos.y << ", " << camPos.z << ")";
    std::cout << std::left << std::setw(width) << "camPos"
        << std::left << std::setw(width) << camPosStr.str() << std::endl;

    // ��ʽ�� camFront
    std::ostringstream camFrontStr;
    camFrontStr << std::fixed << std::setprecision(3)
        << "(" << camFront.x << ", " << camFront.y << ", " << camFront.z << ")";
    std::cout << std::left << std::setw(width) << "camFront"
        << std::left << std::setw(width) << camFrontStr.str() << std::endl;

    // ��ʽ�� camUp
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

    isDragging = false;                                                 // ��¼�Ƿ�������ק
    isFreeMoveMode = true;                                              // �л������ģʽ
    firstMouse = true;                                                  // �Ƿ��ǵ�һ���ƶ����
    // keys[GLFW_KEY_LAST + 1] = { false };                             // ��¼���а�����״̬

    lastX = 0, lastY = 0;                                               // ��һ�����λ��
    radius = 4.0f;                                                      // ��������̶�ģʽ�����ĵľ���
    camPos = glm::vec3(-2.0f, 1.0f, 2.0f);                              // �洢�������λ�ã�x, y, z��
    camFront = glm::normalize(glm::vec3(1.0f, -0.5f, -1.0f));           // �������ǰ����
    camUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));                // ��������Ϸ���
    freeModePitch = glm::degrees(std::asin(camFront.y));                // freeModePitch ǰ����y�����ķ�����ֵ
    freeModeYaw = glm::degrees(std::atan2(-camFront.x, -camFront.z));   // freeModeYaw x,zƽ��ķ�����ֵ

    fixedYaw = 135.0f;                                                  // �̶�ģʽ�µ�ˮƽ��ת�Ƕ�
    fixedPitch = 18.0f;                                                 // �̶�ģʽ�µĴ�ֱ��ת�Ƕ�

    // ��ʼ����ͼ����Ϊ�̶�ģʽ
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

        float sensitivity = 0.1f; // �ӽ�ת��������
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

            // �����������������������λ��
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
    if (button == GLFW_MOUSE_BUTTON_LEFT)       // �������
    {
        if (action == GLFW_PRESS) {
            isDragging = true;                  // �����������ʼ��ק
        } else if (action == GLFW_RELEASE) {
            isDragging = false;                 // �ɿ������ֹͣ��ק
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

    // ���� M �����л������ƶ�ģʽ�͹̶�ģʽ
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keys[GLFW_KEY_M]) {
        PrintKeyEvent(GLFW_KEY_M, "pressed");
        keys[GLFW_KEY_M] = true; // ��� M ������
        isFreeMoveMode = !isFreeMoveMode; // �л�ģʽ
        std::string timeStr = GetCurrentTimeString();
        std::cout << timeStr << " FreeMoveMode: " << (isFreeMoveMode ? "Enabled" : "Disabled") << std::endl;
        firstMouse = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && keys[GLFW_KEY_M]) {
        PrintKeyEvent(GLFW_KEY_M, "released");
        keys[GLFW_KEY_M] = false; // ��� M ���ͷ�
    }

    // ���� L ������һ�ξ�LogInfo
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !keys[GLFW_KEY_L]) {
        PrintKeyEvent(GLFW_KEY_L, "pressed");
        keys[GLFW_KEY_L] = true; // ��� L ������
        LogInfo();
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && keys[GLFW_KEY_L]) {
        PrintKeyEvent(GLFW_KEY_L, "released");
        keys[GLFW_KEY_L] = false; // ��� L ���ͷ�
    }
    
    // ���� R ������һ�ξ͸�ԭ���в���
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !keys[GLFW_KEY_R]) {
        PrintKeyEvent(GLFW_KEY_R, "pressed");
        keys[GLFW_KEY_R] = true; // ��� R ������
        ResetParams();
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && keys[GLFW_KEY_R]) {
        PrintKeyEvent(GLFW_KEY_R, "released");
        keys[GLFW_KEY_R] = false; // ��� L ���ͷ�
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

	glfwSetScrollCallback(window, ScrollCallback);            // ���ֻص�
	glfwSetCursorPosCallback(window, MouseCallback);          // ����ƶ��ص�
	glfwSetMouseButtonCallback(window, MouseButtonCallback);  // ��갴���ص�

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);                        // ������Ȳ��ԣ������ڵ���ϵ 
                                                    // Enable depth testing for 3D rendering

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);                       // �����������
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view;
        if (isFreeMoveMode) {
            view = glm::lookAt(camPos, camPos + camFront, camUp);
        } else {
            view = fixViewMatrix;
        }

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);    // Set the background color to sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);                // ����ͶӰ����
        glLoadMatrixf(&projection[0][0]);

        glMatrixMode(GL_MODELVIEW);                 // ������ͼ����
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