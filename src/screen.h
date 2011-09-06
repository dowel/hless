#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "buffer.h"
#include "brush.h"

class Screen
{
public:
	Screen(Buffer& buffer);
	~Screen();

	static const int status_bar_height;

	/**
	 * Redraws screen returning new bottom and line in it. 
	 *  
	 * @param current 
	 * @param line_in_current 
	 * @param cursor 
	 * @param bottom 
	 * @param line_in_bottom 
	 */
	void redraw_screen(Buffer::iterator& current, 
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
	void stage_redraw_screen(Buffer::iterator& current,
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
	void stage_reversed_redraw_screen(Buffer::iterator& bottom, 
		u32 line_in_bottom, 
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

	/**
	 * Redraws status bar.
	 */
	void update_status_bar();

private:
	u32 _maxx;
	u32 _maxy;
	u32 _width;
	u32 _text_height;
	Brush _brush;
	Buffer& _buffer;
	StatusBar _status_bar;
};

#endif

