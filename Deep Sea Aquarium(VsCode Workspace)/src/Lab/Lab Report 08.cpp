#include<windows.h>
#include <GL/glut.h>
#include<bits/stdc++.h>
using namespace std;

void init(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 1000, 0, 1000);
}

float bx = 10, by = 0;
float bx1 = 50, by1 = 50;
void drawShapes(void){
    glClear(GL_COLOR_BUFFER_BIT);

    //! 1st Shape:
    glColor3f(1.0, 1.0, 1.0);
    
    glPushMatrix();
    glTranslatef(bx, 0.0f, 0.0f);

    glBegin(GL_QUADS);
     glVertex2d(10, 10);
     glVertex2d(100, 10);
     glVertex2d(100, 100);
     glVertex2d(10, 100);
    glEnd();

    glPopMatrix();
    bx += 0.05;
    if(bx > 1000){
      bx = 0;
    }
    glutPostRedisplay();

    //! 2nd Shape:
    glColor3f(1.0, 0.0, 1.0);
    glPushMatrix();
    glTranslatef(0.0, by, 0.0f);
    glBegin(GL_QUADS);
     glVertex2d(10, 10);
     glVertex2d(100, 10);
     glVertex2d(100, 100);
     glVertex2d(10, 100);
    glEnd();
    glPopMatrix();
    by += 0.05;
    if(by > 1000){
      by = 0;
    }
    glutPostRedisplay();

    //! 3rd Shape
    glColor3f(0.0, 0.0, 1.0);
    glPushMatrix();
    glTranslatef(bx1, by1, 0.0f);
    glBegin(GL_QUADS);
     glVertex2d(10, 10);
     glVertex2d(100, 10);
     glVertex2d(100, 100);
     glVertex2d(10, 100);
    glEnd();
    glPopMatrix();
    by1 += 0.05;
    bx1 += 0.05;
    if(by1 > 1000 || bx1 > 1000){
      bx1 = 0;
      by1 = 0;
    }
    glutPostRedisplay();

    glFlush();
}

int main(int argc, char* argv[]){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);

	glutCreateWindow("Shape Drawing");
	init();
	glutDisplayFunc(drawShapes);
	glutMainLoop();
	return 0;
}