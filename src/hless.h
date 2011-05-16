#ifndef __HLESS_H__
#define __HLESS_H__

#include "screen.h"
#include "buffer.h"
#include "input.h"

class Hless
{
public:
	Hless(Screen& screen, Buffer& buffer);
	~Hless();

	void run();

	void on_quit_key();
	void on_down_key();
	void on_up_key();

private:
	Screen& _screen;
	Buffer& _buffer;

	InputProcessor _input;
    Buffer::iterator _current;
	u32 _line_in_current;
    Buffer::iterator _cursor;
	Buffer::iterator _bottom;
	u32 _line_in_bottom;

	bool _done;
};

#endif

