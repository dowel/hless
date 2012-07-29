#include <ncurses.h>

#include "log.h"
#include "screen.h"

static __attribute__((unused)) const char* MODULE_NAME = "screen";

Screen::Screen(Buffer& buffer)
	: _buffer(buffer)
{
	initscr();
	start_color();
	use_default_colors();
	_brush.init_colors();

	update_terminal_size();

	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
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

void Screen::redraw(Buffer::iterator& in_current, 
	u32 line_in_current,
	Buffer::iterator& cursor,
	Buffer::iterator& bottom,
	u32& line_in_bottom)
{
	update_terminal_size();

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

void Screen::stage_redraw(Buffer::iterator& in_current,
	u32 line_in_current,
	Buffer::iterator& bottom,
	u32& line_in_bottom)
{
	update_terminal_size();

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

void Screen::stage_reversed_redraw(Buffer::iterator& bottom, 
	u32 line_in_bottom, 
	Buffer::iterator& current, 
	u32& line_in_current)
{
	Log1("Staging reversed redraw from " << bottom << "@" << line_in_bottom);

	update_terminal_size();

	s32 i = _text_height;
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

void Screen::stage_redraw_from_middle(Buffer::iterator cursor, 
   Buffer::iterator& current,
   u32& line_in_current) 
{
	Log1("Staging redraw from middle " << cursor);

	update_terminal_size();

	current = cursor;
	line_in_current = 0;
	Buffer::iterator up = current;
	Buffer::iterator down = current;

	LineList lst;
	read_and_split(current, lst);

	s32 lines_up = _text_height / 2;
	u32 lines_down = (_text_height / 2) + lst.size();
	while (1) {
		up--;
		down++;
		if ((up == Buffer::end()) || (down == Buffer::end())) {
			return; // we reached beginning of the buffer; bailing out.
		}

		current = up;

		lst.clear();
		read_and_split(up, lst);
		lines_up -= lst.size();

		lst.clear();
		read_and_split(down, lst);
		lines_down += lst.size();

		if (lines_up <= 0 || lines_down >= _text_height) {
			// reached end of the screen. current should point to our reasonably in the middle line.
			break; 
		}

	}

	// TODO: make sure to set line_in_current to correct value.
}

