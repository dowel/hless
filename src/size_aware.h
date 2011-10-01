#ifndef __SIZE_AWARE_H__
#define __SIZE_AWARE_H__

#include <ncurses.h>

#include "config.h"
#include "log.h"

class SizeAware 
{
public:
	SizeAware() { }

	void update_terminal_size()
	{
		const char* MODULE_NAME = "screen"; // shadows module wise MODULE_NAME elsewhere...
		static bool printed = false;

		_maxx = stdscr->_maxx;
		_maxy = stdscr->_maxy;
		_width = _maxx + 1;
		_height = _maxy + 1;
		_text_height = _maxy + 1 - Config::status_bar_height;

		if (!printed) {
			Log1("Terminal initialized with " << _width << "x" << _height << " terminal size");
			printed = true;
		} else {
			Log2("Terminal updated with " << _width << "x" << _height << " terminal size");
		}
	}

protected:
	u32 _maxx;
	u32 _maxy;
	u32 _width;
	u32 _height;
	u32 _text_height;
};

#endif

