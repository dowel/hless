#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>

#include "hless.h"
#include "log.h"

#define KEY_HANDLER(x) boost::function<void ()>(boost::bind(&x, this))

static __attribute__((unused)) const char* MODULE_NAME = "hless";

Hless::Hless(Screen& screen, Buffer& buffer)
	: _screen(screen)
	, _buffer(buffer)
	, _line_in_current(0)
	, _line_in_bottom(0)
	, _done(false)
{
}

Hless::~Hless()
{

}

void Hless::run()
{
	InputProcessor::InputSequence tmp;

	tmp = boost::assign::list_of('q');
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_quit_key));
	tmp = boost::assign::list_of(KEY_DOWN);
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_down_key));
	tmp = boost::assign::list_of(KEY_UP);
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_up_key));
	tmp = boost::assign::list_of(KEY_NPAGE);
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_next_page));
//  tmp = boost::assign::list_of(KEY_PPAGE);
//  _input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_up_key));

	_current = _buffer.begin();
	_cursor = _current;

	while (!_done) {
		Log2("===================================================================================================");
		_screen.redraw_screen(_current, _line_in_current, _cursor, _bottom, _line_in_bottom);

		Log1("After redrawing screen:");
		Log1("Cursor at " << _cursor);
		Log1("Current at " << _current << ", line in current " << _line_in_current);
		Log1("Bottom at " << _bottom << ", line in bottom " << _line_in_bottom);

		Log2("---------------------------------------------------------------------------------------------------");
		_input.wait_for_input();

		Log1("After processing input:");
		Log1("Cursor at " << _cursor);
		Log1("Current at " << _current << ", line in current " << _line_in_current);
		Log1("Bottom at " << _bottom << ", line in bottom " << _line_in_bottom);
	}
}

void Hless::read_and_split(Buffer::iterator& it, LineList& res)
{
	Line line;
	_buffer.read_line(it, line);
	line.strip_back();
	res.clear();
	line.split_lines(_screen.get_width(), res);	
}

Buffer::iterator Hless::closest_line(u64 offset, Buffer::iterator& before_hint, Buffer::iterator& after_hint)
{
	Buffer::iterator temp, old_temp;
	temp = before_hint;
	old_temp = Buffer::end();

	while ((temp != after_hint) && (temp->get_offset() < offset)) {
		old_temp = temp;
		temp++;
	}

	if (old_temp == Buffer::end()) {
		return before_hint;
	} else {
		u64 distance = temp->get_offset() - offset;
		if (distance > (old_temp->get_length() / 2)) {
			return temp;
		} else {
			return old_temp;
		}
	}
}

void Hless::on_quit_key()
{
	_done = true;
}

void Hless::on_down_key()
{
	// This the simplest case - cursor is not at the bottom line. Moving cursor
	// one line down and returning.
	if (_cursor != _bottom) {
		Log2("Cursor is not at the bottom of the screen...");
		_cursor++;
		return;
	}

	Log2("Cursor is at the bottom...");

	Buffer::iterator temp = _bottom;
	temp++;
	if (temp == Buffer::end()) {
		Log2("Cursor is at the bottom of the input file...");
		return;
	}

	// Cursor at the bottom. Here, depending on height of the bottom line we either
	// move the cursor one line down or not. If we're at the last line in the bottom
	// line, then we move cursor one line further. Otherwise, cursor stays where it is
	// and we're moving the top of the screen.
	LineList lst;

	read_and_split(_bottom, lst);

	Log2("There are " << lst.size() << " lines in the bottom line. Last visible line in bottom is " << 
		 _line_in_bottom);

	if (_line_in_bottom == lst.size() - 1) {
		_cursor++;
	}

	read_and_split(_current, lst);
	_line_in_current++;
	if (_line_in_current >= lst.size()) {
		_current++;
		_line_in_current = 0;
	}
}

void Hless::on_up_key()
{
	if (_cursor == _current) {
		Log2("Cursor is at the top of the screen...");

		Buffer::iterator temp = _cursor;
		temp--;
		if (temp == Buffer::end()) {
			Log2("Cursor is at the top of the input file...");
			return;
		}

		if (_line_in_current > 0) {
			_line_in_current--;
		} else {
			if (_current->get_offset() != 0) {
				_current--;

				LineList lst;
				read_and_split(_current, lst);

				_line_in_current = lst.size() - 1;

				_cursor--;
			}
		}
	} else {
		Log2("Cursor is not at the top of the screen...");
		_cursor--;
	}
}

void Hless::on_next_page()
{
	Buffer::iterator old_current = _current;

	// First moving current...
	_current = _bottom;
	_line_in_current = 0;

	// Now calculating location of the cursor...
	Buffer::iterator new_bottom;
	u32 line_in_new_bottom;

	_screen.stage_redraw_screen(_current, _line_in_current, new_bottom, line_in_new_bottom);
	Log1("New bottom expected to be at " << new_bottom << ", in line " << line_in_new_bottom);

	if (new_bottom == _current) {
		_current = old_current;
		_cursor = _bottom;
		return;
	}

	// Now calculating location of the cursor. We want the cursor to remain at relatively the same
	// position as before. For example if it was on third line from the top, we want it to remain on 
	// the third line from the top. 
	u64 cursor_offset = _cursor->get_offset() - old_current->get_offset();
	_cursor = closest_line(_current->get_offset() + cursor_offset, _current, new_bottom);
	if (_cursor->get_offset() > new_bottom->get_offset()) {
		_cursor = new_bottom;
	}
}

