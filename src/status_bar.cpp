#include <iostream>

#include "status_bar.h"

static __attribute__((unused)) const char* MODULE_NAME = "status";

StatusBar::StatusBar(Marks& marks)
	: _marks(marks)
{
	update_terminal_size();
}

StatusBar::~StatusBar()
{

}

void StatusBar::redraw(Buffer::iterator& cursor)
{
	update_terminal_size();

	// Drawing first line...
	std::stringstream ss1;
	ss1 << "  " << cursor.string_name();
	u64 size = cursor.get_chunk()->get_file().get_size();
	ss1 << "  " << cursor->get_offset() << "/" << cursor.get_chunk()->get_file().get_size();
	ss1 << "  " << (cursor->get_offset() + cursor->get_length()) * 100 / size << "%";

	Line line1(ss1.str());
	Line rest(' ', _width - line1.length());
	Log1("Rest: " << rest);
	Log1("Line1: " << line1);
	line1 += rest;
	Log1("Line1: " << line1);
	_brush.draw_line(_text_height, line1, Brush::status_bar_color);

	Line line2(' ', _width);
	_brush.draw_line(_text_height + 1, line2, Brush::status_bar_color);

	draw_marks(cursor);
}

void StatusBar::draw_marks(Buffer::iterator& cursor)
{
	Mark* before_cursor[Brush::MarkColorsTotal] = { 0, };
	Mark* after_cursor[Brush::MarkColorsTotal] = { 0, };
	for (Mark m : _marks) {
		Mark c(cursor);
		if (m < c) {
			before_cursor[m.get_color_index()] = m;
		} else if (m == c) {
			before_cursor[m.get_color_index()] = 0;
			after_cursor[m.get_color_index()] = 0;
		} else {
			after_cursor[m.get_color_index()] = m;
		}
	}

	for (int i = 0; i < Brush::MarkColorsTotal; i++) {
		std::stringstream ss;
		if (before_cursor[i] != 0) {
			s64 n = before_cursor[i]->get_iterator().distance_lines(cursor);
			ss << n;
		} else {
			ss << "---";
		}

		ss << "|";

		if (after_cursor[i] != 0) {
			s64 n = cursor.distance_lines(*after_cursor[i]);
			ss << n;
		} else {
			ss << "---";
		}
		Line l(ss.str());
		_brush.draw_text(2, _text_height + 1, l, _brush.marks[i]);
	}
}

