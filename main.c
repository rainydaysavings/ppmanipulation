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

  // debug to make sure we have a result image smaller or equal in size to input
  // printf ("INPUT IMAGE SIZE: %ld\nNEW IMAGE SIZE: %ld\n", image->file_size, ftell (fptr));

  // free memory
  fclose (fptr);
}

void write_to_file_stdin (struct IMAGE *image)
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

  for (int h = 0; h < image->height; ++h)
    {
      for (int w = 0; w < image->width; ++w)
        {
          // checking for any more comments
          if (tok[0] == '#')
            {
              tok = strstr (tok, "\n");
            }

          // allocating ONE pixel
          struct PIXEL *pixel = malloc (sizeof (struct PIXEL));

          // reading each color info and moving to the next
          pixel->r = strtoul (tok, &tok, 10);
          pixel->g = strtoul (tok, &tok, 10);
          pixel->b = strtoul (tok, &tok, 10);

          // assigning pixel to it's respective matrix position
          image->matrix2d[h][w] = *pixel;

          // freeing that one pixel
          free (pixel);
        }
    }

  // free original buffer
  free (buffer);
}

void get_data (char *buffer, struct IMAGE *image)
{
  // get rid of first line
  sscanf (buffer, "P3");
  char *tok = strstr (buffer, "\n");

  // getting dimensions
  while (strlen (tok) != 0)
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
      else
        {
          image->width = (int) strtol (tok, &tok, 10);
          image->height = (int) strtol (tok, &tok, 10);
          image->limit_value = (int) strtol (tok, &tok, 10);
        }

      // all successfully assigned, else try again at beginning of buffer
      if (image->width != 0 && image->height != 0 && image->limit_value != 0) break;
      //else tok = strstr (buffer, "\n");
    }

  write_to_matrix (&buffer[0], &tok[0], image);
}

void wad (int argc, char *argv[argc])
{
  struct IMAGE image;

  FILE *file;
  size_t result;
  char *buffer;

  if (argc != 1)
    {
      file = fopen (argv[1], "rb");
    }
  else
    {
      file = stdin;
    }

  if (file == NULL)
    {
      fputs ("File error", stderr);
      exit (1);
    }

  // get the file size
  fseek (file, 0, SEEK_END);
  image.file_size = ftell (file);
  rewind (file);

  // allocate memory
  buffer = (char *) malloc (sizeof (char) * image.file_size);
  if (buffer == NULL)
    {
      fputs ("Memory error", stderr);
      exit (2);
    }

  // copy the file into buffer and close original file
  result = fread (buffer, 1, image.file_size, file);
  if (result != image.file_size)
    {
      fputs ("Reading error", stderr);
      exit (3);
    }
  fclose (file);

  image.width = 0;
  image.height = 0;
  image.limit_value = 0;
  // getting data, exclude comments, create an array
  get_data (&buffer[0], &image);

  // modify image, write to file
  v_flip (&image);

  if (argc == 1)
    {
      write_to_file (&image, argv[0]);
    }
  else if (argc == 3)
    {
      write_to_file (&image, argv[2]);
    }
  else
    {
      write_to_file_stdin (&image);
    }

}

int main (int argc, char *argv[])
{
  wad (argc, argv);
}
