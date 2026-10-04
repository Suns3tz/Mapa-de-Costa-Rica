#include "reset.h"

void reset(float *x_min, float *x_max, float *y_min, float *y_max,
           float x_min_init, float x_max_init, float y_min_init, float y_max_init)
{
	*x_min = x_min_init;
	*x_max = x_max_init;
	*y_min = y_min_init;
	*y_max = y_max_init;
}
