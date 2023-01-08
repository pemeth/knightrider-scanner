#include <ncurses.h>

#include "animation.h"


void mode_basic(animation_settings_t const *s)
{
    if (s->direction == DIR_LR) {
        for (int i = 0; i < s->snake_len; i++) {
            attron(COLOR_PAIR(i + 1));
            mvaddch(
                s->snake_y,
                (s->snake_head - (s->snake_len - 1)) + i,
                ACS_BLOCK);
            attroff(COLOR_PAIR(i + 1));
        }
    } else { /* direction == DIR_RL */
        for (int i = 0; i < s->snake_len; i++) {
            attron(COLOR_PAIR(i + 1));
            mvaddch(
                s->snake_y,
                (COLS - 1) - ((s->snake_head - (s->snake_len - 1)) + i),
                ACS_BLOCK
            );
            attroff(COLOR_PAIR(i + 1));
        }
    }
    refresh();
}


void mode_smooth(animation_settings_t const *s)
{
    if (s->direction_change_flag) {
        if (s->direction == DIR_LR) {
            for (int i = 0; i < s->snake_len; i++) {
                attron(COLOR_PAIR(s->snake_len - i));
                mvaddch(
                    s->snake_y,
                    -s->snake_head + i,
                    ACS_BLOCK
                );
                attroff(COLOR_PAIR(s->snake_len - i));
            }
        } else {
            for (int i = 0; i < s->snake_len; i++) {
                attron(COLOR_PAIR(s->snake_len - i));
                mvaddch(
                    s->snake_y,
                    (COLS - 1) - (-s->snake_head + i),
                    ACS_BLOCK
                );
                attroff(COLOR_PAIR(s->snake_len - i));
            }
        }
    }
    refresh();

    // The above code handles just the edges, rest is same as basic
    mode_basic(s);
}
