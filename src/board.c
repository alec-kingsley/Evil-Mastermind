#include "board.h"
#include "colors.h"
#include "guess.h"
#include "reporter.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define FILENAME "board.c"

static size_t g_total_possibility_ct;
static guess_t *g_all_possiblities = NULL;

struct Board {
    /* uint8_t[total_possibility_ct] */
    uint8_t *lie_cts;
    /* bool[total_possibility_ct] */
    bool *impossible_solutions;

    guess_t guesses[GUESS_LIMIT];
    uint8_t guess_ct;
};

uint32_t board_get_possibility_ct(Board *self) {
    size_t possibility_ct = 0;
    size_t i;

    for (i = 0; i < g_total_possibility_ct; i++) {
        if (!self->impossible_solutions[i]) {
            possibility_ct++;
        }
    }
    return possibility_ct;
}

static void board_print_top(void) {
    uint8_t i;

    printf("┌─────┬");
    for (i = 0; i < PEG_CT; i++) {
        printf("───");
    }
    printf("┐\n");

    printf("│ " BOLD "B W" RESET " │" BOLD);
    for (i = 0; i < PEG_CT; i++) {
        printf(" %u ", i + 1);
    }
    printf(RESET "│\n");


    printf("├─────┼");
    for (i = 0; i < PEG_CT; i++) {
        printf("───");
    }
    printf("┤\n");
}

static void board_print_bottom(void) {
    uint8_t i;
    printf("└─────┴");
    for (i = 0; i < PEG_CT; i++) {
        printf("───");
    }
    printf("┘\n");
}

static void print_guess_colors(guess_t guess) {
    color_t color;
    uint8_t i;
    for (i = 0; i < PEG_CT; i++) {
        color = guess_color(guess, i);
        printf("%s", color == COLOR_RED      ? RED
                     : color == COLOR_BLUE   ? BLUE
                     : color == COLOR_YELLOW ? YELLOW
                     : color == COLOR_GREEN  ? GREEN
                     /* TODO - better way to display black? */
                     : color == COLOR_BLACK ? PURPLE
                     : color == COLOR_WHITE
                         ? ""
                         : (report_logic_error("unknown color"), ""));
        printf(" O " RESET);
    }
}

void board_print(Board *self) {
    uint8_t i, j;
    guess_t guess;

    board_print_top();
    for (i = GUESS_LIMIT - 1; i < GUESS_LIMIT; i--) {
        printf("│");
        if (self->guess_ct > i) {
            guess = self->guesses[i];
            printf(" %u %u ", guess_black_ct(guess), guess_white_ct(guess));
            printf("│");
            print_guess_colors(guess);
        } else {
            printf("     │");
            for (j = 0; j < PEG_CT; j++) {
                printf(" - ");
            }
        }
        printf("│\n");
    }
    board_print_bottom();
}

typedef enum { ZERO_LIES, ONE_LIE, MULTIPLE_LIES } lie_ct_t;

static lie_ct_t get_lie_ct(uint8_t black_ct, uint8_t white_ct,
                           uint8_t expected_black_ct,
                           uint8_t expected_white_ct) {
    bool total_pegs_is_lie
        = black_ct + white_ct != expected_black_ct + expected_white_ct;
    uint8_t black_pegs_off = black_ct > expected_black_ct
                                 ? black_ct - expected_black_ct
                                 : expected_black_ct - black_ct;
    uint8_t white_pegs_off = white_ct > expected_white_ct
                                 ? white_ct - expected_white_ct
                                 : expected_white_ct - white_ct;
    if (white_pegs_off == 0 && black_pegs_off == 0) {
        return ZERO_LIES;
    } else if (white_pegs_off == 0 && black_pegs_off == 1) {
        /* a black peg was added or removed */
        return ONE_LIE;
    } else if (white_pegs_off == 1 && black_pegs_off == 0) {
        /* a white peg was added or removed */
        return ONE_LIE;
    } else if (!total_pegs_is_lie && white_pegs_off == 1
               && black_pegs_off == 1) {
        /* one color was switched to another color */
        return ONE_LIE;
    } else {
        return MULTIPLE_LIES;
    }
}

static void board_update_lie_counts(Board *self, color_t colors[PEG_CT],
                                    uint8_t black_ct, uint8_t white_ct) {
    size_t i, j, k;
    uint8_t expected_black_ct, expected_white_ct;
    color_t test_colors[PEG_CT];
    guess_t test_guess;
    lie_ct_t lie_ct;

    for (i = 0; i < g_total_possibility_ct; i++) {
        if (self->impossible_solutions[i]) continue;
        test_guess = g_all_possiblities[i];
        expected_black_ct = 0;
        expected_white_ct = 0;
        for (j = 0; j < PEG_CT; j++) {
            test_colors[j] = guess_color(test_guess, j);
            if (test_colors[j] == colors[j]) expected_black_ct++;
        }
        for (j = 0; j < PEG_CT; j++) {
            for (k = 0; k < PEG_CT; k++) {
                if (colors[j] == test_colors[k]) {
                    expected_white_ct++;
                    /* don't re-count the same color */
                    test_colors[k] = COLOR_CT;
                    break;
                }
            }
        }
        expected_white_ct -= expected_black_ct;

        lie_ct = get_lie_ct(black_ct, white_ct, expected_black_ct,
                            expected_white_ct);
        switch (lie_ct) {
        case ZERO_LIES: break;
        case ONE_LIE: self->lie_cts[i]++; break;
        case MULTIPLE_LIES: self->impossible_solutions[i] = true;
        }
        if (self->lie_cts[i] > LIE_LIMIT) {
            self->impossible_solutions[i] = true;
        }
    }
}

void board_submit_guess(Board *self, guess_t guess) {
    size_t i;
    uint8_t black_ct = guess_black_ct(guess);
    uint8_t white_ct = guess_white_ct(guess);
    color_t colors[PEG_CT];
    if (self->guess_ct == GUESS_LIMIT) {
        report_logic_error(FILENAME ": attempt to submit more guesses than limit");
    }
    for (i = 0; i < PEG_CT; i++) {
        colors[i] = guess_color(guess, i);
    }

    self->guesses[self->guess_ct++] = guess;
    board_update_lie_counts(self, colors, black_ct, white_ct);
}

static bool ensure_all_possibilities_set(void) {
    size_t i, j;
    color_t colors[PEG_CT] = {0};
    if (g_all_possiblities == NULL) {
        g_total_possibility_ct = COLOR_CT;
        for (i = 1; i < PEG_CT; i++) {
            g_total_possibility_ct *= COLOR_CT;
        }
        g_all_possiblities = calloc(g_total_possibility_ct, sizeof(guess_t));
        if (g_all_possiblities == NULL) {
            report_system_error(FILENAME ": memory allocation failure");
            return false;
        }
        for (i = 0; i < g_total_possibility_ct; i++) {
            /* black_ct and white_ct each 0 */
            g_all_possiblities[i] = guess_init(colors, 0, 0);
            j = 0;
            while (j < PEG_CT && colors[j] == COLOR_CT - 1) {
                colors[j] = 0;
                j++;
            }
            if (j < PEG_CT) {
                colors[j]++;
            }
        }
    }
    return true;
}

Board *board_create(void) {
    Board *self = NULL;

    if (!ensure_all_possibilities_set()) goto board_create_fail;

    self = calloc(1, sizeof(Board));
    if (self == NULL) {
        report_system_error(FILENAME ": memory allocation failure");
        goto board_create_fail;
    }

    self->guess_ct = 0;

    self->lie_cts = calloc(g_total_possibility_ct, sizeof(uint8_t));
    if (self->lie_cts == NULL) {
        report_system_error(FILENAME ": memory allocation failure");
        goto board_create_fail;
    }

    self->impossible_solutions = calloc(g_total_possibility_ct, sizeof(bool));
    if (self->impossible_solutions == NULL) {
        report_system_error(FILENAME ": memory allocation failure");
        goto board_create_fail;
    }

    return self;

board_create_fail:
    board_destroy(self);
    return NULL;
}

void board_destroy(Board *self) {
    if (self) {
        free(self->lie_cts);
        free(self->impossible_solutions);
        free(self);
    }
}
