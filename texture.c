#include "texture.h"

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static int leer_uint32_be(FILE *archivo, uint32_t *valor) {
    unsigned char bytes[4];

    if (fread(bytes, 1, 4, archivo) != 4) {
        return 0;
    }

    *valor = ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
                ((uint32_t)bytes[2] << 8)  | (uint32_t)bytes[3];

    return 1;
}

int texture_load(const char *filename, MatAVS *mat) {
    if (!filename || !mat) {
        return -1;
    }

    // No sobrescribir una textura que sigue cargada
    if (mat->Mat != NULL || mat->width != 0 || mat->height != 0) {
        return -1;
    }

    FILE *archivo = fopen(filename, "rb");

    if (!archivo) {
        return -1;
    }

    uint32_t ancho;
    uint32_t alto;

    if (!leer_uint32_be(archivo, &ancho) || !leer_uint32_be(archivo, &alto)) {
        fclose(archivo);
        return -1;
    }

    if (ancho == 0 || alto == 0 || ancho > INT_MAX || alto > INT_MAX) {
        fclose(archivo);
        return -1;
    }

    size_t ancho_size = (size_t)ancho;
    size_t alto_size = (size_t)alto;

    // Comprobar ancho * alto antes de multiplicar
    if (alto_size > SIZE_MAX / ancho_size) {
        fclose(archivo);
        return -1;
    }

    size_t cantidad_pixeles = ancho_size * alto_size;

    // Cada pixel ocupa cuatro bytes: A, R, G, B
    if (cantidad_pixeles > SIZE_MAX / 4) {
        fclose(archivo);
        return -1;
    }

    size_t total = cantidad_pixeles * 4;

    unsigned char *datos = malloc(total);

    if (!datos) {
        fclose(archivo);
        return -1;
    }

    if (fread(datos, 1, total, archivo) != total) {
        free(datos);
        fclose(archivo);
        return -1;
    }

    fclose(archivo);

    mat->width = (int)ancho;
    mat->height = (int)alto;
    mat->Mat = datos;

    return 0;
}

void texture_free(MatAVS *mat) {
    if (!mat) {
        return;
    }

    free(mat->Mat);

    mat->Mat = NULL;
    mat->width = 0;
    mat->height = 0;
}

int texture_sample(const MatAVS *mat, int x, int scanline, Pixel *pixel) {
    if (!mat || !pixel || !mat->Mat || mat->width <= 0 || mat->height <= 0) {
        return 0;
    }

    int columna = x % mat->width;
    int fila = scanline % mat->height;

    // También permite coordenadas negativas
    if (columna < 0) {
        columna += mat->width;
    }

    if (fila < 0) {
        fila += mat->height;
    }

    // Convertir la fila de pantalla a la fila del archivo AVS
    fila = mat->height - 1 - fila;

    // AVS guarda filas consecutivas
    // Cada texel ocupa cuatro bytes: A, R, G, B
    size_t indice = ((size_t)fila * (size_t)mat->width + (size_t)columna) * 4;

    pixel->a = mat->Mat[indice];
    pixel->r = mat->Mat[indice + 1];
    pixel->g = mat->Mat[indice + 2];
    pixel->b = mat->Mat[indice + 3];

    return 1;
}