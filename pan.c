#include "pan.h"

void pan(float *x_min, float *x_max, float *y_min, float *y_max, float dx, float dy)
{
	*x_min += dx;
	*x_max += dx;
	*y_min += dy;
	*y_max += dy;
}
