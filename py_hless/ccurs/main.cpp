#include <iostream>
#include <ncurses.h>

int main()
{
	initscr();
	printw("Hello World\n");
	refresh();
	getch();
	endwin();

	return 0;
}
