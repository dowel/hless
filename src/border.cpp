#include <ncurses.h>

#include "border.h"

Border::Border(u32 x, u32 y, u32 width, u32 height)
	: _x(x)
	, _y(y)
	, _width(width)
	, _height(height) 
{ }

void Border::draw()
{
	box(stdscr, 10, 10);
	refresh();
}

