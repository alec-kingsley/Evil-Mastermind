#include "board.h"
#include "guess.h"
#include <stdio.h>
#include <stdlib.h>

static void play_guess(Board *board, color_t color_a, color_t color_b,
                       color_t color_c, color_t color_d, uint8_t black_ct,
                       uint8_t white_ct) {
    color_t colors[COLOR_CT];
    guess_t guess;
    colors[0] = color_a;
    colors[1] = color_b;
    colors[2] = color_c;
    colors[3] = color_d;
    guess = guess_init(colors, black_ct, white_ct);
    board_submit_guess(board, guess);

    board_print(board);
    printf("Possiblities left: %i\n", board_get_possibility_ct(board));
    printf("Best guess: ");
    guess_print_colors(board_get_best_guess(board));
    printf("\n");
}

int main(void) {
    Board *board = board_create();
    if (board == NULL) goto main_fail;

    printf("Possiblities left: %i\n", board_get_possibility_ct(board));
    printf("Best guess: ");
    guess_print_colors(board_get_best_guess(board));
    printf("\n");
    play_guess(board, COLOR_RED, COLOR_YELLOW, COLOR_YELLOW, COLOR_RED, 1, 1);
    play_guess(board, COLOR_BLUE, COLOR_GREEN, COLOR_YELLOW, COLOR_YELLOW, 2, 1);
    play_guess(board, COLOR_GREEN, COLOR_WHITE, COLOR_RED, COLOR_RED, 1, 0);
    play_guess(board, COLOR_GREEN, COLOR_BLACK, COLOR_YELLOW, COLOR_YELLOW, 1, 1);
    play_guess(board, COLOR_YELLOW, COLOR_WHITE, COLOR_YELLOW, COLOR_BLUE, 0, 3);
    play_guess(board, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_YELLOW, 1, 2);
    play_guess(board, COLOR_BLUE, COLOR_YELLOW, COLOR_BLUE, COLOR_YELLOW, 4, 0);

    board_destroy(board);
    return 0;

main_fail:
    return 1;
}
