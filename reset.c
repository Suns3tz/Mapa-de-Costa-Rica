#include "reset.h"

void reset(double *x_min, double *x_max, double *y_min, double *y_max,
           double x_min_init, double x_max_init, double y_min_init, double y_max_init)
{
	*x_min = x_min_init;
	*x_max = x_max_init;
	*y_min = y_min_init;
	*y_max = y_max_init;
}
