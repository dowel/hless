#ifndef __STATUS_BAR_H__
#define __STATUS_BAR_H__

#include "types.h"
#include "size_aware.h"
#include "brush.h"
#include "buffer.h"
#include "mark.h"

class StatusBar : public SizeAware
{
public:
	StatusBar(Marks& marks);
	~StatusBar();

	void redraw(Buffer::iterator& cursor);

	void draw_marks(Buffer::iterator& cursor);

private:
	Brush _brush;
	Marks& _marks;
};

#endif

