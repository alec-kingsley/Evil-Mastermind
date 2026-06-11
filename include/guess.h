#pragma once
#ifndef GUESS_H
#define GUESS_H

#include <stdint.h>

#define COLOR_CT 6
#define PEG_CT 4

typedef enum {
    COLOR_RED = 0,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_BLACK,
    COLOR_WHITE
} color_t;

/**
 *  >= instead of > to allow a "fake color" (COLOR_CT) for
 * easier algorithms
 */
#if COLOR_CT >= 15
#error too many colors
#else
#if COLOR_CT >= 7
#define COLOR_BITS 4
#else
#if COLOR_CT >= 3
#define COLOR_BITS 3
#else
#define COLOR_BITS 2
#endif
#endif
#endif

#if PEG_CT > 15
#error too many pegs
#else
#if PEG_CT > 7
#define PEG_BITS 4
#else
#if PEG_CT > 3
#define PEG_BITS 3
#else
#define PEG_BITS 2
#endif
#endif
#endif

#if PEG_BITS * 2 + COLOR_BITS * PEG_CT > 64
#error too many pegs and/or colors
#else
#if PEG_BITS * 2 + COLOR_BITS * PEG_CT > 32
typedef uint64_t guess_t;
#else
#if PEG_BITS * 2 + COLOR_BITS * PEG_CT > 16
typedef uint32_t guess_t;
#else
#if PEG_BITS * 2 + COLOR_BITS * PEG_CT > 8
typedef uint16_t guess_t;
#else
typedef uint8_t guess_t;
#endif
#endif
#endif
#endif

/**
 * Print colors of guess as a series of " O ",
 * where it is colored with ANSI escape codes.
 */
void guess_print_colors(guess_t guess);

/**
 * Get # of white or black pegs for a guess.
 */
uint8_t guess_white_ct(guess_t guess);
uint8_t guess_black_ct(guess_t guess);

/**
 * Get the color at a point in the guess.
 */
color_t guess_color(guess_t guess, uint8_t peg_number);

/**
 * Copy the guess but with a new color at such position.
 */
guess_t guess_with_color(guess_t guess, uint8_t peg_number, color_t color);

/**
 * Initialize a guess.
 */
guess_t guess_init(color_t colors[PEG_CT], uint8_t black_ct, uint8_t white_ct);

#endif
