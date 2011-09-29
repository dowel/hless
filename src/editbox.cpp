#include <boost/assign/list_of.hpp>
#include <algorithm>

#include "editbox.h"

static __attribute__((unused)) const char* MODULE_NAME = "editbox"; 

Editbox::Editbox(std::string prompt)
	: _prompt(prompt)
	, _cursor(0)
	, _window(0)
	, _done(false)
{
	update_terminal_size();

	InputProcessor::InputSequence tmp;
	for (char a = ' '; a <= '~'; a++) { // this includes most of the printable characters...
		tmp = boost::assign::list_of(a);
		_input.register_input_sequence(tmp, KEY_HANDLER(Editbox::on_printable_input));
	}

	tmp = boost::assign::list_of(10);
	_input.register_input_sequence(tmp, KEY_HANDLER(Editbox::on_enter));
	tmp = boost::assign::list_of(KEY_BACKSPACE);
	_input.register_input_sequence(tmp, KEY_HANDLER(Editbox::on_backspace));
	tmp = boost::assign::list_of(KEY_LEFT);
	_input.register_input_sequence(tmp, KEY_HANDLER(Editbox::on_left));
	tmp = boost::assign::list_of(KEY_RIGHT);
	_input.register_input_sequence(tmp, KEY_HANDLER(Editbox::on_right));

	_text_width = _width - prompt.length() - 2;
}

struct CursorVisibilityGuard
{
	CursorVisibilityGuard() { curs_set(1); }
	~CursorVisibilityGuard() { curs_set(0); }
};

std::string Editbox::run()
{
	CursorVisibilityGuard curs_guard;

	while (!_done) {
		update_terminal_size();

		std::stringstream ss;
		ss << _prompt << ": ";

		std::string substr = _text.substr(_window, std::min(_text_width, u32(_text.length() - _window)));
		ss << substr;

		Line line(ss.str());

		if (line.length() < _width) {
			Line rest(' ', _width - line.length());
			line += rest;
		}

		Log1("Current editbox line: " << line);

		_brush.draw_line(_text_height + 1, line);

		move(_text_height + 1, _cursor - _window + _prompt.length() + 2);
		refresh();

		_input.wait_for_input();
	}

	return std::string(_text);
}

void Editbox::on_printable_input(char c)
{
	_text.insert(_cursor, &c);
	_cursor++;
	if (_cursor >= _text_width) {
		_window++;
	}
}

void Editbox::on_enter(char c)
{
	_done = true;
}

void Editbox::on_backspace(char c)
{
	if (_cursor == 0) {
		return;
	}
	if (_cursor == _text.length()) {
		_text = _text.substr(0, _cursor - 1);
	} else {
		std::string part1 = _text.substr(0, _cursor);
		std::string part2 = _text.substr(_cursor + 1);

		_text = part1 + part2;
	}

	_cursor--;
}

void Editbox::on_left(char c)
{
	if (_cursor == 0) {
		return;
	}

	_cursor--;
	if (_cursor < _window) {
		_window--;
	}
}

void Editbox::on_right(char c)
{
	if (_cursor == _text.length()) {
		return;
	}

	_cursor++;
	if (_cursor - _window > _text_width) {
		_window++;
	}
}

