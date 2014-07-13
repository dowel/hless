#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "hless.h"
#include "log.h"
#include "editbox.h"
#include "border.h"
#include "progress.h"
#include "messagebox.h"

static __attribute__((unused)) const char* MODULE_NAME = "hless";

Hless::Hless(Buffer& buffer)
	: _buffer(buffer)
	, _screen(buffer)
	, _status_bar()
	, _current(&buffer)
	, _line_in_current(0)
	, _cursor(&buffer)
	, _bottom(&buffer)
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
	tmp = boost::assign::list_of(KEY_PPAGE);
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_prev_page));
	tmp = boost::assign::list_of('G');
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_goto_end));
	tmp = boost::assign::list_of(':');
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_goto));
	tmp = boost::assign::list_of('g');
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_goto_beginning));

	tmp = boost::assign::list_of('D');
	_input.register_input_sequence(tmp, KEY_HANDLER(Hless::on_debug));

	_current = _buffer.begin();
	_cursor = _current;

	while (!_done) {
		Log1("===================================================================================================");
		_screen.redraw(_current, _line_in_current, _cursor, _bottom, _line_in_bottom);

		Log1("After redrawing screen:");
		Log1("Cursor at " << _cursor);
		Log1("Current at " << _current << ", line in current " << _line_in_current);
		Log1("Bottom at " << _bottom << ", line in bottom " << _line_in_bottom);

		_status_bar.redraw(_cursor);

		Log1("---------------------------------------------------------------------------------------------------");
		_input.wait_for_input();

		Log1("After processing input:");
		Log1("Cursor at " << _cursor);
		Log1("Current at " << _current << ", line in current " << _line_in_current);
		Log1("Bottom at " << _bottom << ", line in bottom " << _line_in_bottom);
	}
}

Buffer::iterator Hless::closest_line(u64 offset, Buffer::iterator& before_hint, Buffer::iterator& after_hint)
{
	Buffer::iterator temp(&_buffer), old_temp(&_buffer);
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

void Hless::on_quit_key(char c)
{
	_done = true;
}

void Hless::on_down_key(char c)
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

	_screen.read_and_split(_bottom, lst);

	Log2("There are " << lst.size() << " lines in the bottom line. Last visible line in bottom is " << 
		 _line_in_bottom);

	if (_line_in_bottom == lst.size() - 1) {
		_cursor++;
	}

	_screen.read_and_split(_current, lst);
	_line_in_current++;
	if (_line_in_current >= lst.size()) {
		_current++;
		_line_in_current = 0;
	}
}

void Hless::on_up_key(char c)
{
	if (_cursor == _current) {
		Log2("Cursor is at the top of the screen...");

		if (_line_in_current > 0) {
			_line_in_current--;
		} else {
			Buffer::iterator temp = _cursor;
			temp--;
			if (temp == Buffer::end()) {
				Log2("Cursor is at the top of the input file...");
				return;
			}

			if (_current->get_offset() != 0) {
				_current--;

				LineList lst;
				_screen.read_and_split(_current, lst);

				_line_in_current = lst.size() - 1;

				_cursor--;
			}
		}
	} else {
		Log2("Cursor is not at the top of the screen...");
		_cursor--;
	}
}

void Hless::on_next_page(char c)
{
	Buffer::iterator old_current = _current;

	// First moving current...
	_current = _bottom;
	_line_in_current = 0;

	// Now calculating location of the cursor...
	Buffer::iterator new_bottom(&_buffer);
	u32 line_in_new_bottom;

	_screen.stage_redraw(_current, _line_in_current, new_bottom, line_in_new_bottom);
	Log1("New bottom expected to be at " << new_bottom << "@" << line_in_new_bottom);

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

void Hless::on_prev_page(char c)
{
	// Let's say we're already at the beginning of the file. Moving cursor to the top and bailing out.
	if (_current == _buffer.begin()) {
		_line_in_current = 0;
		_cursor = _current;
		return;
	}

	// Calculating new current...
	Buffer::iterator old_current = _current;
	_bottom = _current;
	_line_in_bottom = 0;
	_screen.stage_reversed_redraw(_bottom, _line_in_bottom, _current, _line_in_current);
	Log1("New current expected to be at " << _current << "@" << _line_in_current);

	// Now moving the cursor...
	u32 offset = old_current->get_offset() - _current->get_offset();
	_cursor = closest_line(_cursor->get_offset() - offset, _current, _bottom);
}

void Hless::on_goto_end(char c)
{
	_bottom = _buffer.back();
	LineList lst;
	_screen.read_and_split(_bottom, lst);
	_line_in_bottom = lst.size() - 1;

	_screen.stage_reversed_redraw(_bottom, _line_in_bottom, _current, _line_in_current);
	_cursor = _bottom;
}

void Hless::on_goto(char c)
{
	Editbox::ModesList modes = boost::assign::list_of(std::string("line"))(std::string("offset"));
	Editbox ed("Go to", modes);
	std::string spec = ed.run();
	Log1("Asked to go to " << spec << " in mode " << ed.get_mode());

	// This is the case when we have only number in search box.
	const boost::regex plain_number("^\\d+$");
	if (boost::regex_match(spec, plain_number)) {
		if (spec == "offset") {

		} else {
			u32 line = boost::lexical_cast<u32>(spec);
			_cursor = _buffer.begin();
			_cursor += line;
		}
		Log1("Set new cursor posisition to " << _cursor);
	}

	_screen.stage_redraw_from_middle(_cursor, _current, _line_in_current);
}

class SampleProgressing : public Progressing
{
public:
	SampleProgressing() : _progress(0) { }

	virtual float progress() {
		return float(_progress);
	}

	virtual void run() {
		while (true) {
			Log1("Setting progress to " << _progress);
			_progress++;
			usleep(500000);
		}
	}

private:
	u32 _progress;
};

void Hless::on_debug(char c)
{
	MessageBox msg(std::string("hello world... lets talk about love love love..."));
	msg.show();
}

void Hless::on_goto_beginning(char c)
{
	_cursor = _buffer.begin();
	_current = _cursor;
	_line_in_current = 0;
}

