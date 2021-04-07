#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PIXEL {
    unsigned char r, g, b;
} pixel_t;

#ifndef PPMANIPULATION_IMAGE_H
void vertical_reflection (int h, int w, pixel_t *m2d[h]);
#define PPMANIPULATION_IMAGE_H

#endif //PPMANIPULATION_IMAGE_H