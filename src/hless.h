#ifndef __HLESS_H__
#define __HLESS_H__

#include "screen.h"
#include "buffer.h"
#include "input.h"

class Hless
{
public:
	Hless(Buffer& buffer);
	~Hless();

	/**
	 * This is the main routine.
	 */
	void run();

	/**
	 * Finds closest line to specified offset in the buffer.
	 * 
	 * @param offset - offset we're looking for.
	 * @param before_hint - iterator pointing to line somewhere before the offset.
	 * @param after_hint - iterator pointing to line somewhere after the offset. 
	 *  
	 * @return Buffer::iterator - iterator pointing to the closest line to the offset. 
	 */
	Buffer::iterator closest_line(u64 offset, Buffer::iterator& before_hint, Buffer::iterator& after_hint);

	/**
	 * These are various key handlers.
	 */
	void on_quit_key();
	void on_down_key();
	void on_up_key();
	void on_next_page();
	void on_prev_page();
	void on_goto_end();
	void on_goto();

private:
	Buffer& _buffer;

	Screen _screen;
	StatusBar _status_bar;

	InputProcessor _input;
    Buffer::iterator _current;
	u32 _line_in_current;
    Buffer::iterator _cursor;
	Buffer::iterator _bottom;
	u32 _line_in_bottom;

	bool _done;
};

#endif

