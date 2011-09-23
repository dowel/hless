#include <iostream>

#include "status_bar.h"

static __attribute__((unused)) const char* MODULE_NAME = "status";

StatusBar::StatusBar()
{
	update_terminal_size();
}

StatusBar::~StatusBar()
{

}

void StatusBar::redraw(Buffer::iterator& cursor)
{
	// Drawing first line...
	std::stringstream ss;
	ss << "  " << cursor.string_name();
	u64 size = cursor.get_chunk()->get_file().get_size();
	ss << "  " << cursor->get_offset() << "/" << cursor.get_chunk()->get_file().get_size();
	ss << "  " << (cursor->get_offset() + cursor->get_length()) * 100 / size << "%";

	Line line1(ss.str());
	Line rest(' ', _width - line1.length());
	Log1("Rest: " << rest);
	Log1("Line1: " << line1);
	line1 += rest;
	Log1("Line1: " << line1);
	_brush.draw_line(_text_height, line1, Brush::status_bar_color);
}

