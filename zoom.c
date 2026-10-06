#include "zoom.h"

#define MIN_ZOOM 0.001f
#define MAX_ZOOM 1000.0f

void zoom(double *x_min, double *x_max, double *y_min, double *y_max, double z)
{
	double xc = (*x_min + *x_max) / 2.0f;
	double yc = (*y_min + *y_max) / 2.0f;

	double xmin = (*x_min - xc) * z + xc;
	double xmax = (*x_max - xc) * z + xc;
	double ymin = (*y_min - yc) * z + yc;
	double ymax = (*y_max - yc) * z + yc;

	double width = xmax - xmin;
	double height = ymax - ymin;

	if (width < MIN_ZOOM || height < MIN_ZOOM) return;
	if (width > MAX_ZOOM || height > MAX_ZOOM) return;

	*x_min = xmin;
	*x_max = xmax;
	*y_min = ymin;
	*y_max = ymax;
}
