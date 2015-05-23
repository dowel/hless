#include <iostream>
#include <iomanip>

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
	Log1("Redrawing status bar");

	std::vector<std::shared_ptr<Mark>> before_cursor(Brush::MarkColorsTotal);
	std::vector<std::shared_ptr<Mark>> after_cursor(Brush::MarkColorsTotal);

	// Initialize before_cursor and after_cursor to 0s.
	for (int i = 0; i < Brush::MarkColorsTotal; i++) {
		before_cursor[i] = 0;
		after_cursor[i] = 0;
	}

	Mark c(cursor);
	for (Mark m : _marks) {
		Log1("Looking at mark " << m);
		Log3("Comparing with cursor " << c);
		if (m < c) {
			Log2("Mark " << m << " is less then cursor. Setting for color " << m.get_color_index());
			before_cursor[m.get_color_index()].reset(new Mark(m));
		} else if (m == c) {
			Log2("Mark " << m << " is equal to cursor.");
		} else {
			Log2("Mark " << m << " is greater then cursor. Setting for color " << m.get_color_index());
			after_cursor[m.get_color_index()].reset(new Mark(m));
		}
	}

	for (int i = 0, x = 2; i < Brush::MarkColorsTotal; i++) {
		std::stringstream ss;
		if (before_cursor[i] != 0) {
			s64 n = before_cursor[i]->get_iterator().distance_lines(cursor);
			Log3("Drawing " << n << " lines before cursor, from " << *before_cursor[i] << 
				" until " << cursor);
			ss << std::setfill(' ') << std::setw(4) << n;
		} else {
			ss << "----";
		}

		ss << "|";

		if (after_cursor[i] != 0) {
			s64 n = after_cursor[i]->get_iterator().distance_lines(cursor);
			Log3("Drawing " << n << " lines after cursor, from " << *after_cursor[i] << 
				" until " << cursor);
			ss << std::setfill(' ') << std::setw(4) << std::left << n;
		} else {
			ss << "----";
		}

		Log3("Marks line: " << ss.str());

		Line l(ss.str());
		_brush.draw_text(x, _text_height + 1, l, _brush.marks[i]);
		x += l.length() + 1;
	}
}

