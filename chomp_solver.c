#include <stdio.h>
#include <stdlib.h>

#include "chomp.h"
#include "gmap.h"

void solve_chomp(const chomp_state *start, chomp_move *win, gmap *memo);
void free_value(const void *key, void *value, void *context);

int main(int argc, char **argv)
{
  if (argc < 2)
    {
      fprintf(stderr, "USAGE: %s col0-height [col1-height [...]]\n", argv[0]);
      return 1;
    }

  size_t *heights = malloc(sizeof(size_t) * (argc - 1));
  for (size_t a = 1; a < argc; a++)
    {
      int h = atoi(argv[a]);
      if (h < 0)
	{
	  fprintf(stderr, "%s: column heights must be positive\n", argv[0]);
	  free(heights);
	  return 1;
	}
      heights[a - 1] = h;
    }

  chomp_state *s = chomp_create(argc - 1, heights);
  free(heights);
  chomp_move win;

  gmap *memo = gmap_create(chomp_copy, chomp_compare, chomp_hash, chomp_free);
			
  solve_chomp(s, &win, memo);
  if (win.row == 0 && win.col == 0)
    {
      fprintf(stdout, "give up\n");
    }
  else
    {
      fprintf(stdout, "eat column %zu row %zu\n", win.col, win.row);
    }

  chomp_destroy(s);
  gmap_for_each(memo, free_value, NULL);
  gmap_destroy(memo);
  return 0;
}
   
void solve_chomp(const chomp_state *start, chomp_move *win, gmap *memo)
{
  if (chomp_is_terminal(start))
    {
      win->row = -1;
      win->col = -1;
      return;
    }

  if (gmap_contains_key(memo, start))
    {
      chomp_move *m = gmap_get(memo, start);
      *win = *m;
      return;
    }
  
  win->row = 0;
  win->col = 0;
  size_t cols = chomp_get_width(start);
  for (size_t c = 0; c < cols && (win->row == 0 && win->col == 0); c++)
    {
      size_t rows = chomp_get_height(start, c);
      for (size_t r = 0; r < rows && (win->row == 0 && win->col == 0); r++)
	{
	  chomp_move try = {r, c};
	  chomp_state *next = chomp_next(start, &try);
	  chomp_move m;
	  solve_chomp(next, &m, memo);
	  chomp_destroy(next);
	  if (m.row == 0 && m.col == 0)
	    {
	      win->row = r;
	      win->col = c;
	    }
	}
    }
  chomp_move *copy = malloc(sizeof(*copy));
  *copy = *win;
  gmap_put(memo, start, copy);
}

void free_value(const void *key, void *value, void *context)
{
  free(value);
}
