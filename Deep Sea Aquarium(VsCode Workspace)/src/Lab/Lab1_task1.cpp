#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>

void init(void){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-400.0, 400.0, -400.0, 400.0);
}

void drawPrimitives(void){
    glClear(GL_COLOR_BUFFER_BIT);   
    
    // Top-Right
    glColor3d(0.722, 0.027, 0.027);
    glBegin(GL_POLYGON);
        glVertex2d(0, 0);
        glVertex2d(150, 0);
        glVertex2d(130, 20);
        glVertex2d(150, 60);
        glVertex2d(105, 60);
        glVertex2d(100, 90);
        glVertex2d(65, 60);
        glVertex2d(50, 140);
        glVertex2d(40, 110);
        glVertex2d(0, 200);
    glEnd();

    // Top-Left
    glBegin(GL_POLYGON);
        glVertex2d(0, 0);
        glVertex2d(-150, 0);
        glVertex2d(-130, 20);
        glVertex2d(-150, 60);
        glVertex2d(-105, 60);
        glVertex2d(-100, 90);
        glVertex2d(-65, 60);
        glVertex2d(-50, 140);
        glVertex2d(-40, 110);
        glVertex2d(0, 200);
    glEnd();

    // Bottom-Right
    glColor3d(0.988, 0.071, 0.071);
    glBegin(GL_POLYGON);
        glVertex2d(0, 0);
        glVertex2d(150, 0);
        glVertex2d(60, -60);
        glVertex2d(90, -180);
        glVertex2d(0, -80);
    glEnd();

    // Bottom-Left
    glBegin(GL_POLYGON);
        glVertex2d(0, 0);
        glVertex2d(-150, 0);
        glVertex2d(-60, -60);
        glVertex2d(-90, -180);
        glVertex2d(0, -80);
    glEnd();
    glFlush();
}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    glutInitWindowPosition(100, 100);       
    glutInitWindowSize(800, 800);          
    glutCreateWindow("Centered Maple Leaf"); 

    init();                          
    glutDisplayFunc(drawPrimitives); 
    glutMainLoop();                 
    return 0;
}