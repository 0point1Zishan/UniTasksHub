#include<windows.h>
#include <GL/glut.h>
#include<math.h>

void init(void)
{
  glClearColor(1.0, 1.0, 1.0, 1.0);  // Set display window colour to white

  glMatrixMode(GL_PROJECTION);    // Set projection parameters
  gluOrtho2D(-10.0, 65.0, -10.0, 90.0);
}

void circle(GLfloat rx, GLfloat ry, GLfloat cx, GLfloat cy){
    glBegin(GL_POLYGON);
    glVertex2f(cx, cy);
    for(int i = 0; i <= 360; i++){
        float angle = i * 3.1416 / 180;
        float x = rx * cos(angle);
        float y = ry * sin(angle);
        glVertex2f((x + cx), (y + cy));
    }
    glEnd();
} 

void drawShapes(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
        //* Bottom1
        glColor3f(0.859, 0.847, 0.024);
        glVertex2d(5, 0);
        glVertex2d(19, 0);
        glVertex2d(19, 2);
        glVertex2d(5, 2);

        //* Bottom2
        glColor3f(0.671, 0.278, 0.737);
        glVertex2d(7, 2);
        glVertex2d(17, 2);
        glVertex2d(17, 4);
        glVertex2d(7, 4);

        //* Stand
        glColor3f(1, 0.984, 0.02);
        glVertex2d(11, 4);
        glVertex2d(13, 4);
        glVertex2d(13, 70);
        glVertex2d(11, 70);

        //* Flag Bottom
        glColor3f(0.192, 0.106, 0.573);
        glVertex2d(13, 45);
        glVertex2d(45, 45);
        glVertex2d(45, 50);
        glVertex2d(13, 50);

        //* Flag Middle
        glColor3f(0.192, 0.106, 0.573);
        glVertex2d(13, 55);
        glVertex2d(45, 55);
        glVertex2d(45, 60);
        glVertex2d(13, 60);

        //* Flag Top
        glColor3f(0.192, 0.106, 0.573);
        glVertex2d(13, 65);
        glVertex2d(45, 65);
        glVertex2d(45, 70);
        glVertex2d(13, 70);
    glEnd();

    glBegin(GL_TRIANGLES);
        //* Top Triangle
        glColor3f(0.718, 0.11, 0.11);
        glVertex2d(10, 70);
        glVertex2d(14, 70);
        glVertex2d(12, 73);

        //* Flag Triangle
        glColor3f(0.718, 0.11, 0.11);
        glVertex2d(13, 70);
        glVertex2d(31, 58);
        glVertex2d(13, 45);
    glEnd();

    //* Circle
    glColor3f(0.188, 0.247, 0.624);
    circle(4,4,18,57);
    glFlush();
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);            // Initalise GLUT
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);  // Set display mode

  glutInitWindowPosition(100, 100);        // Set window position
  glutInitWindowSize(700, 700);          // Set window size
  glutCreateWindow("An Example OpenGL Program");  // Create display window

  init();              // Execute initialisation procedure
  glutDisplayFunc(drawShapes);    // Send graphics to display window
  glutMainLoop();          // Display everything and wait

  return 0;
}