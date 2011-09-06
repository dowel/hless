#ifndef __STATUS_BAR_H__
#define __STATUS_BAR_H__

#include "types.h"

class StatusBar
{
public:
	StatusBar();
	~StatusBar();

	void update_screen_size(u32 width, u32 height);

private:
	u32 _width; // of entire screen...
	u32 _height; // of entire screen...
};

#endif

