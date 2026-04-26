#include<windows.h>
#include <GL/glut.h>
#include<bits/stdc++.h>
using namespace std;

const int W = 814, H = 479;
void init(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, W, 0, H);
}

void drawShapes(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(10.0);
    

	static const float p[][2] = {
		{405.088, 676.896},
		{447.066, 678.995},
		{481.698, 664.302},
		{477.500, 686.341},
		{511.082, 704.181},
		{472.253, 709.429},
		{470.154, 743.011},
		{442.868, 714.676},
		{397.742, 723.071},
		{426.077, 695.786}

        };
        int i, n = sizeof(p)/sizeof(p[0]);
        glColor3f(0.58, 0.537, 0.247);
        glBegin(GL_POLYGON);
        for(i = 0; i < n; i++)
            glVertex2f(p[i][0], p[i][1]);
        glEnd();

	
    glFlush();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(W, H);

	glutCreateWindow("Shape Drawing");
	init();
	glutDisplayFunc(drawShapes);
	glutMainLoop();
	return 0;
}