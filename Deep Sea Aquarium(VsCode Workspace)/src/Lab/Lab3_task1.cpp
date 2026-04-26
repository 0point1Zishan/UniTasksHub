#include<windows.h>
#include <GL/glut.h>
#include<bits/stdc++.h>
using namespace std;

int x1, yy1, x2, y2;
void init(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 50, 0, 50);
}
void drawShapes(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(10.0);
	
    double dx = x2 - x1;
    double dy = y2 - yy1;

    double steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
    if(steps == 0){
        double x = x1, y = yy1;
        glBegin(GL_POINTS);
            glVertex2d(x, y);
        glEnd();
        glFlush();
        return;
    }

    double x_inc = dx / steps;
    double y_inc = dy / steps;

    double x = x1, y = yy1;
    glBegin(GL_POINTS);
    for(int i = 0; i < steps; i++){
        x += x_inc;
        y += y_inc;
        glVertex2d(x, y);
    }
	glEnd();
    glFlush();
}

int main(int argc, char* argv[]){

    cout << "Enter the values of x1, yy1, x2 & y2: ";
    cin >> x1 >> yy1 >> x2 >> y2;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 800);

	glutCreateWindow("Shape Drawing");
	init();
	glutDisplayFunc(drawShapes);
	glutMainLoop();
	return 0;
}