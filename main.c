#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "animation.h"


void clear_lines(int from, int to)
{
    for (int i = from; i <= to; i++) {
        move(i, 0);
        clrtoeol();
    }
}


void help(int toggle)
{
    char* const msgs[] = {
        "Controls:",
        "Arrow Up     increase snake speed",
        "Arrow Down   decrease snake speed",
        "n            cycle animation modes",
        "h            hide / show this text",
    };

    int const n_lines = sizeof(msgs) / sizeof(msgs[0]);

    if (toggle == TRUE) {
        for (int i = 0; i < n_lines; i++) {
            mvprintw(i, 0, msgs[i]);
        }
    } else {
        clear_lines(0, n_lines - 1);
    }

    refresh();
}


int main(int argc, char* const argv[])
{
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Check for color support
    if (has_colors() == FALSE) {
        // TODO figure out what to do in this case
        printw("Colors are not supported in this terminal\n");
        refresh();

        getch();
        endwin();
        return 0;
    }

    // Start color if supported
    start_color();

    // Print quit hint and help
    char const quit_hint[] = "Press any key to quit";
    mvprintw(LINES - 1, (COLS - strlen(quit_hint)) / 2, quit_hint);
    help(TRUE);
    refresh();

    // Set non-blocking input for infinite loop
    timeout(0);

    // Animation initialization
    animation_settings_t s = {
        .snake_y = LINES / 2,
        .snake_len = COLS / 4,
        .snake_head = 1,
        .direction = DIR_LR,
        .direction_change_flag = 0, // TODO unify with other true/false values
        .min_y = 0,
        .min_x = 0,
        .max_y = LINES - 1,
        .max_x = COLS - 1,
    };
    animation_settings_t s_split_left = s;
    animation_settings_t s_split_right = s;

    // Bounds in split mode
    int const cols_half = COLS / 2;
    s_split_left.max_x = cols_half - 1;
    s_split_right.min_x = COLS % 2 == 0 ? cols_half : cols_half + 1;

    // Starting settings in split mode
    s_split_right.snake_head = s_split_right.min_x + 1;
    s_split_right.direction ^= DIR_CHANGE;

    void (*animation_func)(animation_settings_t const *) = &mode_smooth;

    // Controls
    int keypress;
    int help_flag = TRUE;
    int exit_flag = FALSE;
    int mode = MODE_SMOOTH;
    useconds_t snake_speed = DEFAULT_SPEED;

    // Red intensities based on snake length
    for (int i = 1; i <= s.snake_len; i++) {
        int const color_idx = i + 10; // To not overwrite defaults

        init_color(color_idx, (1000 / s.snake_len) * i, 0, 0);
        init_pair(i, color_idx, COLOR_BLACK);
    }

    // Animate
    while (!exit_flag) {

        // Handle controls
        keypress = getch();
        switch (keypress)
        {
        case KEY_UP:
            snake_speed -= SPEED_STEP;
            if (snake_speed < MAX_SPEED) {
                snake_speed = MAX_SPEED;
            }
            break;
        case KEY_DOWN:
            snake_speed += SPEED_STEP;
            if (snake_speed > MIN_SPEED) {
                snake_speed = MIN_SPEED;
            }
            break;
        case 'n':
            // Change mode and select the animation function on mode change
            mode = (mode + 1) % MODES_N;
            if (mode == MODE_BASIC) { animation_func = &mode_basic; }
            else if (mode == MODE_SMOOTH) { animation_func = &mode_smooth; }
            else if (mode == MODE_SPLIT) { animation_func = &mode_smooth; }
            break;
        case 'h':
            help_flag ^= TRUE;
            help(help_flag);
            break;
        case ERR:
            // No key pressed, continue as usual
            break;
        default:
            // Any other key pressed, exit
            exit_flag ^= TRUE;
            break;
        }

        // Clear snake row
        clear_lines(s.snake_y, s.snake_y);

        // Render frame and update animation variables based on selected mode
        if (mode == MODE_BASIC || mode == MODE_SMOOTH) {
            (*animation_func)(&s);
            update(&s);
        } else if (mode == MODE_SPLIT) {
            (*animation_func)(&s_split_left);
            (*animation_func)(&s_split_right);
            update(&s_split_left);
            update(&s_split_right);
        }
        refresh();

        usleep(snake_speed);
    }

    endwin();

    return 0;
}
