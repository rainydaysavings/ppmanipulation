#include "image.h"

void vertical_reflection (int h, int w, pixel_t *m2d[h])
{
  pixel_t pixel;
  int mid = w / 2;
  for (int j = 0; j < h; ++j)
    {
      for (int k = 0; k < mid; ++k)
        {
          pixel = m2d[j][k];
          m2d[j][k] = m2d[j][w - 1 - k];
          m2d[j][w - 1 - k] = pixel;
        }
    }
}