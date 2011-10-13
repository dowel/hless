#include <ncurses.h>

#include "border.h"

Border::Border()
	: _x(0)
	, _y(0)
	, _width(0)
	, _height(0)
	, _window(0)
{
}

Border::Border(u32 x, u32 y, u32 width, u32 height)
	: _x(x)
	, _y(y)
	, _width(width)
	, _height(height) 
{
	_window = newwin(_height, _width, _y, _x);
}

void Border::set_geometry(u32 x, u32 y, u32 width, u32 height)
{
	if (_window != 0) {
		delwin(_window);
	}

	_x = x;
	_y = y;
	_width = width;
	_height = height;
	_window = newwin(_height, _width, _y, _x);
}

Border::~Border()
{
	if (_window) {
		delwin(_window);
	}
}

void Border::draw()
{
	if (_window) {
		box(_window, ACS_VLINE, ACS_HLINE);
		wrefresh(_window);
	}
}

