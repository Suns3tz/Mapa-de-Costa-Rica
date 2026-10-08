#ifndef FILLING_H
#define FILLING_H

#include "geo.h"

// Función que pinta un pixel del framebuffer
typedef void (*PintarPixel)(
    int columna,
    int fila,
    void *contexto
);
// Rellena un poligono expresado en coordenadas del framebuffer
int filling_poligono(const Polygon *poligono, int ancho, int alto,
                        PintarPixel pintar, void *contexto);

#endif