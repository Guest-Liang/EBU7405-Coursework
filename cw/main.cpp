//Reference GitHub repo: https://github.com/JoeyDeVries/LearnOpenGL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//End of GitHub repo
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif
constexpr double M_PI = 3.14159265358979323846;

// �Ƕȱ�����������ת�ӽ�
GLfloat theta[] = { 20.0, 128.0, 0.0 };  // X, Y, Z �����ת�Ƕ�
int lastX = 0, lastY = 0;  // ��һ�����λ��

bool isDragging = false; // ��¼�Ƿ�������ק
bool isFreeMoveMode = false;  // �л������ģʽ��Ĭ��Ϊ��תģʽ
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;

float CusScale = 1.0f;
float radius = 900.0f * CusScale;  // �����ӵ�İ뾶
float CubeSize = 100.0f * CusScale;
float GroundSize = 500.0f * CusScale;
float movementSpeed = 10.0f; // �ƶ��ٶ�
float camPosf[] = { 0.0f, 0.0f, radius }; // �洢�������λ�ã�x, y, z��
float camFrontf[] = { 0.0f, 0.0f, -1.0f }; // �������ǰ����
float camUpf[] = { 0.0f, 1.0f, 0.0f }; // ��������Ϸ���
glm::vec3 camPos = glm::vec3(camPosf[0], camPosf[1], camPosf[2]);
glm::vec3 camFront = glm::normalize(glm::vec3(camFrontf[0], camFrontf[1], camFrontf[2]));
glm::vec3 camUp = glm::normalize(glm::vec3(camUpf[0], camUpf[1], camUpf[2]));

void drawGround() {
    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);  // Green color
    glVertex3f(-GroundSize / 2, -1.0f, -GroundSize / 2);  // Bottom-left
    glVertex3f(GroundSize / 2, -1.0f, -GroundSize / 2);   // Bottom-right
    glVertex3f(GroundSize / 2, -1.0f, GroundSize / 2);    // Top-right
    glVertex3f(-GroundSize / 2, -1.0f, GroundSize / 2);   // Top-left
    glEnd();
}

void drawBody() {
    glColor3f(1.0f, 1.0f, 1.0f);  // White color for the cube
    glutSolidCube(CubeSize);  // Draw a cube

    glPushMatrix();  // Save the current transformation matrix

    glColor3f(0.7f, 0.7f, 0.7f);  // Light gray color
    glTranslatef(-CubeSize / 2 / 2, 0.0f, CubeSize / 2 + 0.01 * CubeSize);
    glBegin(GL_QUADS);
    glVertex3f(-CubeSize / 2 / 2 * 0.6, -CubeSize / 2 / 2 * 0.6, 0.0f); //Bottom-left
    glVertex3f(CubeSize / 2 / 2 * 0.6, -CubeSize / 2 / 2 * 0.6, 0.0f);  //Bottom-right
    glVertex3f(CubeSize / 2 / 2 * 0.6, CubeSize / 2 / 2 * 0.6, 0.0f);   //Top-right
    glVertex3f(-CubeSize / 2 / 2 * 0.6, CubeSize / 2 / 2 * 0.6, 0.0f);  //Top-left
    glEnd();

    glTranslatef(CubeSize / 2, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-CubeSize / 2 / 2 * 0.6, -CubeSize / 2, 0.0f);
    glVertex3f(CubeSize / 2 / 2 * 0.6, -CubeSize / 2, 0.0f);
    glVertex3f(CubeSize / 2 / 2 * 0.6, CubeSize / 2 / 2 * 0.6, 0.0f);
    glVertex3f(-CubeSize / 2 / 2 * 0.6, CubeSize / 2 / 2 * 0.6, 0.0f);
    glEnd();

    glPopMatrix();  // Restore the previous transformation matrix
}

void setupCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (isFreeMoveMode) {
        // �����ƶ�ģʽ��ʹ�� camPos ��ȷ���ӵ�
        gluLookAt(camPos.x, camPos.y, camPos.z,
            camPos.x + camFront.x, camPos.y + camFront.y, camPos.z + camFront.z,
            camUp.x, camUp.y, camUp.z);
    }
    else {
        // ��תģʽ������theta���������
        float camX = radius * cos(theta[1] * M_PI / 180.0f) * cos(theta[0] * M_PI / 180.0f);
        float camY = radius * sin(theta[0] * M_PI / 180.0f);
        float camZ = radius * sin(theta[1] * M_PI / 180.0f) * cos(theta[0] * M_PI / 180.0f);
        gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    }
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set the light source properties (white light coming from above)
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };  // Directional light
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // ��ɫɢ���
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // White light specular reflection ���淴���
    GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };   // Ambient light ������

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    // ���ò�����ɫ���Դ��ɫ�Ļ��
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // ���ò��ʷ���ϵ�� Set the material reflectance coefficients
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // ��ɫ��ɢ���
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ǿ���淴���
    GLfloat mat_shininess[] = { 100.0f };  // �߹�ǿ��

    // ��������Ĳ��ʷ��� Set the material reflectance of the object
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);    // ����ɢ�䷴���
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);  // ���þ��淴���
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); // ���ø߹�ǿ��
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the screen and depth buffer

    setupCamera(); // Set the camera view

    drawGround(); // Draw the ground

    // Draw the cube at the center of the scene
    glPushMatrix();  // Save the current transformation matrix
    glTranslatef(0.0f, CubeSize / 2, 0.0f);  // Move the cube above the ground
    drawBody();
    glPopMatrix();  // Restore the previous transformation matrix

    glFlush();  // Ensure all OpenGL commands are executed
    glutSwapBuffers();  // Swap the buffer to display the rendered image

    // �����ǰ�ĽǶ�
    std::cout << "Current angles: ";
    std::cout << "X: " << theta[0] << ", Y: " << theta[1] << ", Z: " << theta[2] << std::endl;
}

void init() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);  // Sky blue background color
    glEnable(GL_DEPTH_TEST);  // Enable depth testing for 3D rendering

    glEnable(GL_MULTISAMPLE);  // Enable anti-aliasing
    glShadeModel(GL_SMOOTH);  // Enable smooth shading

    // Set up the projection matrix for 3D rendering
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 10000.0);  // near=0.1��far=10000������ü� Field of view, aspect ratio, near and far planes
    glMatrixMode(GL_MODELVIEW);
    // Set up lighting
    setupLighting();
}

// ������¼� Mouse click event
void mouseFunc(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
        isDragging = true;  // ��ʼ��ק
    }
    else {
        isDragging = false;  // ֹͣ��ק
    }
}

// ����϶��¼� Mouse drag event
void motionFunc(int x, int y) {
    if (isDragging) {
        float xpos = static_cast<float>(x);
        float ypos = static_cast<float>(y);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = ypos - lastY;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;  // �������ж� sensitivity
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

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camFront = glm::normalize(front);
        glutPostRedisplay();
    }
}

// �����¼� Keyboard event
void keyboardFunc(unsigned char key, int x, int y) {
    if (key == 27) exit(0); //  `ESC` �˳�

    // ʹ�ü�ͷ������������ Use the arrow keys to control the zoom
    if (key == '+') radius *= 0.9f; // +����С�Ӿ� Plus: zoom in
    if (key == '-') radius *= 1.1f; // -���Ŵ��Ӿ� Minus: zoom out

    // ����radius�ķ�Χ Limit the range of radius
    if (radius < 100.0f) radius = 100.0f;
    if (radius > 2000.0f) radius = 2000.0f;

    // �л������ģʽ
    if (key == 'm' || key == 'M') {  // �� 'M' ���л�ģʽ
        isFreeMoveMode = !isFreeMoveMode;
        std::cout << (isFreeMoveMode ? "Switched to Free Move Mode." : "Switched to Rotation Mode.") << std::endl;
    }

    // �л������ƶ�ģʽ�µ���������ƣ�WASD��
    if (isFreeMoveMode) {
        glm::vec3 front = glm::normalize(camFront);
        glm::vec3 right = glm::normalize(glm::cross(front, camUp)); // ������
        glm::vec3 up = glm::normalize(glm::cross(right, front)); // ������

        if (key == 'w' || key == 'W') camPos += movementSpeed * front; // ��ǰ�ƶ����������������ƽ��

        if (key == 's' || key == 'S') camPos -= movementSpeed * front; // ����ƶ�

        if (key == 'a' || key == 'A') camPos -= movementSpeed * right; // ����ƽ�ƣ���������������෽��

        if (key == 'd' || key == 'D') camPos += movementSpeed * right; // ����ƽ�ƣ��������������Ҳ෽��

    }

    glutPostRedisplay();  // ���»��Ƴ��� Redraw the scene
    std::cout << "Key " << key << " was pressed!" << std::endl;
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // Double buffering and depth buffering
    glutInitWindowSize(500, 500);
    glutCreateWindow("3D Scene");

    init();

    glutMouseFunc(mouseFunc);  // ������ص� Mouse click callback
    glutMotionFunc(motionFunc);  // ����϶��ص� Mouse drag callback
    glutKeyboardFunc(keyboardFunc);  // �����¼��ص� Keyboard event callback

    glutDisplayFunc(display);
    glutMainLoop();  // Enter the GLUT main loop

    return 0;
}
