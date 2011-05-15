#include <ncurses.h>

#include "log.h"
#include "screen.h"

static __attribute__((unused)) const char* MODULE_NAME = "screen";

const int Screen::status_bar_height = 2;

Screen::Screen(Buffer& buffer)
	: _buffer(buffer)
{
	initscr();
	start_color();
	_brush.init_colors();

	_maxx = stdscr->_maxx;
	_maxy = stdscr->_maxy;
	_width = _maxx + 1;
	_text_height = _maxy - status_bar_height;

	cbreak();
	keypad(stdscr, TRUE);
	noecho();

	Log1("Screen initialized with " << _maxx << "x" << _maxy << " window size");
}

Screen::~Screen()
{
	endwin();
}

void Screen::redraw_screen(Buffer::iterator& in_current, 
	u32 line_in_current,
	Buffer::iterator& cursor,
	Buffer::iterator& bottom,
	u32& line_in_bottom)
{
	u32 n = 0, i;
	Buffer::iterator current = in_current;

	Log1("Asked to redraw screen starting from " << current);
	Log1("Cursor is " << cursor);
	while ((n < _text_height) && (current != Buffer::end())) {
		Log1("Considering line " << current << " for drawing on the screen");

		Line line;
		_buffer.read_line(current, line);
		line.strip_back();

		LineList lst;
		LineList::iterator it;

		line.split_lines(_width, lst);
		Log2("Line was split into " << lst.size() << " lines of " << _width << " characters");

		for (it = lst.begin(), i = 0; (it != lst.end()) && (i < line_in_current); it++, i++);
		for (; it != lst.end(); it++, n++) {
			if (current == cursor) {
				_brush.draw_line(n, *it->get(), Brush::cursor_color);
			} else {
				_brush.draw_line(n, *it->get());
			}
		}
		line_in_current = 0;

		bottom = current;
		current++;
	}
}

