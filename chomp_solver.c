#include <stdio.h>
#include <stdlib.h>

#include "chomp.h"
#include "gmap.h"

/**
 * Finds all states reachable from the given state and adds them to the given map with value NULL.
 *
 * @param start a valid pointer to a state, non-NULL
 * @param states a map from (pointers to) states to (pointers to) moves
 */
void find_chomp_states(const chomp_state *start, gmap *states);

/**
 * Finds a winning move for the given state, if one exists, and stores it as the value for that state
 * in the given map.  If there is no winning move then the value stored is {0, 0} (take the last
 * brownie and lose immediately).  If the state is terminal, then the value stored is {-1, -1}.
 *
 * @param s a valid pointer to a state, non-NULL
 * @param wins a map from states to moves containing every state reachable in one move from s
 * with corresponding winning move as the value as given above
 */
void find_winning_move(const chomp_state *s, gmap *wins);

/**
 * Frees the given value.
 *
 * @param key ignored
 * @param value a pointer to dynamically allocated memory
 * @param context ignored
 */
void free_value(const void *key, void *value, void *context);

/**
 * Compares two chomp states according to the number of brownies remaining.  The return
 * value is negative if the first state has fewer brownies than the second, positive
 * if the first has more, and 0 if they have the same number.
 *
 * @param p1 a valid pointer to a state, non-NULL
 * @param p2 a valid pointer to a state, non-NULL
 */
int compare_by_brownies(const void *p1, const void *p2);

/**
 * Prints the given move, or "give up" if the move takes the last brownie, or
 * "already won" if the move is invalid.
 *
 * @param out an output stream
 * @param m a valid pointer to a move, non-NULL
 */
void print_winning_move(FILE *out, const chomp_move *m);

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

  // create a map from states to winning moves
  gmap *states = gmap_create(chomp_copy, chomp_compare, chomp_hash, chomp_free);

  // initially the map will have all the states mapped to NULL
  find_chomp_states(s, states);
  size_t num_states = gmap_size(states);
  
  // get all states out of the map
  const void **keys = gmap_keys(states);
  
  // sort in increasing order of number of browies left
  qsort(keys, num_states, sizeof(*keys), compare_by_brownies);

  // terminal state must be first and you win by doing nothing
  chomp_move *no_move = malloc(sizeof(*no_move));
  no_move->row = -1;
  no_move->col = -1;
  gmap_put(states, keys[0], no_move);

  // go through all other states in order
  for (size_t i = 1; i < num_states; i++)
    {
      find_winning_move(keys[i], states);
    }

  // we now have the winning move for every reachable state!
  // but just print the one we were interested in
  chomp_move *win = gmap_get(states, s);
  print_winning_move(stdout, win);
  
  // clean up!
  chomp_destroy(s);
  free(keys);
  gmap_for_each(states, free_value, NULL);
  gmap_destroy(states);
  return 0;
}
   
void find_chomp_states(const chomp_state *start, gmap *all)
{
  if (gmap_contains_key(all, start))
    {
      // already added this state and anything reachable from it
      return;
    }

  // add this state (no winning move calculated yet)
  gmap_put(all, start, NULL);

  // try all states reachable in one move (select any remaining brownie)
  size_t cols = chomp_get_width(start);
  for (size_t c = 0; c < cols; c++)
    {
      size_t rows = chomp_get_height(start, c);
      for (size_t r = 0; r < rows; r++)
	{
	  chomp_move try = {r, c};
	  chomp_state *next = chomp_next(start, &try);
	  find_chomp_states(next, all);
	  chomp_destroy(next);
	}
    }
}

void find_winning_move(const chomp_state *start, gmap *wins)
{
  chomp_move *win = malloc(sizeof(*win));
  if (chomp_is_terminal(start))
    {
      win->row = -1;
      win->col = -1;
    }
  else
    {
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
	      chomp_move *win_from_next = gmap_get(wins, next);
	      
	      if (win_from_next->row == 0 && win_from_next->col == 0)
		{
		  win->row = r;
		  win->col = c;
		}
	      chomp_destroy(next);
	    }
	}
    }
  gmap_put(wins, start, win);
}

void free_value(const void *key, void *value, void *context)
{
  free(value);
}

int compare_by_brownies(const void *p1, const void *p2)
{
  const chomp_state * const *s1 = p1;
  const chomp_state * const *s2 = p2;

  size_t total1 = 0;
  size_t w1 = chomp_get_width(*s1);
  for (size_t c = 0; c < w1; c++)
    {
      total1 += chomp_get_height(*s1, c);
    }

  size_t total2 = 0;
  size_t w2 = chomp_get_width(*s2);
  for (size_t c = 0; c < w2; c++)
    {
      total2 += chomp_get_height(*s2, c);
    }

  if (total1 < total2)
    {
      return -1;
    }
  else if (total1 > total2)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

void print_winning_move(FILE *out, const chomp_move *m)
{
  if (m->col == -1 && m->row == -1)
    {
      fprintf(out, "already won!\n");
    }
  else if (m->col == 0 && m->row == 0)
    {
      fprintf(out, "give up\n");
    }
  else
    {
      fprintf(out, "eat brownie at column %zo, row %zu\n", m->col, m->row);
    }
}
