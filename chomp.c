#include "chomp.h"

#include <stdlib.h>

struct _chomp_state
{
  size_t cols;
  size_t *heights;
};

static chomp_state *chomp_init(size_t cols, size_t *heights)
{
  chomp_state *s = malloc(sizeof(*s));
  s->cols = cols;
  s->heights = heights;
  return s;
}

chomp_state *chomp_initial(size_t rows, size_t cols)
{
  size_t *heights = malloc(sizeof(*heights) * cols);
  for (size_t c = 0; c < cols; c++)
    {
      heights[c] = rows;
    }
  return chomp_init(cols, heights);
}

chomp_state *chomp_create(size_t cols, const size_t *heights)
{
  size_t *h = malloc(sizeof(*heights) * cols);
  for (size_t c = 0; c < cols; c++)
    {
      h[c] = heights[c];
    }
  return chomp_init(cols, h);
}

bool chomp_is_terminal(const chomp_state *s)
{
  size_t c = 0;
  while (c < s->cols && s->heights[c] == 0)
    {
      c++;
    }
  return c == s->cols;
}

size_t chomp_get_width(const chomp_state *s)
{
  return s->cols;
}

size_t chomp_get_height(const chomp_state *s, size_t col)
{
  return s->heights[col];
}

chomp_state *chomp_next(const chomp_state *s, const chomp_move *move)
{
  size_t *heights = malloc(sizeof(*heights) * s->cols);
  for (size_t c = 0; c < s->cols; c++)
    {
      if (c < move->col || s->heights[c] < move->row)
	{
	  heights[c] = s->heights[c];
	}
      else
	{
	  heights[c] = move->row;
	}
    }
  return chomp_init(s->cols, heights);
}

size_t chomp_hash(const void * s);

int chomp_compare(const void *s1, const void *s2);

void chomp_free(void *s)
{
  chomp_destroy((chomp_state *)s);
}

void chomp_destroy(chomp_state *s)
{
  free(s->heights);
  free(s);
}
   
