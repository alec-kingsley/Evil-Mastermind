#include "guess.h"
#include <stdlib.h>

#define BLACK_CT_OFF ((PEG_CT - 1) * COLOR_BITS + PEG_BITS)
#define WHITE_CT_OFF ((PEG_CT - 1) * COLOR_BITS + 2 * PEG_BITS)

#define PEG_CT_MASK ((1 << PEG_BITS) - 1)
#define COLOR_MASK ((1 << COLOR_BITS) - 1)

uint8_t guess_white_ct(guess_t guess) {
    return (guess >> WHITE_CT_OFF) & PEG_CT_MASK;
}

uint8_t guess_black_ct(guess_t guess) {
    return (guess >> BLACK_CT_OFF) & PEG_CT_MASK;
}

color_t guess_color(guess_t guess, uint8_t peg_number) {
    return (guess >> (COLOR_BITS * peg_number)) & COLOR_MASK;
}

guess_t guess_init(color_t colors[PEG_CT], uint8_t black_ct, uint8_t white_ct) {
    guess_t guess = 0;
    size_t i = 0;

    for (i = 0; i < PEG_CT; i++) {
        guess |= colors[i] << i * COLOR_BITS;
    }

    guess |= black_ct << BLACK_CT_OFF;
    guess |= white_ct << WHITE_CT_OFF;

    return guess;
}
