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

	Log1("Asked to redraw screen starting from " << current);
	Log1("Cursor is " << cursor);
	while ((n < _text_height) && (current != Buffer::end())) {
		Log3("Considering line " << current << " for drawing");

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

	Log1("Asked to stage redraw screen starting from " << current);
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
	Log1("Asked to stage reversed redraw screen starting from " << bottom << "/" << line_in_bottom);

	Buffer::iterator it = bottom;
	u32 temp = line_in_bottom;
	u32 i = _text_height;
	LineList lst;
	while ((i > 0) && (it != Buffer::end())) {
		lst.clear();
		read_and_split(it, lst);
		u32 size = lst.size(); // somehow std::min doesn't like to compare lst.size() and u32...
		Log2("Considering line " << it << ". Lines inside " << size << ", lines left " << i);
		i -= std::min(temp, size); // decrease by either line_in_bottom or lst.size()
		temp = 0xffffffff;
		it--;
		Log1("bottom " << bottom << ", it " << it);
	}

	if (it == Buffer::end()) {
		current = _buffer.begin();
		line_in_current = 0;
	} else {
		current = it;
		line_in_current = lst.size() + i;
	}

	Log1("After staged redrawing, bottom " << bottom << "/" << line_in_bottom << ", current " << current << 
		 "/" << line_in_current);
}

