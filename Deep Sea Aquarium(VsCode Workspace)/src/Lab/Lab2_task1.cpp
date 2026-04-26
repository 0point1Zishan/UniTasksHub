#include<windows.h>
#include <GL/glut.h>

void init(void){
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 70.0, -10.0, 90.0);
}

void drawShapes(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(10.0);
	glColor3f(0.749, 0.439, 0.224);

	// Bottom quad
	glBegin(GL_QUADS);
		glVertex2i(26, 0);
		glVertex2i(34, 0);
		glVertex2i(40, 13);
		glVertex2i(20, 13);

		// Middle Quad
		glColor3f(0.451, 0.922, 0.502);
		glVertex2d(29.5, 13);
		glVertex2d(29.5, 40);
		glVertex2d(30.5, 40);
		glVertex2d(30.5, 13);

		// Middle's 2 quads
		glColor3f(0.02, 0.89, 0.969);
		glVertex2i(18, 31);
		glVertex2i(19, 29);
		glVertex2i(25, 25);
		glVertex2i(28, 25);
	
		glVertex2i(18, 32);
		glVertex2i(21, 32);
		glVertex2i(28, 28);
		glVertex2i(29, 25);
	glEnd();

	// Top 4 polygon
	glColor3f(1, 0.325, 0.682);
	glBegin(GL_POLYGON);
		glVertex2i(28, 40);
		glVertex2i(32, 40);
		glVertex2i(34, 42);
		glVertex2i(34, 46);
		glVertex2i(32, 48);
		glVertex2i(28, 48);
		glVertex2i(26, 46);
		glVertex2i(26, 42);
	glEnd();

	int x = 7, y = 7;
	glBegin(GL_POLYGON);
		glVertex2i(28 + x, 40 + y);
		glVertex2i(32 + x, 40 + y);
		glVertex2i(34 + x, 42 + y);
		glVertex2i(34 + x, 46 + y);
		glVertex2i(32 + x, 48 + y);
		glVertex2i(28 + x, 48 + y);
		glVertex2i(26 + x, 46 + y);
		glVertex2i(26 + x, 42 + y);
	glEnd();

	x = 0, y = 13;
	glBegin(GL_POLYGON);
		glVertex2i(28 + x, 40 + y);
		glVertex2i(32 + x, 40 + y);
		glVertex2i(34 + x, 42 + y);
		glVertex2i(34 + x, 46 + y);
		glVertex2i(32 + x, 48 + y);
		glVertex2i(28 + x, 48 + y);
		glVertex2i(26 + x, 46 + y);
		glVertex2i(26 + x, 42 + y);
	glEnd();

	x = -7, y = 7;
	glBegin(GL_POLYGON);
		glVertex2i(28 + x, 40 + y);
		glVertex2i(32 + x, 40 + y);
		glVertex2i(34 + x, 42 + y);
		glVertex2i(34 + x, 46 + y);
		glVertex2i(32 + x, 48 + y);
		glVertex2i(28 + x, 48 + y);
		glVertex2i(26 + x, 46 + y);
		glVertex2i(26 + x, 42 + y);
	glEnd();
	glFlush();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(700, 800);

	glutCreateWindow("Shape Drawing");
	init();
	glutDisplayFunc(drawShapes);
	glutMainLoop();
	return 0;
}