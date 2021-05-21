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
    int start_x;
    int start_y;
    int final_x;
    int final_y;
    int limit_value;
    long file_size;
    struct PIXEL **matrix2d;
} image_t;

void recut (struct IMAGE *image)
{
  int new_width = image->final_x - image->start_x;
  int new_height = image->final_y - image->start_y;

  struct PIXEL **recut_matrix = malloc (new_height * sizeof (recut_matrix));
  for (int i = 0; i < new_height; ++i)
    {
      recut_matrix[i] = malloc (sizeof (struct PIXEL) * new_width);
    }

  for (int j = image->start_y; j < image->final_y; ++j)
    {
      for (int k = image->start_x; k < image->final_x; ++k)
        {
          recut_matrix[j][k] = image->matrix2d[j][k];
        }
    }

  if (new_height < image->height) image->height = new_height;
  else exit (0);
  if (new_width < image->width) image->width = new_width;
  else exit (0);

  image->matrix2d = recut_matrix;
}

void write_to_file (struct IMAGE *image, char *f2_name)
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
          fprintf (fptr, "%hhu %hhu %hhu\n", image->matrix2d[j][k].r, image->matrix2d[j][k].g, image->matrix2d[j][k].b);
        }
    }

  // free memory
  fclose (fptr);
}

void write_to_stdout (struct IMAGE *image)
{
  // write header
  printf ("P3\n%d %d\n%d\n", image->width, image->height, image->limit_value);

  // write pixels
  for (int j = 0; j < image->height; ++j)
    {
      for (int k = 0; k < image->width; ++k)
        {
          printf ("%hhu %hhu %hhu\n", image->matrix2d[j][k].r, image->matrix2d[j][k].g, image->matrix2d[j][k].b);
        }
    }
}

void write_to_matrix (char *buffer, char *tok, struct IMAGE *image)
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

void get_data (char *file_name, struct IMAGE *image)
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

void get_data_stdin (struct IMAGE *image)
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

void wad (char *filename, char *filename2, int option, int sx, int sy, int fx, int fy)
{
  struct IMAGE image;
  image.start_x = sx;
  image.start_y = sy;
  image.final_x = fx;
  image.final_y = fy;

  if (option == 1)
    {
      get_data (filename, &image);
      recut (&image);
      write_to_stdout (&image);
    }
  else if (option == 2)
    {
      get_data (filename, &image);
      recut (&image);
      write_to_file (&image, filename2);
    }
  else
    {
      get_data_stdin (&image);
      recut (&image);
      write_to_stdout (&image);
    }
}

int main (int argc, char *argv[])
{
  /*int dR, dG, dB;
  dR = dG = dB = INT_MAX;
  for (int i = 1; i < argc; ++i)
    {
      char *cur = argv[i];
      int cur_int = (int ) strtol (cur, &cur, 10);

      if (dR == INT_MAX) dR = cur_int;
      else if (dG == INT_MAX) dG = cur_int;
      else
        {
          dB = cur_int;
          mod = i;
          break;
        }
    }*/

  int mod = 0;
  int start_x = (int) strtol (argv[1], &argv[1], 10);
  int start_y = (int) strtol (argv[2], &argv[2], 10);
  int final_x = (int) strtol (argv[3], &argv[3], 10);
  int final_y = (int) strtol (argv[4], &argv[4], 10);

  mod = argc - 1;
  switch (mod)
    {
      case 5:
        wad (argv[mod], NULL, 1, start_x, start_y, final_x, final_y);
      break;
      case 6:
        wad (argv[mod - 1], argv[mod], 2, start_x, start_y, final_x, final_y);
      break;
      default:
        wad (NULL, NULL, 3, start_x, start_y, final_x, final_y);
      break;
    }
}