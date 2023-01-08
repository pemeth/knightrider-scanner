#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "animation.h"

// TODO combine the help print / clear functions into one,
//      and make `help_rows` automatically detect the number of rows
void print_help()
{
    mvprintw(0,0, "Controls:");
    mvprintw(1,0, "Arrow Up     increase snake speed");
    mvprintw(2,0, "Arrow Down   decrease snake speed");
    mvprintw(3,0, "n            cycle animation modes");
    mvprintw(4,0, "h            hide / show this text");
    refresh();
}

void clear_help()
{
    int const help_rows = 5;
    for (int i = 0; i < help_rows; i++) {
        for (int j = 0; j < COLS; j++) {
            mvdelch(i,0);
        }
    }
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
    print_help();
    refresh();

    // Set non-blocking input for infinite loop
    timeout(0);

    // Animation initialization
    animation_settings_t s = {
        .snake_y = LINES / 2,
        .snake_len = COLS / 4,
        .snake_head = 1,
        .direction = DIR_LR,
        .direction_change_flag = 0
    };
    void (*animation_func)(animation_settings_t const *) = &mode_basic;

    // Controls
    int keypress;
    int help_flag = ~0;
    int exit_flag = 0;
    int mode = 0;
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
            // Select the animation function on mode change
            mode = (mode + 1) % MODES_N;
            if (mode == MODE_BASIC) { animation_func = &mode_basic; }
            else if (mode == MODE_SMOOTH) { animation_func = &mode_smooth; }
            break;
        case 'h':
            help_flag = ~help_flag;
            if (help_flag) {
                print_help();
            } else {
                clear_help();
            }
            break;
        case ERR:
            // No key pressed, continue as usual
            break;
        default:
            // Any other key pressed, exit
            exit_flag = ~exit_flag;
            break;
        }

        // Clear snake row
        attron(COLOR_PAIR(1));
        for (int i = 0; i < COLS; i++) {
            mvaddch(s.snake_y, i, ACS_BLOCK);
        }
        attroff(COLOR_PAIR(1));
        refresh();

        // Animate based on selected mode
        (*animation_func)(&s);

        // Update animation variables
        s.snake_head = (s.snake_head + 1) % (COLS);
        if (s.snake_head == 0) {
            s.direction ^= DIR_CHANGE;
            s.direction_change_flag = ~s.direction_change_flag;
        }

        if (s.direction_change_flag && s.snake_head > s.snake_len - 1) {
            // Direction change in smooth mode complete (snake passed its tail)
            s.direction_change_flag = ~s.direction_change_flag;
        }

        usleep(snake_speed);
    }

    endwin();

    return 0;
}
