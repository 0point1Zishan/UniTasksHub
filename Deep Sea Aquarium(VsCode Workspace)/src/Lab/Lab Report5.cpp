#include<windows.h>
#include <GL/glut.h>
#include<bits/stdc++.h>
using namespace std;
int xc, yc, r;
void init(void){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-20, 80, -20, 60);
}
void drawShapes(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(10.0);
    int x = 0,  y = r, pk = 1 - r;

    glBegin(GL_POINTS);
        glVertex2i(xc + y, yc + x);
        glVertex2i(xc + x, yc + y);
        glVertex2i(xc - x, yc + y);
        glVertex2i(xc - y, yc + x);
        glVertex2i(xc - y, yc - x);
        glVertex2i(xc - x, yc - y);
        glVertex2i(xc + x, yc - y);
        glVertex2i(xc + y, yc - x);
        while(x < y){
            x = x + 1;
            if(pk < 0){
                pk = pk + 2 * x + 1;
            } 
            else{
                y = y - 1;
                pk = pk + 2 * x - 2 * y + 1;
            }
            glVertex2i(xc + y, yc + x);
            glVertex2i(xc + x, yc + y);
            glVertex2i(xc - x, yc + y);
            glVertex2i(xc - y, yc + x);
            glVertex2i(xc - y, yc - x);
            glVertex2i(xc - x, yc - y);
            glVertex2i(xc + x, yc - y);
            glVertex2i(xc + y, yc - x);
        }
    glEnd();
    glFlush();
}
int main(int argc, char* argv[]){
 cout << "Input the center point: ";
 cin >> xc >> yc;
 cout << "Input the radius: ";
 cin >> r;
 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
 glutInitWindowPosition(100, 100);
 glutInitWindowSize(400, 400);
 glutCreateWindow("Shape Drawing");
 init();
 glutDisplayFunc(drawShapes);
 glutMainLoop();
 return 0;
}