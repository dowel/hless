#include <iostream>
#include <ncurses.h>

int main()
{
	initscr();
	start_color();
	use_default_colors();
	
	init_pair(1, COLOR_WHITE, COLOR_BLACK);

	attron(COLOR_PAIR(1));
	mvaddstr(0, 0, "Hello world");
	attroff(COLOR_PAIR(1));
	refresh();
	getch();
	endwin();

	return 0;
}
