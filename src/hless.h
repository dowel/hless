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

	/**
	 * This is the main routine.
	 */
	void run();

	/**
	 * Reads line from buffer and splits it into list of smaller lines according to width of the screen.
	 * 
	 * @param it 
	 * @param res 
	 */
	void read_and_split(Buffer::iterator& it, LineList& res);
	/**
	 * Finds closest line to specified offset in the buffer.
	 * 
	 * @param offset 
	 * @param it 
	 */
	void closest_line(u64 offset, Buffer::iterator& it);

	/**
	 * These are various key handlers.
	 */
	void on_quit_key();
	void on_down_key();
	void on_up_key();
	void on_next_page();

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

