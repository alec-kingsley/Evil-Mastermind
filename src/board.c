#include "board.h"
#include "colors.h"
#include "guess.h"
#include "reporter.h"
#include <math.h>
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

    /* could be calculated, but this is faster */
    size_t possible_solution_ct;
};

typedef struct {
    uint8_t black_ct;
    uint8_t white_ct;
} evaluation_t;

static evaluation_t get_evaluation(guess_t expected, guess_t guess) {
    size_t i, j;
    evaluation_t evaluation;
    evaluation.black_ct = 0;
    evaluation.white_ct = 0;
    for (i = 0; i < PEG_CT; i++) {
        if (guess_color(expected, i) == guess_color(guess, i))
            evaluation.black_ct++;
    }
    for (i = 0; i < PEG_CT; i++) {
        for (j = 0; j < PEG_CT; j++) {
            if (guess_color(expected, j) == guess_color(guess, i)) {
                evaluation.white_ct++;
                /* don't re-count the same color */
                expected = guess_with_color(expected, j, COLOR_CT);
                break;
            }
        }
    }
    evaluation.white_ct -= evaluation.black_ct;
    return evaluation;
}

typedef float entropy_t;

static entropy_t board_get_entropy(Board *self, guess_t guess) {
    /* black by white */
    /* increment possible_answers[1][0] to suggest 1 possibility of 1 black and
     * no white */
    size_t possible_evaluations[PEG_CT + 1][PEG_CT + 1];
    size_t i, j;
    entropy_t entropy;
    evaluation_t evaluation;
    entropy_t response_probability;
    for (i = 0; i <= PEG_CT; i++) {
        /* minus i since i + j can't be greater than peg count */
        for (j = 0; j <= PEG_CT - i; j++) {
            possible_evaluations[i][j] = 0;
        }
    }

    for (i = 0; i < g_total_possibility_ct; i++) {
        if (self->impossible_solutions[i]) continue;
        evaluation = get_evaluation(g_all_possiblities[i], guess);
        possible_evaluations[evaluation.black_ct][evaluation.white_ct]++;
        if (evaluation.black_ct + evaluation.white_ct > PEG_CT) {
            report_logic_error("Woah! Impossible eval!");
        }
    }

    entropy = 0.0;
    for (i = 0; i <= PEG_CT; i++) {
        for (j = 0; j <= PEG_CT - i; j++) {
            if (possible_evaluations[i][j] != 0) {
                response_probability = (1.0 * possible_evaluations[i][j])
                                       / self->possible_solution_ct;
                entropy += response_probability
                           * log(1.0 / response_probability) / log(2);
            }
        }
    }
    return entropy;
}

guess_t board_get_best_guess(Board *self) {
    entropy_t entropy = 0.0, max_entropy = 0.0;
    guess_t best_guess = g_all_possiblities[0];
    bool best_guess_is_possible = false;
    size_t i;
    for (i = 0; i < g_total_possibility_ct; i++) {
        entropy = board_get_entropy(self, g_all_possiblities[i]);
        /* prioritize possible solutions */
        if (entropy > max_entropy
            || (!best_guess_is_possible && !self->impossible_solutions[i]
                && entropy > max_entropy - 0.01)) {

            max_entropy = entropy;
            best_guess = g_all_possiblities[i];
            best_guess_is_possible = !self->impossible_solutions[i];
        }
    }
    return best_guess;
}

uint32_t board_get_possibility_ct(Board *self) {
    return self->possible_solution_ct;
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
            guess_print_colors(guess);
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

static void board_update_lie_counts(Board *self, guess_t guess) {
    size_t i;
    evaluation_t expected;
    guess_t test_guess;
    lie_ct_t lie_ct;

    for (i = 0; i < g_total_possibility_ct; i++) {
        if (self->impossible_solutions[i]) continue;
        test_guess = g_all_possiblities[i];
        expected = get_evaluation(test_guess, guess);
        lie_ct = get_lie_ct(guess_black_ct(guess), guess_white_ct(guess),
                            expected.black_ct, expected.white_ct);
        switch (lie_ct) {
        case ZERO_LIES: break;
        case ONE_LIE:
            self->lie_cts[i]++;
            if (self->lie_cts[i] > LIE_LIMIT) {
                self->impossible_solutions[i] = true;
                self->possible_solution_ct--;
            }

            break;
        case MULTIPLE_LIES:
            self->impossible_solutions[i] = true;
            self->possible_solution_ct--;
        }
    }
}

void board_submit_guess(Board *self, guess_t guess) {
    if (self->guess_ct == GUESS_LIMIT) {
        report_logic_error(FILENAME
                           ": attempt to submit more guesses than limit");
    }
    self->guesses[self->guess_ct++] = guess;
    board_update_lie_counts(self, guess);
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

    self->possible_solution_ct = g_total_possibility_ct;

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
