#include <GL/gl.h>

#include "plots.h"

void plot(int col, int row)
{
	glBegin(GL_LINES);
    glVertex2f(
        (double)col + 0.5f,
        (double)row + 0.5f
    );
    glEnd();
}
