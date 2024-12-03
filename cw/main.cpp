//Reference GitHub repo: https://github.com/JoeyDeVries/LearnOpenGL
#include <glad/glad.h>  // 需要加载OpenGL扩展
#include <GLFW/glfw3.h>  // 使用GLFW库进行窗口管理
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

// 角度变量，用于旋转视角
GLfloat theta[] = { 18.0, 36.0, 0.0 };  // X, Y, Z 轴的旋转角度
int lastX = 0, lastY = 0;  // 上一次鼠标位置
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

bool isDragging = false; // 记录是否正在拖拽
bool isFreeMoveMode = true;  // 切换摄像机模式，默认为自由移动模式
bool firstMouse = true;  // 记录是否是第一次移动鼠标
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float CusScale = 1.0f;
float radius = 900.0f * CusScale;  // 距离视点的半径
float CubeSize = 100.0f * CusScale;
float GroundSize = 500.0f * CusScale;
float movementSpeed = 10.0f; // 移动速度
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f); // 存储摄像机的位置（x, y, z）
glm::vec3 camFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)); // 摄像机的前方向
glm::vec3 camUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)); // 摄像机的上方向

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
            // 自由移动模式：鼠标拖动改变摄像机的朝向
            theta[1] += xoffset;  // 改变水平旋转角度
            theta[0] -= yoffset;  // 改变垂直旋转角度
        }
        else {
            // 旋转模式：改变视角
            theta[0] += yoffset;
            theta[1] += xoffset;
        }

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
        lastX = SCR_WIDTH/2;
		lastY = SCR_HEIGHT/2;
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
    

    // 检查其他按键（遍历所有常见的按键）
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS) {
            const char* keyName = glfwGetKeyName(key, 0);  // 获取按键名称
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
    glfwSwapInterval(0);  // 禁用 V-Sync

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // 设置主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  // 设置次版本号
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 使用核心模式

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

    // ---------------------------创建着色器
    // build and compile our shader program
    Shader GLShader("GLvs.vs", "GLfs.fs");
    float vertices[] = {
        // 后面 (白色)
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // 左下
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // 右下
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // 右上
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // 右上
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // 左上
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  // 左下

        // 前面 (黄色)
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // 左下
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // 右下
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // 右上
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // 右上
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // 左上
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // 左下

        // 左面 (红色)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // 后左
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // 前左
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // 前左
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // 前左
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // 后左
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // 后左

        // 右面 (灰色)
         0.5f, -0.5f, -0.5f,  0.2f, 0.2f, 0.2f,  // 后右
         0.5f,  0.5f, -0.5f,  0.2f, 0.2f, 0.2f,  // 前右
         0.5f,  0.5f,  0.5f,  0.2f, 0.2f, 0.2f,  // 前右
         0.5f,  0.5f,  0.5f,  0.2f, 0.2f, 0.2f,  // 前右
         0.5f, -0.5f,  0.5f,  0.2f, 0.2f, 0.2f,  // 后右
         0.5f, -0.5f, -0.5f,  0.2f, 0.2f, 0.2f,  // 后右

         // 上面 (绿色)
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // 左前
          0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // 右前
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // 右后
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // 右后
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // 左后
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // 左前

         // 下面 (蓝色)
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // 左前
          0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // 右前
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  // 右后
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  // 右后
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  // 左后
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // 左前
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);  // 位置
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));  // 颜色
    glEnableVertexAttribArray(1);
    glBindVertexArray(VAO);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Set the background color to sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLShader.use();


        //glm::vec3 lightPos(1.0f, 1.0f, 1.0f);  // 设置光源的位置
        //GLShader.setVec3("lightPos", lightPos);  // 传递光源位置给着色器
        GLShader.setVec3("ambientColor", glm::vec3(20.0f, 20.0f, 20.0f));  // 设置环境光
        GLShader.setVec3("diffuseColor", glm::vec3(0.8f, 0.8f, 0.8f));  // 设置漫反射光
        GLShader.setVec3("specularColor", glm::vec3(1.0f, 1.0f, 1.0f));  // 设置镜面反射光
        GLShader.setFloat("shininess", 50.0f);  // 设置镜面反射的光泽度
        // 设置投影矩阵
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        GLShader.setMat4("projection", projection);
        // 设置视图矩阵
        glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);
        GLShader.setMat4("view", view);
		// 设置模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        GLShader.setMat4("model", model);

        glBindVertexArray(VAO);  // 绑定VAO
        glDrawArrays(GL_TRIANGLES, 0, 36);  // 绘制立方体

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