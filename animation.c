#include <ncurses.h>

#include "animation.h"


void mode_basic(animation_settings_t const *s)
{
    for (int i = 0; i < s->snake_len; i++) {
        int const curr_cell_idx = (s->snake_head - (s->snake_len - 1)) + i;

        if (curr_cell_idx < s->min_x) {
            continue;
        }

        int const final_idx = s->direction == DIR_LR
            ? curr_cell_idx
            : s->max_x - curr_cell_idx + s->min_x;

        attron(COLOR_PAIR(i + 1));
        mvaddch(
            s->snake_y,
            final_idx,
            ACS_BLOCK
        );
        attroff(COLOR_PAIR(i + 1));
    }
    refresh();
}


void mode_smooth(animation_settings_t const *s)
{
    if (s->direction_change_flag) {
        // This approach simulates the smooth change in direction by creating
        // another snake, which "continues" in the direction of the original
        // snake before the direction change.
        // Therefore the `s->direction` variable holds the current direction
        // of the original snake, but the animation for the clone is carried out
        // in the opposite direction.

        // Normalize
        int const snake_head_norm = s->snake_head - s->min_x;
        int const max_x_norm = s->max_x - s->min_x;

        for (int i = 0; i < s->snake_len; i++) {
            int const curr_cell_idx = (-snake_head_norm + i);

            if (curr_cell_idx < 0) {
                continue;
            }

            int const final_idx = s->direction == DIR_LR
                ? curr_cell_idx
                : max_x_norm - curr_cell_idx;

            attron(COLOR_PAIR(s->snake_len - i));
            mvaddch(
                s->snake_y,
                final_idx + s->min_x, // Renormalize
                ACS_BLOCK
            );
            attroff(COLOR_PAIR(s->snake_len - i));
        }
    }
    refresh();

    // The above code handles just the edges, rest is same as basic
    mode_basic(s);
}


void update(animation_settings_t *s)
{
    int snake_head_norm = s->snake_head - s->min_x;
    int max_norm = s->max_x - s->min_x;

    snake_head_norm = (snake_head_norm + 1) % (max_norm + 1);
    if (snake_head_norm == 0) {
        s->direction ^= DIR_CHANGE;
        s->direction_change_flag ^= TRUE;
    }

    if (s->direction_change_flag && snake_head_norm > s->snake_len - 1) {
        // Direction change in smooth mode complete (snake passed its tail)
        s->direction_change_flag ^= TRUE;
    }

    s->snake_head = snake_head_norm + s->min_x;
}
