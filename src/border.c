#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

typedef struct PIXEL {
    unsigned char r, g, b;
} pixel_t;

typedef struct IMAGE {
    int height;
    int width;
    int n_pixels;
    int limit_value;
    long file_size;
    pixel_t border;
    pixel_t **matrix2d;
} image_t;

void border (image_t *image)
{
  int real_width = (image->width + (2 * image->n_pixels));
  int real_height = (image->height + (2 * image->n_pixels));

  pixel_t **matrix_b = malloc (real_height * sizeof (matrix_b));
  for (int i = 0; i < real_height; ++i)
    {
      matrix_b[i] = malloc (sizeof (struct PIXEL) * real_width);
    }

  // creating a border line
  pixel_t *line_b = malloc (real_width * sizeof (pixel_t));
  for (int i = 0; i < real_width; ++i)
    {
      line_b[i] = image->border;
    }

  // applying border line at the top
  for (int i = 0; i < image->n_pixels; ++i)
    {
      matrix_b[i] = line_b;
    }

  int j, k;
  for (j = 0; j < image->height; ++j)
    {
      // applying border at beginning of the line
      for (int i = 0; i < image->n_pixels; ++i)
        {
          matrix_b[j + image->n_pixels][i] = image->border;
        }

      for (k = 0; k < image->width; ++k)
        {
          matrix_b[j + image->n_pixels][k + image->n_pixels] = image->matrix2d[j][k];
        }

      // applying border at the end of the line
      for (int i = image->width + image->n_pixels; i < real_width; ++i)
        {
          matrix_b[j + image->n_pixels][i] = image->border;
        }
    }

  // applying border line at the bottom
  for (int i = image->n_pixels + image->height; i < real_height; ++i)
    {
      matrix_b[i] = line_b;
    }

  image->matrix2d = matrix_b;
  image->width = real_width;
  image->height = real_height;
}

void write_to_file (image_t *image, char *f2_name)
{
  FILE *fptr;

  // check for any errors
  if ((fptr = fopen (f2_name, "w")) == NULL)
    {
      printf ("Failed to open file\n");
      exit (-1);
    }

  // write header
  fprintf (fptr, "P3\n%d %d\n%d\n", image->width, image->height, image->limit_value);

  // write pixels
  for (int j = 0; j < image->height; ++j)
    {
      for (int k = 0; k < image->width; ++k)
        {
          fprintf (fptr, "%hhu %hhu %hhu\n",
                   image->matrix2d[j][k].r,
                   image->matrix2d[j][k].g,
                   image->matrix2d[j][k].b);
        }
    }

  // free memory
  fclose (fptr);
}

void write_to_stdout (image_t *image)
{
  // write header
  printf ("P3\n%d %d\n%d\n", image->width, image->height, image->limit_value);

  // write pixels
  for (int j = 0; j < image->height; ++j)
    {
      for (int k = 0; k < image->width; ++k)
        {
          printf ("%hhu %hhu %hhu\n",
                  image->matrix2d[j][k].r,
                  image->matrix2d[j][k].g,
                  image->matrix2d[j][k].b);
        }
    }
}

void write_to_matrix (char *buffer, char *tok, image_t *image)
{
  int h = 0, w = 0, c = 0;

  image->matrix2d = malloc (image->height * sizeof (image->matrix2d));
  for (int i = 0; i < image->height; ++i)
    {
      image->matrix2d[i] = malloc (sizeof (struct PIXEL) * image->width);
    }

  while (c != image->height * image->width)
    {
      // we can have comments after this
      if (tok[0] == '#')
        {
          tok = strstr (tok, "\n");
        }
      else if (tok[0] == '\n' && tok[1] == '#')
        {
          tok = strstr (tok, "#");
        }
      else if (tok[0] == '\n' || tok[0] == ' ')
        {
          tok += 1;
        }
      else
        {
          // allocating ONE pixel
          struct PIXEL *pixel = malloc (sizeof (struct PIXEL));

          // reading each color info and moving to the next
          pixel->r = strtoul (tok, &tok, 10);
          pixel->g = strtoul (tok, &tok, 10);
          pixel->b = strtoul (tok, &tok, 10);

          // assigning pixel to it's respective matrix position
          image->matrix2d[h][w] = *pixel;
          c += 1;
          w = c % image->width;
          h = c / image->width;
          // freeing that one pixel
          free (pixel);
        }
    }

  // free original buffer
  free (buffer);
}

void get_data (char *file_name, image_t *image)
{
  FILE *file;
  char *buffer;
  size_t result;

  file = fopen (file_name, "rb");
  // get the file size
  fseek (file, 0, SEEK_END);
  image->file_size = ftell (file);
  rewind (file);

  // allocate memory
  buffer = (char *) malloc (sizeof (char) * image->file_size);
  if (buffer == NULL)
    {
      fputs ("Memory error", stderr);
      exit (2);
    }

  // copy the file into buffer and close original file
  result = fread (buffer, 1, image->file_size, file);
  if (result != image->file_size)
    {
      fputs ("Reading error", stderr);
      exit (3);
    }
  fclose (file);

  image->width = 0;
  image->height = 0;
  image->limit_value = 0;

  // get rid of first line
  sscanf (buffer, "P3");
  char *tok = strstr (buffer, "\n");

  // getting dimensions
  while (1)
    {
      // we can have comments after this
      if (tok[0] == '#')
        {
          tok = strstr (tok, "\n");
        }
      else if (tok[0] == '\n' && tok[1] == '#')
        {
          tok = strstr (tok, "#");
        }
      else if (tok[0] == '\n')
        {
          tok += 1;
        }
      else if (image->width == 0 && image->height == 0 && image->limit_value == 0)
        {
          image->width = (int) strtol (tok, &tok, 10);
          image->height = (int) strtol (tok, &tok, 10);
          image->limit_value = (int) strtol (tok, &tok, 10);

          if (image->width != 0 && image->height != 0)
            {
              break;
            }
        }
    }

  write_to_matrix (&buffer[0], &tok[0], image);
}

void get_data_stdin (image_t *image)
{
  FILE *file;
  char *buffer;

  file = stdin;

  // allocate memory
  buffer = (char *) malloc (sizeof (char) * INT_MAX);
  fread (buffer, 1, INT_MAX, file);
  fclose (file);

  image->width = 0;
  image->height = 0;
  image->limit_value = 0;

  // get rid of first line
  sscanf (buffer, "P3");
  char *tok = strstr (buffer, "\n");

  // getting dimensions
  while (1)
    {
      // we can have comments after this
      if (tok[0] == '#')
        {
          tok = strstr (tok, "\n");
        }
      else if (tok[0] == '\n' && tok[1] == '#')
        {
          tok = strstr (tok, "#");
        }
      else if (tok[0] == '\n')
        {
          tok += 1;
        }
      else if (image->width == 0 && image->height == 0 && image->limit_value == 0)
        {
          image->width = (int) strtol (tok, &tok, 10);
          image->height = (int) strtol (tok, &tok, 10);
          image->limit_value = (int) strtol (tok, &tok, 10);

          if (image->width != 0 && image->height != 0)
            {
              break;
            }
        }
    }

  image->file_size = image->width * image->height;

  write_to_matrix (&buffer[0], &tok[0], image);
}

void wad (char *filename, char *filename2, int option, int pxs, int r, int g, int b)
{
  image_t image;
  image.n_pixels = pxs;
  pixel_t border_pixel = {r, g, b};
  image.border = border_pixel;

  if (option == 1)
    {
      get_data (filename, &image);
      border (&image);
      write_to_stdout (&image);
    }
  else if (option == 2)
    {
      get_data (filename, &image);
      border (&image);
      write_to_file (&image, filename2);
    }
  else
    {
      get_data_stdin (&image);
      border (&image);
      write_to_stdout (&image);
    }
}

int main (int argc, char *argv[])
{

  int mod = 0;
  int pixels = (int) strtol (argv[1], &argv[1], 10);
  int red = (int) strtol (argv[2], &argv[2], 10);
  int green = (int) strtol (argv[3], &argv[3], 10);
  int blue = (int) strtol (argv[4], &argv[4], 10);

  mod = argc - 1;
  switch (mod)
    {
      case 5:
        wad (argv[mod], NULL, 1, pixels, red, green, blue);
      break;
      case 6:
        wad (argv[mod - 1], argv[mod], 2, pixels, red, green, blue);
      break;
      default:
        wad (NULL, NULL, 3, pixels, red, green, blue);
      break;
    }
}
