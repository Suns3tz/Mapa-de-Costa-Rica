#include "rotate.h"
#include "geo.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void rotate(Country *cr, float angle, float x_min, float x_max, float y_min, float y_max)
{
    if (!cr || cr->Q_prov == 0) return;

    float cx = (float)(x_min + x_max) / 2.0f;
    float cy = (float)(y_min + y_max) / 2.0f;

    // Convertir grados a radianes
    float rad = angle * (M_PI / 180.0f);
    float c = cosf(rad);
    float s = sinf(rad);

    // Traslación implícita en coordenadas universales
    float tx = (cx * (1.0f - c)) + (cy * s);
    float ty = (cy * (1.0f - c)) - (cx * s);

    // Transformar los puntos del mapa
    for (int p = 0; p < cr->Q_prov; p++) {
        for (int g = 0; g < cr->prov[p].Q_poly; g++) {
            Polygon *poly = &cr->prov[p].polygons[g];
            for (int i = 0; i < poly->Q_point; i++) {
                float x = poly->points[i].x;
                float y = poly->points[i].y;

                poly->points[i].x = x * c - y * s + tx;
                poly->points[i].y = x * s + y * c + ty;
            }
        }
    }
}
