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

	_current = _buffer.begin();
	_cursor = _current;

	while (!_done) {
		Log2("===================================================================================================");
		_screen.redraw_screen(_current, _line_in_current, _cursor, _bottom, _line_in_bottom);
		Log2("---------------------------------------------------------------------------------------------------");
		_input.wait_for_input();

		Log1("After processing input:");
		Log1("Cursor at " << _cursor);
		Log1("Current at " << _current << ", line in current " << _line_in_current);
		Log1("Bottom at " << _bottom << ", line in bottom " << _line_in_bottom);
	}
}

void Hless::on_quit_key()
{
	_done = true;
}

void Hless::on_down_key()
{
	if (_cursor != _bottom) {
		Log2("Cursor is not at the bottom of the screen...");
		_cursor++;
		return;
	}

	Log2("Cursor is at the bottom...");

	Line line;
	_buffer.read_line(_bottom, line);
	line.strip_back();

	LineList lst;
	LineList::iterator it;

	line.split_lines(_screen.get_width(), lst);

	Log2("There are " << lst.size() << " lines in the bottom line. Last visible line in bottom is " << 
		 _line_in_bottom);

	if (lst.size() == _line_in_bottom + 1) {
		_current++;
		_line_in_current = 0;
	} else {
		_line_in_current++;
	}

	if (lst.size() == _line_in_bottom + 1) {
		_current++;
		_line_in_current = 0;
	}
}

void Hless::on_up_key()
{
	_cursor--;
}

