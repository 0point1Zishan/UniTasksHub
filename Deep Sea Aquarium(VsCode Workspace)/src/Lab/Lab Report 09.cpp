#include<windows.h>
#include <GL/glut.h>
#include<bits/stdc++.h>
using namespace std;

float shift = 0;
void init(void){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-20, 100, -20, 100);
}
void drawShapes(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
      glVertex2d(10, 40 + shift);
      glVertex2d(40, 40 + shift);
      glVertex2d(50, 70 + shift);
      glVertex2d(40, 70 + shift);
      glVertex2d(10, 70 + shift);
    glEnd();
    glFlush();
}
void key(int key, int x, int ty){
    switch(key){
      case GLUT_KEY_UP:
        shift += 5;
        glutPostRedisplay();
        break;
      case GLUT_KEY_DOWN:
        shift -= 5;
        glutPostRedisplay();
        break;
    }
}
int main(int argc, char* argv[]){
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(700, 700);
  glutCreateWindow("Shape Drawing");
  init();
  glutDisplayFunc(drawShapes);
  glutSpecialFunc(key);
  glutMainLoop();
  return 0;
}