#ifndef CLIPPING_H
#define CLIPPING_H

#include "geo.h"

// Recorta un poligono contra una ventana rectangular.
int clipping_poligono(const Polygon *entrada, Polygon *salida, double x_min, double x_max,
                        double y_min, double y_max);

void liberar_poligono_clipping(Polygon *poligono);

#endif