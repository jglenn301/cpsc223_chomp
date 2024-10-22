#ifndef __CHOMP_H__
#define __CHOMP_H__

#include <stdlib.h>
#include <stdbool.h>

typedef struct _chomp_state chomp_state;

/**
 * A move in chomp.  Given as the index of the row and column to eat from
 * with (0,0) in the bottom left (the wormy brownie)
 */
typedef struct
{
  size_t row;
  size_t col;
} chomp_move;

/**
 * Creates the initial chomp state with a full pan of the given size.
 * The caller owns the resulting dynamically allocated state,
 *
 * @param rows a positive integer
 * @param cols a positive integer
 * @return a pointer to the corresponding state
 */
chomp_state *chomp_initial(size_t rows, size_t cols);

/**
 * Creates a chomp state with the given number of brownies remaining
 * in each column.  The caller owns the resulting dynamically
 * allocated state and retains ownership of the array passed in.
 *
 * @param cols a positive integer
 * @param heights an array of cols nonnegative integers
 * @return a pointer to the corresponding state
 */
chomp_state *chomp_create(size_t cols, const size_t *heights);

/**
 * Determines if the given state is the terminal state.  The terminal state is
 * the state with no brownies remaining.
 *
 * @param s a valid pointer to a state, non-NULL
 * @return true if and only if s is terminal
 */
bool chomp_is_terminal(const chomp_state *s);

/**
 * Returns the width of the pan in the given state.  The width includes any
 * empty columns.
 *
 * @param s a valid pointer to a state, non-NULL
 * @return the width of the state
 */
size_t chomp_get_width(const chomp_state *s);

/**
 * Returns the number of brownies remaining in the given column for the given state.
 *
 * @param s a valid pointer to a state, non-NULL
 * @param col a nonnegative index into the columns in state s
 * @return the height of col in s
 */
size_t chomp_get_height(const chomp_state *s, size_t col);

/**
 * Creates the chomp state resulting from making the given move in the given
 * state.  The caller owns the resulting dynamically allocated state.
 *
 * @param s a valid pointer to a state, non-NULL
 * @param move a valid move in state s
 * @return a pointer to the corresponding state
 */
chomp_state *chomp_next(const chomp_state *s, const chomp_move *move);

/**
 * Computes the hash value for the given state.
 *
 * @param p a valid pointer to a state, non-NULL
 * @return the hash value
 */
size_t chomp_hash(const void *p);

/**
 * Compares the two states.  The return value is negative if the first
 * state comes first, positive if the second comes first, and 0 if the
 * states are equal.
 *
 * @param p1 a valid pointer to a state, non-NULL
 * @param p2 a valid pointer to a state, non-NULL
 * @return the result of the comparison
 */
int chomp_compare(const void *p1, const void *p2);

/**
 * Creates a copy of the given state.  The caller owns the resulting
 * copy.
 *
 * @param p a valid pointer to a state, non-NULL
 */
void *chomp_copy(const void *p)

/**
 * Destroys the given chomp state.
 *
 * @param p a valid pointer to a state, non-NULL
 */
void chomp_free(void *p);

/**
 * Destroys the given state, releasing any allocated resources belonging
 * to it.
 *
 * @param s a valid pointer to a state, non-NULL
 */
void chomp_destroy(chomp_state *s);

#endif
