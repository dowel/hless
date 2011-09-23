#ifndef __STATUS_BAR_H__
#define __STATUS_BAR_H__

#include "types.h"
#include "size_aware.h"
#include "brush.h"
#include "buffer.h"

class StatusBar : public SizeAware
{
public:
	StatusBar();
	~StatusBar();

	void redraw(Buffer::iterator& cursor);

private:
	Brush _brush;
};

#endif

