#include "rotate.h"
#include "geo.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void rotate(const Country *cr, Country *destino, double angle, double x_min, double x_max, double y_min, double y_max)
{
    if (!cr || !destino) return;

    double cx = (double)(x_min + x_max) / 2.0f;
    double cy = (double)(y_min + y_max) / 2.0f;

    // Convertir grados a radianes
    double rad = angle * (M_PI / 180.0f);
    double c = cosf(rad);
    double s = sinf(rad);

    // Traslación implícita en coordenadas universales
    double tx = (cx * (1.0f - c)) + (cy * s);
    double ty = (cy * (1.0f - c)) - (cx * s);

    // Transformar los puntos del mapa
    for (int p = 0; p < cr->Q_prov; p++) {
        for (int g = 0; g < cr->prov[p].Q_poly; g++) {
			const Polygon *poly_orig = &cr->prov[p].polygons[g];
            Polygon *poly_dest = &destino->prov[p].polygons[g];
            for (int i = 0; i < poly_orig->Q_point; i++) {
                double x = poly_orig->points[i].x;
                double y = poly_orig->points[i].y;

                poly_dest->points[i].x = x * c - y * s + tx;
                poly_dest->points[i].y = x * s + y * c + ty;
            }
        }
    }
}
