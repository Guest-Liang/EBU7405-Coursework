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

bool isDragging = false;  // �Ƿ�������ק

float CusScale = 1.0f;
float radius = 900.0f * CusScale;  // �����ӵ�İ뾶
float CubeSize = 100.0f * CusScale;
float GroundSize = 500.0f * CusScale;

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

    float camX = radius * cos(theta[1] * M_PI / 180.0f) * cos(theta[0] * M_PI / 180.0f);
    float camY = radius * sin(theta[0] * M_PI / 180.0f);
    float camZ = radius * sin(theta[1] * M_PI / 180.0f) * cos(theta[0] * M_PI / 180.0f);
    gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    
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
        int deltaX = x - lastX;
        int deltaY = y - lastY;

        theta[0] += deltaY * 0.1f;
        theta[1] += deltaX * 0.1f;

        if (theta[0] > 89.0f) theta[0] = 89.0f;
        if (theta[0] < -89.0f) theta[0] = -89.0f;
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

// �����¼� Keyboard event
void keyboardFunc(unsigned char key, int x, int y) {
    if (key == 27) {  //  `ESC` �˳�
        exit(0);
    }

    // ʹ�ü�ͷ������������ Use the arrow keys to control the zoom
    if (key == '+') {  // +����С�Ӿ� Plus: zoom in
        radius *= 0.9f;
    }
    if (key == '-') {  // -���Ŵ��Ӿ� Minus: zoom out
        radius *= 1.1f;
    }
    // ����radius�ķ�Χ Limit the range of radius
    if (radius < 100.0f) radius = 100.0f;
    if (radius > 2000.0f) radius = 2000.0f;

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
