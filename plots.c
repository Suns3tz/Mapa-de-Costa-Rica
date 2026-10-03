#include <GL/gl.h>

#include "plots.h"

void plot(int col, int row)
{
	glBegin(GL_LINES);
    glVertex2f(
        (float)col + 0.5f,
        (float)row + 0.5f
    );
    glEnd();
}
