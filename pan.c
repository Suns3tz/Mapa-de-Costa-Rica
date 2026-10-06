#include "pan.h"

void pan(double *x_min, double *x_max, double *y_min, double *y_max, double dx, double dy)
{
	*x_min += dx;
	*x_max += dx;
	*y_min += dy;
	*y_max += dy;
}
