#include <stdio.h>
#include <stdlib.h>

#include "chomp.h"

/**
 * Finds a winning move for the given state.  The winning move is
 * copied to the move passed by reference.  If there is no winning
 * move, then the move is set to {0, 0}.  If there are no moves (start
 * is terminal) then the move is set to {-1, -1}.
 *
 * @param start a valid pointer to a state, non-NULL
 * @param win a valid pointer to a move, non-NULL
 */
void solve_chomp(const chomp_state *start, chomp_move *win, gmap *memo);

int main(int argc, char **argv)
{
  if (argc < 2)
    {
      fprintf(stderr, "USAGE: %s col0-height [col1-height [...]]\n", argv[0]);
      return 1;
    }

  // read heights from command line
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

  // create the state to find a winning move from
  chomp_state *s = chomp_create(argc - 1, heights);
  free(heights);
  chomp_move win;

  // find winning move
  solve_chomp(s, &win);

  // output result
  if (win.row == 0 && win.col == 0)
    {
      fprintf(stdout, "give up\n");
    }
  else
    {
      fprintf(stdout, "eat column %zu row %zu\n", win.col, win.row);
    }

  chomp_destroy(s);
  return 0;
}
   
void solve_chomp(const chomp_state *start, chomp_move *win, gmap *memo)
{
  if (chomp_is_terminal(start))
    {
      // terminal state -- have already run; no need to move
      win->row = -1;
      win->col = -1;
      return;
    }

  // initialize to "no winning move"
  win->row = 0;
  win->col = 0;

  // sequential search over all remaining brownies for a winning move
  size_t cols = chomp_get_width(start);
  for (size_t c = 0; c < cols && (win->row == 0 && win->col == 0); c++)
    {
      size_t rows = chomp_get_height(start, c);
      for (size_t r = 0; r < rows && (win->row == 0 && win->col == 0); r++)
	{
	  // get state that results from taking brownie r, c
	  chomp_move try = {r, c};
	  chomp_state *next = chomp_next(start, &try);

	  // find winning move for that state
	  chomp_move m;
	  solve_chomp(next, &m);
	  chomp_destroy(next);

	  if (m.row == 0 && m.col == 0)
	    {
	      // no winning move from next state -- next state is losing
	      // position, so moving to put our opponent there is a
	      // winning move
	      win->row = r;
	      win->col = c;
	    }
	}
    }
}
