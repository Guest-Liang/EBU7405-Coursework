#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
constexpr double M_PI = 3.14159265358979323846;

void drawCircle(GLfloat x, GLfloat y, GLfloat radius, GLint numberOfSides, bool filled) {
    GLint numberOfVertices = numberOfSides + 2;
    GLfloat doublePi = 2.0f * M_PI;

    std::vector<GLfloat> circleVerticesX(numberOfVertices);
    std::vector<GLfloat> circleVerticesY(numberOfVertices);
    std::vector<GLfloat> circleVerticesZ(numberOfVertices);

    circleVerticesX[0] = x;
    circleVerticesY[0] = y;
    circleVerticesZ[0] = 0;

    for (int i = 1; i < numberOfVertices; i++) {
        circleVerticesX[i] = x + (radius * cos(i * doublePi / numberOfSides));
        circleVerticesY[i] = y + (radius * sin(i * doublePi / numberOfSides));
        circleVerticesZ[i] = 0;
    }

    std::vector<GLfloat> allCircleVertices(numberOfVertices * 3);

    for (int i = 0; i < numberOfVertices; i++) {
        allCircleVertices[i * 3] = circleVerticesX[i];
        allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
        allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, allCircleVertices.data());
    if (filled) {
        glDrawArrays(GL_TRIANGLE_FAN, 0, numberOfVertices);
    }
    else {
        glDrawArrays(GL_LINE_LOOP, 1, numberOfVertices - 1);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
}

void draw4SquareGrid(GLfloat x, GLfloat y, GLfloat halfLength) {
    GLfloat points[9][2] = { {x - halfLength, y + halfLength}, {x, y + halfLength}, {x + halfLength, y + halfLength},
                             {x - halfLength, y}, {x, y}, {x + halfLength, y},
                             {x - halfLength, y - halfLength}, {x, y - halfLength}, {x + halfLength, y - halfLength} };
    glBegin(GL_LINES);

    glVertex2fv(points[0]);
    glVertex2fv(points[2]);
    glVertex2fv(points[3]);
    glVertex2fv(points[5]);
    glVertex2fv(points[6]);
    glVertex2fv(points[8]);

    glVertex2fv(points[0]);
    glVertex2fv(points[6]);
    glVertex2fv(points[1]);
    glVertex2fv(points[7]);
    glVertex2fv(points[2]);
    glVertex2fv(points[8]);
    glEnd();
}

void drawRailings(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, GLfloat height, GLint railingsnum) {
    if (startY != endY) {
        std::cerr << "Error: The railing is not horizontal. The y-coordinates of the start and end points must be the same." << std::endl;
        return;
    }

    GLfloat halfRailingHeight = height / 2.0f;
    GLfloat railingWidth = (endX - startX) / railingsnum;

    glBegin(GL_LINES);
    for (int i = 0; i <= railingsnum; ++i) {
        GLfloat x = startX + i * railingWidth;
        glVertex2f(x, startY - halfRailingHeight);
        glVertex2f(x, startY + halfRailingHeight);
    }
    glVertex2f(startX, startY);
    glVertex2f(endX, endY);
    glEnd();
}

void display() {
    glClearColor(0.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.0, 0.0); // Black
    GLfloat house_chimney[4][2] = { {0.2,0.7}, {0.4,0.7}, {0.4,0.21}, {0.2,0.21} };
    glBegin(GL_POLYGON);
    glVertex2fv(house_chimney[0]);
    glVertex2fv(house_chimney[1]);
    glVertex2fv(house_chimney[2]);
    glVertex2fv(house_chimney[3]);
    glEnd();

    glColor3f(1.0, 0.0, 0.0); // Red
    GLfloat house_roof[3][2] = { {0.0,0.8}, {0.5,0.2}, {-0.5,0.2} };
    glBegin(GL_TRIANGLES);
    glVertex2fv(house_roof[0]);
    glVertex2fv(house_roof[1]);
    glVertex2fv(house_roof[2]);
    glEnd();

    glColor3f(1.0, 1.0, 1.0); // White
    GLfloat house_body[4][2] = { {0.5,0.2}, {0.5,-0.7}, {-0.5,-0.7}, {-0.5,0.2} };
    glBegin(GL_POLYGON);
    glVertex2fv(house_body[0]);
    glVertex2fv(house_body[1]);
    glVertex2fv(house_body[2]);
    glVertex2fv(house_body[3]);
    glEnd();

    glColor3f(0.0, 0.0, 0.0); // Black
    glLineWidth(2.0f);
    GLfloat chimney_line[2][2] = { {0.45,0.7}, {0.7,0.9} };
    glBegin(GL_LINES);
    glVertex2fv(chimney_line[0]);
    glVertex2fv(chimney_line[1]);
    glEnd();

    drawCircle(0.4, 0.7, 0.05, 100, false);
    drawCircle(0.5, 0.8, 0.06, 100, false);
    drawCircle(0.6, 0.9, 0.07, 100, false);

    draw4SquareGrid(-0.25, -0.125, 0.125);

    GLfloat door[4][2] = { {0.125,-0.7}, {0.125,0.0}, {0.375,0.0}, {0.375,-0.7} };
    glBegin(GL_LINES);
    glVertex2fv(door[0]);
    glVertex2fv(door[1]);
    glVertex2fv(door[1]);
    glVertex2fv(door[2]);
    glVertex2fv(door[2]);
    glVertex2fv(door[3]);
    glEnd();

    GLfloat doorknob[4][2] = { {0.15,-0.35}, {0.15,-0.4}, {0.2,-0.4}, {0.2,-0.35} };
    glBegin(GL_POLYGON);
    glVertex2fv(doorknob[0]);
    glVertex2fv(doorknob[1]);
    glVertex2fv(doorknob[2]);
    glVertex2fv(doorknob[3]);
    glEnd();

    glColor3f(0.66, 0.41, 0.16);
    glLineWidth(5.0f);
    drawRailings(-0.9, -0.7, 0.9, -0.7, 0.3, 20);

    glFlush();
}

void reshape(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (width >= height) {
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    }
    else {
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(300, 300);
    glutCreateWindow("House");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
}