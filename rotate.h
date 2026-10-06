#ifndef ROTATE_H
#define ROTATE_H

#define ROTATE_NORMAL_C -2.0f
#define ROTATE_RAPIDO_C -5.0f
#define ROTATE_LENTO_C -0.5f

#define ROTATE_NORMAL 2.0f
#define ROTATE_RAPIDO 5.0f
#define ROTATE_LENTO 0.5f

#include "geo.h"

void rotate(Country *cr, float angle, float x_min, float x_max, float y_min, float y_max);

#endif
