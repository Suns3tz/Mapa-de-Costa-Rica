#include "zoom.h"

#define MIN_ZOOM 0.001f
#define MAX_ZOOM 1000.0f

void zoom(float *x_min, float *x_max, float *y_min, float *y_max, float z)
{
	float xc = (*x_min + *x_max) / 2.0f;
	float yc = (*y_min + *y_max) / 2.0f;

	float xmin = (*x_min - xc) * z + xc;
	float xmax = (*x_max - xc) * z + xc;
	float ymin = (*y_min - yc) * z + yc;
	float ymax = (*y_max - yc) * z + yc;

	float width = xmax - xmin;
	float height = ymax - ymin;

	if (width < MIN_ZOOM || height < MIN_ZOOM) return;
	if (width > MAX_ZOOM || height > MAX_ZOOM) return;

	*x_min = xmin;
	*x_max = xmax;
	*y_min = ymin;
	*y_max = ymax;
}
