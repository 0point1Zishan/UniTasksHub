#include<windows.h>
#include <GL/glut.h>

void init(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 600, 50, 800);
}

void drawShapes(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(10.0);
	
	glBegin(GL_QUADS);

    for(int i = 0; i < 6; i++){
        int y = i * 150;
        int shift = 0;
		if(i & 1) shift = 50;

        for(int j = 0; j < 6; j++){
            int x = (j * 100) + shift;

            glColor3f(0.0, 0.0, 0.0);
            glVertex2i(x + 50, y);
            glVertex2i(x, y + 50);
            glVertex2i(x, y + 150);
            glVertex2i(x + 50, y + 200);

            glColor3f(1.0, 1.0, 1.0);
            glVertex2i(x + 50, y);
            glVertex2i(x + 50, y + 200);
            glVertex2i(x + 100, y + 150);
            glVertex2i(x + 100, y + 50);
        }
    }
    glEnd();
    glFlush();
}

int main(int argc, char* argv[]){
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