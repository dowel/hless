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
	use_default_colors();
	_brush.init_colors();

	_maxx = stdscr->_maxx;
	_maxy = stdscr->_maxy;
	_width = _maxx + 1;
	_text_height = _maxy + 1 - status_bar_height;

	cbreak();
	keypad(stdscr, TRUE);
	noecho();

	Log1("Screen initialized with " << _maxx << "x" << _maxy << " window size");
}

Screen::~Screen()
{
	endwin();
}

void Screen::read_and_split(Buffer::iterator& it, LineList& res)
{
	Line line;
	_buffer.read_line(it, line);
	line.strip_back();
	res.clear();
	line.split_lines(_width, res);	
}

void Screen::redraw_screen(Buffer::iterator& in_current, 
	u32 line_in_current,
	Buffer::iterator& cursor,
	Buffer::iterator& bottom,
	u32& line_in_bottom)
{
	u32 n = 0, i;
	Buffer::iterator current = in_current;

	erase();

	Log1("Redrawing screen from " << current);
	Log1("Cursor is " << cursor);
	while ((n < _text_height) && (current != Buffer::end())) {
		Log3("Drawing " << current << ". Done " << n << " out of " << _text_height);

		LineList lst;
		LineList::iterator it;

		read_and_split(current, lst);

		Log3("Line was split into " << lst.size() << " lines of " << _width << " characters");
		for (it = lst.begin(), i = 0; (i < line_in_current) && (it != lst.end()); it++, i++);
		for (line_in_bottom = 0; (it != lst.end()) && (n < _text_height); it++, n++, line_in_bottom++) {
			if (current == cursor) {
				_brush.draw_line(n, *it->get(), Brush::cursor_color);
			} else {
				_brush.draw_line(n, *it->get());
			}
		}
		if (line_in_bottom >= 1) {
			line_in_bottom--;
		}
		line_in_current = 0;

		bottom = current;
		current++;
	}
}

void Screen::stage_redraw_screen(Buffer::iterator& in_current,
	u32 line_in_current,
	Buffer::iterator& bottom,
	u32& line_in_bottom)
{
	u32 n = 0, i;
	Buffer::iterator current = in_current;

	Log1("Staing redraw from " << current);
	while ((n < _text_height) && (current != Buffer::end())) {
		LineList lst;
		LineList::iterator it;
		read_and_split(current, lst);

		for (it = lst.begin(), i = 0; (i < line_in_current) && (it != lst.end()); it++, i++);
		for (line_in_bottom = 0; (it != lst.end()) && (n < _text_height); it++, n++, line_in_bottom++);

		if (line_in_bottom >= 1) {
			line_in_bottom--;
		}
		line_in_current = 0;

		bottom = current;
		current++;
	}
}

void Screen::stage_reversed_redraw_screen(Buffer::iterator& bottom, 
	u32 line_in_bottom, 
	Buffer::iterator& current, 
	u32& line_in_current)
{
	Log1("Staging reversed redraw from " << bottom << "@" << line_in_bottom);

	u32 i = _text_height;
	Buffer::iterator it = bottom;
	u32 lines_in = line_in_bottom + 1;
	u32 prev_lines_in = 0;
	LineList lst;
	while (i > 0) {
		i -= lines_in;
		current = it;
		it--;
		if (it == Buffer::end()) {
			break;
		}
		lst.clear();
		read_and_split(it, lst);
		Log2("Considering line " << it << ". Lines " << lst.size() << ", left " << i);
		prev_lines_in = lines_in;
		lines_in = lst.size();
	}
	line_in_current = prev_lines_in + i - 1;

	Log1("After staged redrawing, bottom " << bottom << "@" << line_in_bottom << ", current " << current << 
		 "@" << line_in_current);
}

