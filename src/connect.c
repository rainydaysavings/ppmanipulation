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
    int limit_value;
    long file_size;
    pixel_t **matrix2d;
} image_t;

void connect (image_t *image1, image_t *image2, image_t *fusion)
{
  int lowest_height = image1->height < image2->height ? image1->height : image2->height;
  int real_width = image1->width + image2->width;
  fusion->height = lowest_height;
  fusion->width = real_width;
  fusion->limit_value = image1->limit_value;

  pixel_t **image_fusion = malloc (lowest_height * sizeof (image_fusion));
  for (int i = 0; i < lowest_height; ++i)
    {
      image_fusion[i] = malloc (sizeof (struct PIXEL) * real_width);
    }

  for (int j = 0; j < lowest_height; ++j)
    {
      for (int k = 0; k < real_width; ++k)
        {
          if (k < image1->width) image_fusion[j][k] = image1->matrix2d[j][k];
          else image_fusion[j][k] = image2->matrix2d[j][k - image1->width];
        }
    }

  fusion->matrix2d = image_fusion;
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

void wad (char *filename1, char *filename2, char *filename_out, int option)
{
  image_t image1;
  image_t image2;
  image_t fusion;

  if (option == 2)
    {
      get_data (filename1, &image1);
      get_data (filename2, &image2);

      connect (&image1, &image2, &fusion);
      write_to_file (&fusion, filename_out);
    }
  else
    {
      get_data (filename1, &image1);
      get_data (filename2, &image2);

      connect (&image1, &image2, &fusion);
      write_to_stdout (&fusion);
    }
}

int main (int argc, char *argv[])
{

  int mod = 0;
  char *image1 = argv[1];
  char *image2 = argv[2];

  mod = argc - 1;
  switch (mod)
    {
      case 2:
        wad (image1, image2, NULL, 1);
      break;
      case 3:
        wad (image1, image2, argv[mod], 2);
      break;
      default:
        wad (image1, image2, NULL, 1);
      break;
    }
}
