#include "rotate.h"
#include "geo.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void rotate(Country *cr, float angle)
{
    if (!cr || cr->Q_prov == 0) return;

    // Obtener el centro en coordenadas universales
	double suma_x = 0.0;
    double suma_y = 0.0;
    long total_puntos = 0;
    for (int p = 0; p < cr->Q_prov; p++) {
        for (int g = 0; g < cr->prov[p].Q_poly; g++) {
            Polygon *poly = &cr->prov[p].polygons[g];
            for (int i = 0; i < poly->Q_point; i++) {
                suma_x += poly->points[i].x;
                suma_y += poly->points[i].y;
                total_puntos++;
                
            }
        }
    }

    float cx = (float)(suma_x / total_puntos);
    float cy = (float)(suma_y / total_puntos);

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
