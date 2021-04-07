#include "file.h"

char *buffer;
char *tok;
int width;
int height;
int limit_value;
long file_size;
struct PIXEL **matrix2d;

void write_to_file ()
{
  FILE *fptr;

  // check for any errors
  if ((fptr = fopen ("meme.ppm", "w")) == NULL)
    {
      printf ("Failed to open file\n");
      exit (-1);
    }

  // write header
  fprintf (fptr, "P3\n%d %d\n%d\n", width, height, limit_value);

  // write pixels
  for (int j = 0; j < height; j++)
    {
      for (int k = 0; k < width; ++k)
        {
          fprintf (fptr, "%hhu\t%hhu\t%hhu\t", matrix2d[j][k].r, matrix2d[j][k].g, matrix2d[j][k].b);
        }
    }

  // debug to make sure we have a result image smaller or equal in size to input
  printf ("INPUT IMAGE SIZE: %ld\nNEW IMAGE SIZE: %ld\n", file_size, ftell (fptr));

  // free memory
  fclose (fptr);
  free (matrix2d);
}

void write_to_matrix ()
{
  // allocating an array with the same width and height as input image
  matrix2d = malloc (height * sizeof (matrix2d));
  for (int i = 0; i < height; ++i)
    {
      matrix2d[i] = malloc (sizeof (struct PIXEL) * width);
    }

  for (int h = 0; h < height; ++h)
    {
      for (int w = 0; w < width; ++w)
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
          matrix2d[h][w] = *pixel;

          // freeing that one pixel
          free (pixel);
        }
    }

  // free original buffer
  free (buffer);
}

void get_data ()
{
  // get rid of first line
  sscanf (buffer, "P3");
  tok = strstr (buffer, "\n");

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
          width = (int) strtol (tok, &tok, 10);
          height = (int) strtol (tok, &tok, 10);
          limit_value = (int) strtol (tok, &tok, 10);
        }

      // all successfully assigned, else try again at beginning of buffer
      if (width != 0 && height != 0 && limit_value != 0) break;
      else tok = strstr (buffer, "\n");
    }
}

void wad (char *arg)
{
  FILE *file;
  size_t result;

  // open in binary mode
  file = fopen (arg, "rb");
  if (file == NULL)
    {
      fputs ("File error", stderr);
      exit (1);
    }

  // get the file size
  fseek (file, 0, SEEK_END);
  file_size = ftell (file);
  rewind (file);

  // allocate memory
  buffer = (char *) malloc (sizeof (char) * file_size);
  if (buffer == NULL)
    {
      fputs ("Memory error", stderr);
      exit (2);
    }

  // copy the file into buffer
  result = fread (buffer, 1, file_size, file);
  if (result != file_size)
    {
      fputs ("Reading error", stderr);
      exit (3);
    }
  fclose (file);

  // getting data, exclude comments, create an array
  get_data ();
  write_to_matrix ();

  // modify image, write to file
  struct PIXEL **p_matrix = &matrix2d[0];
  vertical_reflection (height, width, p_matrix);
  write_to_file ();
}