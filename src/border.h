#ifndef __BORDER_H__
#define __BORDER_H__

#include <ncurses.h>

#include "types.h"
#include "brush.h"

class Border
{
public:
	Border();
	Border(u32 x, u32 y, u32 width, u32 height);
	~Border();

	void set_geometry(u32 x, u32 y, u32 width, u32 height);
	void draw(Color* color);

private:
	u32 _x;
	u32 _y;
	u32 _width;
	u32 _height;
	WINDOW* _window;
};

#endif

