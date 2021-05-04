#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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

void v_flip (struct IMAGE *image)
{
  pixel_t *pixel_column;
  int mid = image->height / 2;
  for (int j = 0; j < mid; ++j)
    {
      pixel_column = image->matrix2d[j];
      image->matrix2d[j] = image->matrix2d[image->height - j - 1];
      image->matrix2d[image->height - j - 1] = pixel_column;
    }
}

void h_flip (struct IMAGE *image)
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

void d_flip (struct IMAGE *image)
{
  v_flip (image);
  h_flip (image);
}

void rot_right (struct IMAGE *image)
{
  // need to change in writing to file the loops order to use this
  struct PIXEL **image1;
  image1 = malloc (image->width * sizeof (image->matrix2d));
  for (int i = 0; i < image->width; ++i)
    {
      image1[i] = malloc (sizeof (struct PIXEL) * image->height);
    }

  int n_rows = image->height;
  int n_cols = image->width;
  pixel_t *pixel_row;

  for (int j = 0; j < n_rows; ++j)
    {
      pixel_row = image->matrix2d[j];
      for (int k = 0; k < n_cols; ++k)
        {
          image1[k][j] = pixel_row[k];
        }
    }

  int tmp_width = image->width;
  int tmp_height = image->height;
  image->matrix2d = image1;
  image->width = tmp_height;
  image->height = tmp_width;
  h_flip (image);
}

void rot_left (struct IMAGE *image)
{
  // need to change in writing to file the loops order to use this
  rot_right (image);
  v_flip (image);
  h_flip (image);
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
  // allocating an array with the same width and height as input image
  image->matrix2d = malloc (image->height * sizeof (image->matrix2d));
  for (int i = 0; i < image->height; ++i)
    {
      image->matrix2d[i] = malloc (sizeof (struct PIXEL) * image->width);
    }

  int h = 0, w = 0, c = 0;
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

void wad (int argc, char *argv[argc], int option, int mod)
{
  struct IMAGE image;

  if (option == 1)
    {
      get_data (argv[1], &image);

      switch (mod)
        {
          case 1:
            h_flip(&image);
          break;
          case 2:
            v_flip(&image);
          break;
          case 3:
            d_flip(&image);
          break;
          case 4:
            rot_right(&image);
          break;
          case 5:
            rot_left(&image);
          break;
          default:
            break;
        }

      write_to_stdout (&image);
    }
  else if (option == 2)
    {
      get_data (argv[1], &image);

      switch (mod)
        {
          case 1:
            h_flip(&image);
            break;
          case 2:
            v_flip(&image);
            break;
          case 3:
            d_flip(&image);
            break;
          case 4:
            rot_right(&image);
            break;
          case 5:
            rot_left(&image);
            break;
          default:
            break;
        }

      write_to_file (&image, argv[1]);
    }
  else if (option == 3)
    {
      get_data_stdin (&image);

      switch (mod)
        {
          case 1:
            h_flip(&image);
          break;
          case 2:
            v_flip(&image);
          break;
          case 3:
            d_flip(&image);
          break;
          case 4:
            rot_right(&image);
          break;
          case 5:
            rot_left(&image);
          break;
          default:
            break;
        }

      write_to_stdout (&image);
    }
}

int main (int argc, char *argv[])
{
  int mod = 5;
  switch (argc)
    {
      case 1:
        wad (argc, argv, 3, mod);
      break;
      case 2:
        wad (argc, argv, 1, mod);
      break;
      case 3:
        wad (argc, argv, 2, mod);
      break;
      default:
        wad (argc, argv, 3, mod);
      break;
    }

}
