#ifndef __BRUSH_H__
#define __BRUSH_H__

#include <string>
#include <ncurses.h>
#include <iostream>

#include "types.h"
#include "line.h"

class Brush
{
public:
	class Color
	{
	public:
		enum ColorValue
		{
			NO_COLOR = -1,
			BLACK = COLOR_BLACK,
			RED = COLOR_RED,
			GREEN = COLOR_GREEN,
			YELLOW = COLOR_YELLOW,
			BLUE = COLOR_BLUE,
			MAGENTA = COLOR_MAGENTA,
			CYAN = COLOR_CYAN,
			WHITE = COLOR_WHITE,
		};

		Color(std::string name);
		Color(std::string name, ColorValue foreground, ColorValue background);
		~Color();

		std::string name() { return _name; }
		void seton();
		void setoff();

	private:
		std::string _name;
		ColorValue _fg;
		ColorValue _bg;
		u32 _pair;

		static u32 _last_pair;
	};

	Brush() { }
	~Brush() { }

	/**
	 * Should be called exactly once...
	 */
	void init_colors();

	static Color* text_color;
	static Color* cursor_color;
	static Color* status_bar_color;
	static Color* progress_bar_color;

	void draw_text(u32 x, u32 y, Line& line, Color* color = text_color);
	void draw_line(u32 y, Line& line, Color* color = text_color);

private:
};

std::ostream& operator<<(std::ostream& os, Brush::Color* color);

#endif

