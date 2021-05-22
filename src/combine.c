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
    int limit_value;
    long file_size;
    pixel_t **matrix2d;
} image_t;

void combine (image_t *over, image_t *og, image_t *out)
{
  pixel_t **image_out = malloc (og->height * sizeof (image_out));
  for (int i = 0; i < og->height; ++i)
    {
      image_out[i] = malloc (sizeof (struct PIXEL) * og->width);
    }

  for (int j = 0; j < og->height; ++j)
    {
      for (int k = 0; k < og->width; ++k)
        {
          if ((j >= over->start_y && k >= over->start_x) && ((j < (over->height + over->start_y)) && (k < over->width + over->start_x)))
            image_out[j][k] = over->matrix2d[j - over->start_y][k - over->start_x];
          else
            image_out[j][k] = og->matrix2d[j][k];
        }
    }

  out->matrix2d = image_out;
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

void wad (char *over, char *original, char *filename_out, int option, int sx, int sy)
{
  image_t over_img;
  image_t og_img;
  image_t out;

  over_img.start_x = sx;
  over_img.start_y = sy;

  if (option == 1)
    {
      get_data (over, &over_img);
      get_data (original, &og_img);

      out.width = og_img.width;
      out.height = og_img.height;
      out.limit_value = og_img.limit_value;

      combine (&over_img, &og_img, &out);
      write_to_file (&out, filename_out);
    }
  else if (option == 2)
    {
      get_data (over, &over_img);
      get_data (original, &og_img);

      out.width = og_img.width;
      out.height = og_img.height;
      out.limit_value = og_img.limit_value;

      combine (&over_img, &og_img, &out);
      write_to_stdout (&out);
    }
  else
    {
      get_data (over, &over_img);
      get_data_stdin (&og_img);

      out.width = og_img.width;
      out.height = og_img.height;
      out.limit_value = og_img.limit_value;

      combine (&over_img, &og_img, &out);
      write_to_stdout (&out);
    }
}

int main (int argc, char *argv[])
{

  int mod = 0;
  char *over = argv[3];
  int start_x = (int) strtol (argv[1], &argv[1], 10);
  int start_y = (int) strtol (argv[2], &argv[1], 10);

  char *original;
  mod = argc - 1;
  switch (mod)
    {
      case 4:
        original = argv[4];
      wad (over, original, NULL, 2, start_x, start_y);
      break;
      case 5:
        original = argv[4];
      wad (over, original, argv[mod], 1, start_x, start_y);
      break;
      default:
        wad (over, NULL, NULL, 3, start_x, start_y);
      break;
    }
}