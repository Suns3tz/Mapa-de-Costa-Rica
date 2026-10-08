#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
    uint8_t a, r, g, b;
} Pixel;

typedef struct {
    int width;  // TH: ancho de la textura
    int height; // TV: alto de la textura

    unsigned char *Mat;
} MatAVS;

// mat debe comenzar como {0, 0, NULL}
int texture_load(const char *filename, MatAVS *mat);

void texture_free(MatAVS *mat);

// Obtiene el texel correspondiente a: TEXTURA[x % TH][scanline % TV]
int texture_sample(const MatAVS *mat, int x, int scanline, Pixel *pixel);

#endif