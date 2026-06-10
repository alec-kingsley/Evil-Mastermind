#pragma once
#ifndef BOARD_H
#define BOARD_H

#include "guess.h"

typedef struct Board Board;

#define GUESS_LIMIT 10
#define LIE_LIMIT 1

/**
 * Get # of possibilities remaining.
 */
uint32_t board_get_possibility_ct(Board *self);

/**
 * Print current board state.
 */
void board_print(Board *self);

/**
 * Submit a guess to the board.
 */
void board_submit_guess(Board *self, guess_t guess);

/**
 * Create `board` object.
 * Return `null` on failure.
 */
Board *board_create(void);

/**
 * Destroy `self`.
 * Does nothing if `self` is void.
 */
void board_destroy(Board *self);

#endif
