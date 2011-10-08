#ifndef __BORDER_H__
#define __BORDER_H__

#include "types.h"

class Border
{
public:
	Border(u32 x, u32 y, u32 width, u32 height);
	void draw();

private:
	u32 _x;
	u32 _y;
	u32 _width;
	u32 _height;
};

#endif

