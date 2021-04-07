#include "file.h"

char *buffer;
char *tok;
int next;
int width;
int height;
int limit_value;
long file_size;
struct PIXEL **matrix2d;

void write_to_file ()
{
  FILE *fptr;
  if((fptr = fopen("meme.ppm", "w")) == NULL)
    {
      printf("Failed to open file\n");
      exit(-1);
    }

  fprintf(fptr, "P3\n%d %d\n%d\n", width, height, limit_value);

  for (int j = 0; j < height; j++)
    {
      for (int k = 0; k < width; ++k)
        {
          fprintf (fptr, "%hhu\t%hhu\t%hhu\n", matrix2d[j][k].r, matrix2d[j][k].g, matrix2d[j][k].b);
        }
    }

  fclose (fptr);
}

void write_to_matrix ()
{
  matrix2d = malloc (height * sizeof(matrix2d));
  for (int i = 0; i < height; ++i)
    {
      matrix2d[i] = malloc (sizeof (struct PIXEL) * width);
    }

  for (int h = 0; h < height; ++h)
    {
      for (int w = 0; w < width; ++w)
        {
          if (tok[0] == '#')
            {
              tok = strstr (tok, "\n");
            }
          struct PIXEL *pixel = malloc (sizeof (struct PIXEL));
          sscanf (tok, "%hhu %hhu %hhu %n", &pixel->r, &pixel->g, &pixel->b, &next);
          matrix2d[h][w] = *pixel;

          free(pixel);
          tok += next;
        }
    }

    free(buffer);
}

void get_data ()
{
  next = 0;
  char *ptr;

  // getting dimensions
  sscanf (buffer, "P3%n", &next);

  // get rid of first line
  tok = strstr (buffer, "\n");
  tok += next - 1;

  // we can have comments after this
  // getting dimensions
  while (strlen (tok) != 0)
    {
      next = 0;
      // next line pls
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
          width = (int ) strtol(tok, &tok, 10);
          height = (int ) strtol(tok, &tok, 10);
          limit_value = (int ) strtol(tok, &tok, 10);
        }
      if (width != 0 && height != 0 && limit_value != 0) break;
    }

  printf ("m: %d\nn: %d\nlimit_value: %d\n", width, height, limit_value);
  printf ("\n");
}

void wad (char *arg)
{
  FILE *file;
  size_t result;

  // open file in binary mode
  file = fopen (arg, "rb");
  if (file == NULL)
    {
      fputs ("File error", stderr);
      exit (1);
    }

  // get filesize
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

  get_data ();
  write_to_matrix ();

  struct PIXEL **p_matrix = &matrix2d[0];
  vertical_reflection (height, width, p_matrix);
  write_to_file ();
}