#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "size_aware.h"
#include "buffer.h"
#include "brush.h"
#include "status_bar.h"

class Screen : public SizeAware
{
public:
	Screen(Buffer& buffer);
	~Screen();

	/**
	 * Redraws screen returning new bottom and line in it. 
	 *  
	 * @param current 
	 * @param line_in_current 
	 * @param cursor 
	 * @param bottom 
	 * @param line_in_bottom 
	 */
	void redraw(Buffer::iterator& current, 
		u32 line_in_current,
		Buffer::iterator& cursor,
		Buffer::iterator& bottom,
		u32& line_in_bottom);

	/**
	 * Stages screen redrawing without actually doing anything. current and line_in_current expected to 
	 * be lines where to start from. bottom and line_in_bottom filled with bottom line after redrawing.
	 * 
	 * @param current 
	 * @param line_in_current 
	 * @param bottom 
	 * @param line_in_bottom 
	 */
	void stage_redraw(Buffer::iterator& current,
		u32 line_in_current,
		Buffer::iterator& bottom,
		u32& line_in_bottom);

	/**
	 * Stages screen redrawing without actually doing anything. Unlike stage_redraw_screen(), this 
	 * version redraws from bottom up. bottom and line_in_bottom expected to be where screen ends. 
	 * current and line_in_current set to the top of the screen. 
	 * 
	 * @param bottom 
	 * @param line_in_bottom 
	 * @param current 
	 * @param line_in_current 
	 */
	void stage_reversed_redraw(Buffer::iterator& bottom, 
		u32 line_in_bottom, 
		Buffer::iterator& current, 
		u32& line_in_current);

	/**
	 * Stages screen redrawing without actually doing anything. This version redraws from the middle, 
	 * assuming cursor is right at the middle. Once done, it sets current, line_in_current, bottom and 
	 * line_in_button accordingly. 
	 *  
	 * @param cursor 
	 * @param current 
	 * @param line_in_current 
	 * @param bottom 
	 * @param line_in_bottom 
	 */
	void stage_redraw_from_middle(Buffer::iterator cursor, 
	   Buffer::iterator& current,
	   u32& line_in_current);

	/**
	 * Reads line from buffer and splits it into list of smaller lines according to width of the screen.
	 * 
	 * @param it - iterator pointing to the line to read.
	 * @param res - list that returns after the split.
	 */
	void read_and_split(Buffer::iterator& it, LineList& res);

	/**
	 * Returns width of the screen.
	 * 
	 * @return u32 
	 */
	u32 get_width() { return _width; }

private:
	Brush _brush;
	Buffer& _buffer;
};

#endif

