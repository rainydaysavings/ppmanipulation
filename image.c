#include "image.h"

void vertical_reflection (struct IMAGE *image)
{
  pixel_t pixel;
  int mid = image->width / 2;
  for (int j = 0; j < image->height; ++j)
    {
      for (int k = 0; k < mid; ++k)
        {
          pixel = image->matrix2d[j][k];
          image->matrix2d[j][k] = image->matrix2d[j][image->width - 1 - k];
          image->matrix2d[j][image->width - 1 - k] = pixel;
        }
    }
}