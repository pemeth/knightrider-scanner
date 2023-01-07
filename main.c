#include <ncurses.h>
#include <stdlib.h>

int main(int argc, char* const argv[])
{
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    printw("Hello world\n");
    refresh();

    getch();
    endwin();

    return 0;
}
