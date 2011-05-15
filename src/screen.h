#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "buffer.h"
#include "brush.h"

class Screen
{
public:
	Screen(Buffer& buffer);
	~Screen();

	static const int status_bar_height;

	void redraw_screen(Buffer::iterator& current, 
		u32 line_in_current,
		Buffer::iterator& cursor,
		Buffer::iterator& bottom,
		u32& line_in_bottom);

	u32 get_width() { return _width; }

private:
	u32 _maxx;
	u32 _maxy;
	u32 _width;
	u32 _text_height;
	Brush _brush;
	Buffer& _buffer;
};

#endif

