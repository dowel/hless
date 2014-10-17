#ifndef __BRUSH_H__
#define __BRUSH_H__

#include <string>
#include <ncurses.h>
#include <iostream>

#include "types.h"
#include "line.h"

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

	void seton(WINDOW* window);
	void setoff(WINDOW* window);

private:
	std::string _name;
	ColorValue _fg;
	ColorValue _bg;
	u32 _pair;

	static u32 _last_pair;
};

class ColorRange
{
public:
	ColorRange()
		: _start(0)
		, _length(0)
		, _color(0) { }

	ColorRange(u32 start, u32 length, Color* color)
		: _start(start)
		, _length(length)
		, _color(color) { }

	void set_start(u32 start) { _start = start; }
	void set_length(u32 length) { _length = length; }
	void set_color(Color* color) { _color = color; }

	u32 get_start() { return _start; }
	u32 get_length() { return _length; }
	Color* get_color() { return _color; }

private:
	u32 _start;
	u32 _length;
	Color* _color;
};

typedef std::list<ColorRange*> ColorRanges;

class Brush
{
public:
	Brush() { }
	~Brush() { }

	/**
	 * Should be called exactly once...
	 */
	void init_colors();

	static Color* text_color;
	static Color* cursor_color;
	static Color* status_bar_color;
	static Color* border_color;
	static Color* progress_done_color;
	static Color* progress_remains_color;
	static Color* active_button_color;

	typedef std::vector<Color*> MarkColors;
	static MarkColors marks;

	/**
	 * Routines below used to draw text on screen using specified color. Difference between two is that 
	 * first expects coordinates while second only expects line number and assumes line length is as 
	 * width of the terminal. 
	 */
	void draw_text(u32 x, u32 y, Line& line, Color* color = text_color);
	void draw_line(u32 y, Line& line, Color* color = text_color);

//  void draw_text(u32 x, u32 y, Line& line, ColorRange& range);
//  void draw_line(u32 y, Line& line, ColorRange& range);

	/**
	 * Routines below similar to draw_text and draw_line() adove. The difference is that they access 
	 * color ranges, allowing to draw line with several different colors in the line. 
	 */
	void draw_text(u32 x, u32 y, Line& line, ColorRanges& ranges);
	void draw_line(u32 y, Line& line, ColorRanges& ranges);
private:
};

std::ostream& operator<<(std::ostream& os, Color* color);

#endif

