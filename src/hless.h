#ifndef __HLESS_H__
#define __HLESS_H__

#include <set>

#include "screen.h"
#include "buffer.h"
#include "input.h"
#include "mark.h"

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
	void on_quit_key(char c);
	void on_down_key(char c);
	void on_up_key(char c);
	void on_next_page(char c);
	void on_prev_page(char c);
	void on_goto_end(char c);
	void on_goto(char c);
	void on_goto_beginning(char c);
	void on_mark(char c);
	void on_next_mark(char c);
	void on_prev_mark(char c);

	void on_debug(char c);

private:
	Buffer& _buffer;

	Marks _marks;

	Screen _screen;
	StatusBar _status_bar;

	InputProcessor _input;

	// _current points to the top most line in the currently visible piece of buffer. Line of text it
	// points to can span accross multiple lines on the screen. _line_in_current indicates screen line
	// in the current buffer line of text that is visible. _cursor always points to buffer line where
	// the cursor is. _bottom points to the bottom most visible buffer line on the screen.
	// _line_in_bottom points to the last visible screen line in bottom buffer line that is visible.
    Buffer::iterator _current;
	u32 _line_in_current;
    Buffer::iterator _cursor;
	Buffer::iterator _bottom;
	u32 _line_in_bottom;

	bool _done;
};

#endif

