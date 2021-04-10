#include "file.h"

void write_to_file (struct IMAGE *image)
{
  FILE *fptr;

  // check for any errors
  if ((fptr = fopen ("meme.ppm", "w")) == NULL)
    {
      printf ("Failed to open file\n");
      exit (-1);
    }

  // write header
  fprintf (fptr, "P3\n%d %d\n%d\n", image->width, image->height, image->limit_value);

  // write pixels
  for (int j = 0; j < image->height; j++)
    {
      for (int k = 0; k < image->width; ++k)
        {
          fprintf (fptr, "%hhu\t%hhu\t%hhu\t", image->matrix2d[j][k].r, image->matrix2d[j][k].g, image->matrix2d[j][k].b);
        }
    }

  // debug to make sure we have a result image smaller or equal in size to input
  printf ("INPUT IMAGE SIZE: %ld\nNEW IMAGE SIZE: %ld\n", image->file_size, ftell (fptr));

  // free memory
  fclose (fptr);
}

void write_to_matrix (char* buffer, char* tok, struct IMAGE *image)
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

void get_data (char* buffer, struct IMAGE *image)
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

void wad (char *arg)
{
  struct IMAGE image;

  FILE *file;
  size_t result;
  char *buffer;

  // open in binary mode
  file = fopen (arg, "rb");
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
  vertical_reflection (&image);

  write_to_file (&image);
}