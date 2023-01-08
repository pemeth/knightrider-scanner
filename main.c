#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DIR_CHANGE  1   // To be XORed if direction is changed
#define DIR_LR      0   // Left-to-Right
#define DIR_RL      1   // Right-to-Left

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

    // Print quit hint
    char const quit_hint[] = "Press any key to quit";
    mvprintw(LINES - 1, (COLS - strlen(quit_hint)) / 2, quit_hint);
    refresh();

    // Set non-blocking input for infinite loop
    timeout(0);

    // Animation initialization
    int const snake_y = LINES / 2;
    int const snake_len = COLS / 4;
    int snake_head = 0;
    int direction = DIR_LR;

    // Red intensities based on snake length
    for (int i = 1; i <= snake_len; i++) {
        int const color_idx = i + 10; // To not overwrite defaults

        init_color(color_idx, (1000 / snake_len) * i, 0, 0);
        init_pair(i, color_idx, COLOR_BLACK);
    }

    // Animate
    while(getch() == ERR) {
        // Clear row // TODO use either delch or addch
        //move(snake_y, 0);
        attron(COLOR_PAIR(1));
        for (int i = 0; i < COLS; i++) {
            mvaddch(snake_y, i, ACS_BLOCK);
            //delch();
        }
        attroff(COLOR_PAIR(1));
        refresh();

        if (direction == DIR_LR) {
            for (int i = 0; i < snake_len; i++) {
                attron(COLOR_PAIR(i + 1));
                mvaddch(snake_y, (snake_head - (snake_len - 1)) + i, ACS_BLOCK);
                attroff(COLOR_PAIR(i + 1));
            }
        } else { /* direction == DIR_RL */
            for (int i = 0; i < snake_len; i++) {
                attron(COLOR_PAIR(i + 1));
                mvaddch(
                    snake_y,
                    COLS - 1 - ((snake_head - (snake_len - 1)) + i),
                    ACS_BLOCK
                );
                attroff(COLOR_PAIR(i + 1));
            }
        }
        refresh();

        snake_head = (snake_head + 1) % (COLS);
        if (snake_head == 0) {
            direction ^= DIR_CHANGE;
        }

        usleep(25000);
    }

    endwin();

    return 0;
}
