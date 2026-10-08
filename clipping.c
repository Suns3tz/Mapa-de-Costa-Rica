#include "clipping.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    BORDE_DERECHO,
    BORDE_IZQUIERDO,
    BORDE_SUPERIOR,
    BORDE_INFERIOR
} Borde;

// Decide si un punto esta dentro del semiplano del borde actual
static int esta_adentro(Point punto, Borde borde, double limite) {
    switch (borde) {
        case BORDE_DERECHO:
            return punto.x <= limite;

        case BORDE_IZQUIERDO:
            return punto.x >= limite;

        case BORDE_SUPERIOR:
            return punto.y <= limite;

        case BORDE_INFERIOR:
            return punto.y >= limite;
    }

    return 0;
}

// Calcula la interseccion del segmento origen-destino con la recta que contiene el borde
// Se llama solo cuando un extremo está adentro y el otro afuera
static Point interseccion(Point origen, Point destino, Borde borde, double limite) {
    Point resultado;
    double t;

    if (borde == BORDE_DERECHO || borde == BORDE_IZQUIERDO) {

        t = (limite - origen.x) / (destino.x - origen.x);

        resultado.x = limite;

        resultado.y = origen.y + t * (destino.y - origen.y);
    } else {
        t = (limite - origen.y) / (destino.y - origen.y);

        resultado.x = origen.x + t * (destino.x - origen.x);

        resultado.y = limite;
    }

    resultado.w = origen.w + t * (destino.w - origen.w);

    return resultado;
}

// Evita vertices consecutivos exactamente iguales
static void agregar_punto(Polygon *poligono, Point punto) {
    if (poligono->Q_point > 0) {
        Point ultimo = poligono->points[poligono->Q_point - 1];

        if (ultimo.x == punto.x && ultimo.y == punto.y) {
            return;
        }
    }
    poligono->points[poligono->Q_point] = punto;
    poligono->Q_point++;
}

// Procesa un solo borde
static int recortar_borde(const Polygon *entrada, Polygon *salida,
                            Borde borde, double limite) {
    salida->points = NULL;
    salida->Q_point = 0;

    if (entrada->Q_point == 0) {
        return 1;
    }

    // Cada segmento puede agregar como máximo dos puntos: interseccion y destino
    size_t cantidad = (size_t)entrada->Q_point;

    if (cantidad > (size_t)INT_MAX / 2 || cantidad > SIZE_MAX / sizeof(Point) / 2) {
        return 0;
    }

    salida->points = malloc(2 * cantidad * sizeof(Point));

    if (!salida->points) {
        return 0;
    }

    // Empezamos con el ultimo vertice para procesar tambien el segmento ultimo -> primero
    Point origen = entrada->points[entrada->Q_point - 1];

    int origen_adentro = esta_adentro(origen, borde, limite);

    for (int i = 0; i < entrada->Q_point; i++) {
        Point destino = entrada->points[i];

        int destino_adentro = esta_adentro(destino, borde, limite);

        if (origen_adentro && destino_adentro) {
            // Adentro -> adentro
            agregar_punto(salida, destino);

        } else if (origen_adentro && !destino_adentro) {
            // Adentro -> afuera
            agregar_punto(salida, interseccion(origen, destino, borde, limite));

        } else if (!origen_adentro && destino_adentro) {
            // Afuera -> adentro
            agregar_punto(salida, interseccion(origen, destino, borde, limite));

            agregar_punto(salida, destino);
        }
        // Para Afuera -> afuera no agregamos nada
        origen = destino;
        origen_adentro = destino_adentro;
    }

    if (salida->Q_point > 1) {
        Point primero = salida->points[0];

        Point ultimo = salida->points[salida->Q_point - 1];

        if (primero.x == ultimo.x && primero.y == ultimo.y) {
            salida->Q_point--;
        }
    }

    if (salida->Q_point == 0) {
        liberar_poligono_clipping(salida);
    }

    return 1;
}

int clipping_poligono(const Polygon *entrada, Polygon *salida, double x_min,
                        double x_max, double y_min, double y_max) {
    if (!entrada || !salida || entrada == salida) {
        return 0;
    }

    // Evitar sobrescribir un resultado que todavia tiene memoria asignada
    if (salida->points != NULL || salida->Q_point != 0) {
        return 0;
    }

    if (!(x_min < x_max) || !(y_min < y_max) ||
        entrada->Q_point < 0 ||
        (entrada->Q_point > 0 && !entrada->points)) {
        return 0;
    }

    // Menos de tres vertices no define un área  que podamos rellenar
    if (entrada->Q_point < 3) {
        return 1;
    }

    const Borde bordes[4] = {BORDE_DERECHO, BORDE_IZQUIERDO, BORDE_SUPERIOR, BORDE_INFERIOR};

    const double limites[4] = {x_max, x_min, y_max, y_min};

    const Polygon *actual = entrada;

    Polygon temporal = {NULL, 0};

    for (int i = 0; i < 4; i++) {
        Polygon siguiente = {NULL, 0};

        if (!recortar_borde(actual, &siguiente, bordes[i], limites[i])) {
            liberar_poligono_clipping(&temporal);

            return 0;
        }

        // El resultado anterior ya no hace falta
        // Nunca liberamos el poligono original
        liberar_poligono_clipping(&temporal);

        temporal = siguiente;
        actual = &temporal;

        if (temporal.Q_point == 0) {
            break;
        }
    }

    if (temporal.Q_point < 3) {
        liberar_poligono_clipping(&temporal);
    }

    *salida = temporal;

    return 1;
}

void liberar_poligono_clipping(Polygon *poligono) {
    if (!poligono) {
        return;
    }

    free(poligono->points);

    poligono->points = NULL;
    poligono->Q_point = 0;
}