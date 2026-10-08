#include "filling.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    int fila_inicio;
    int fila_fin;
    // Interseccion con la fila actual
    double x;
    // Delta x / Delta y
    double inversa_pendiente;
} Borde;

// Ordena los bordes por la fila en que se activan, de arriba hacia abajo
static int comparar_inicio(const void *a, const void *b) {
    const Borde *borde_a = a;
    const Borde *borde_b = b;

    if (borde_a->fila_inicio > borde_b->fila_inicio) {
        return -1;
    }

    if (borde_a->fila_inicio < borde_b->fila_inicio) {
        return 1;
    }

    return 0;
}

// Ordena los bordes activos por su interseccion x
static int comparar_x(const void *a, const void *b) {
    const Borde *borde_a = a;
    const Borde *borde_b = b;

    if (borde_a->x < borde_b->x) {
        return -1;
    }

    if (borde_a->x > borde_b->x) {
        return 1;
    }

    return 0;
}

int filling_poligono(const Polygon *poligono, int ancho, int alto,
                        PintarPixel pintar, void *contexto) {
    if (!poligono || !pintar || ancho <= 0 || alto <= 0 || poligono->Q_point < 0) {
        return 0;
    }

    if (poligono->Q_point < 3) {
        return 1;
    }

    if (!poligono->points) {
        return 0;
    }

    // Esta funcion recibe puntos del framebuffer
    for (int i = 0; i < poligono->Q_point; i++) {
        Point punto = poligono->points[i];

        if (!isfinite(punto.x) || !isfinite(punto.y) || punto.x < 0.0 ||
            punto.x > (double)ancho || punto.y < 0.0 || punto.y > (double)alto) {
            return 0;
        }
    }

    size_t capacidad = (size_t)poligono->Q_point;

    if (capacidad > SIZE_MAX / sizeof(Borde)) {
        return 0;
    }

    Borde *bordes = malloc(capacidad * sizeof(Borde));

    Borde *activos = malloc(capacidad * sizeof(Borde));

    if (!bordes || !activos) {
        free(bordes);
        free(activos);
        return 0;
    }

    int cantidad_bordes = 0;
    int ultima_fila = alto - 1;

    // Construccion de la lista de bordes
    for (int i = 0; i < poligono->Q_point; i++) {
        int siguiente =
            (i + 1) % poligono->Q_point;

        Point a = poligono->points[i];
        Point b = poligono->points[siguiente];

        // Ignorar bordes horizontales
        if (a.y == b.y) {
            continue;
        }

        Point inferior;
        Point superior;

        if (a.y < b.y) {
            inferior = a;
            superior = b;
        } else {
            inferior = b;
            superior = a;
        }

        // Filas que cumplen inferior.y < fila <= superior.y
        int fila_inicio = (int)floor(superior.y);

        int fila_fin = (int)floor(inferior.y) + 1;

        // Mantener las filas dentro del framebuffer
        if (fila_inicio >= alto) {
            fila_inicio = alto - 1;
        }

        if (fila_inicio < fila_fin) {
            continue;
        }

        double inversa_pendiente = (superior.x - inferior.x) / (superior.y - inferior.y);

        // La primera fila puede estar por debajo del vertice superior si es fraccionario 
        double x_inicial = superior.x + ((double)fila_inicio - superior.y) * inversa_pendiente;

        bordes[cantidad_bordes].fila_inicio = fila_inicio;
        bordes[cantidad_bordes].fila_fin = fila_fin;
        bordes[cantidad_bordes].x = x_inicial;
        bordes[cantidad_bordes].inversa_pendiente = inversa_pendiente;

        cantidad_bordes++;

        if (fila_fin < ultima_fila) {
            ultima_fila = fila_fin;
        }
    }

    if (cantidad_bordes == 0) {
        free(bordes);
        free(activos);
        return 1;
    }

    qsort(bordes, (size_t)cantidad_bordes, sizeof(Borde), comparar_inicio);

    int siguiente_borde = 0;
    int cantidad_activos = 0;

    int scanline = bordes[0].fila_inicio;

    while (scanline >= ultima_fila) {
        // Activar bordes que comienzan en esta fila
        while (siguiente_borde < cantidad_bordes && bordes[siguiente_borde].fila_inicio == scanline) {
            
            activos[cantidad_activos] = bordes[siguiente_borde];

            cantidad_activos++;
            siguiente_borde++;
        }

        // Ordenar intersecciones
        qsort(activos, (size_t)cantidad_activos, sizeof(Borde), comparar_x);

        // Pintar por parejas
        for (int i = 0; i + 1 < cantidad_activos; i += 2) {
            double izquierda = activos[i].x;
            double derecha = activos[i + 1].x;

            // Protección contra pequeños errores numéricos
            if (izquierda < 0.0) {
                izquierda = 0.0;
            }

            if (derecha > (double)(ancho - 1)) {
                derecha = (double)(ancho - 1);
            }

            if (izquierda > derecha) {
                continue;
            }

            int desde = (int)ceil(izquierda);
            int hasta = (int)floor(derecha);

            for (int columna = desde; columna <= hasta; columna++) {
                pintar(columna, scanline, contexto);
            }
        }

        // Actualizar las intersecciones
        // Desactivar los bordes que terminan
        int restantes = 0;

        for (int i = 0; i < cantidad_activos; i++) {
            activos[i].x -= activos[i].inversa_pendiente;

            if (scanline > activos[i].fila_fin) {
                activos[restantes] = activos[i];
                restantes++;
            }
        }

        cantidad_activos = restantes;

        // Bajar una fila
        scanline--;
    }

    free(bordes);
    free(activos);

    return 1;
}