void circle(GLfloat rx, GLfloat ry, GLfloat cx, GLfloat cy, int stD, int endD)
{
    glBegin(GL_POLYGON);
    glVertex2f(cx, cy);
    for (int i = stD; i <= endD; i++)
    {
        float angle = i * 3.1416 / 180;
        float x = rx * cos(angle);
        float y = ry * sin(angle);
        glVertex2f((x + cx), (y + cy));
    }
    glEnd();
}