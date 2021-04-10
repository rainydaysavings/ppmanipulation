#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PIXEL {
    unsigned char r, g, b;
} pixel_t;

typedef struct IMAGE {
    int height;
    int width;
    int limit_value;
    long file_size;
    struct PIXEL **matrix2d;
} image_t;

#ifndef PPMANIPULATION_IMAGE_H
void vertical_reflection (struct IMAGE *image);
#define PPMANIPULATION_IMAGE_H

#endif //PPMANIPULATION_IMAGE_H