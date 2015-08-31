#include <algorithm>

#include "editbox.h"

static __attribute__((unused)) const char* MODULE_NAME = "editbox"; 

Editbox::Editbox(std::string prompt)
	: _original_prompt(prompt)
	, _cursor(0)
	, _window(0)
	, _done(false)
	, _mode(_modes.end())
{
	init();
}

Editbox::Editbox(std::string prompt, ModesList modes)
	: _original_prompt(prompt)
	, _cursor(0)
	, _window(0)
	, _done(false)
	, _modes(modes)
	, _mode(_modes.begin())
{
	init();
}

void Editbox::init()
{
	update_terminal_size();

	InputProcessor::InputSequence tmp;
	for (char a = ' '; a <= '~'; a++) { // this includes most of the printable characters...
		_input.register_input_sequence({a}, KEY_HANDLER(Editbox::on_printable_input));
	}

	_input.register_input_sequence({'\n'}, KEY_HANDLER(Editbox::on_enter));
	_input.register_input_sequence({KEY_BACKSPACE}, KEY_HANDLER(Editbox::on_backspace));
	_input.register_input_sequence({127}, KEY_HANDLER(Editbox::on_backspace));
	_input.register_input_sequence({KEY_DC}, KEY_HANDLER(Editbox::on_delete));
	_input.register_input_sequence({KEY_LEFT}, KEY_HANDLER(Editbox::on_left));
	_input.register_input_sequence({KEY_RIGHT}, KEY_HANDLER(Editbox::on_right));
	_input.register_input_sequence({KEY_HOME}, KEY_HANDLER(Editbox::on_home));
	_input.register_input_sequence({KEY_END}, KEY_HANDLER(Editbox::on_end));
	_input.register_input_sequence({'\t'}, KEY_HANDLER(Editbox::on_tab));

	recalc_prompt();
}

void Editbox::recalc_prompt()
{
	_prompt = _original_prompt;
	if (_modes.empty()) {
		_prompt += ": ";
	} else {
		_prompt += " (";
		_prompt += *_mode;
		_prompt += "): ";
	}
}

struct CursorVisibilityGuard
{
	CursorVisibilityGuard() { curs_set(1); }
	~CursorVisibilityGuard() { curs_set(0); }
};

std::string Editbox::run()
{
	CursorVisibilityGuard curs_guard;
	InterruptibleGuard ctrl_c(*this);

	while (!_done && !_interrupted) {
		update_terminal_size();
		recalc_prompt();
		_text_width = _width - _prompt.length();

		std::stringstream ss;
		ss << _prompt;

		Log2("Prompt: " << _prompt << ", window: " << _window << ", text_width: " << _text_width << 
			 ", text length: " << _text.length());

 		std::string substr = _text.substr(_window, std::min(_text_width, u32(_text.length() - _window)));
		ss << substr;

		Log2("Substring of text: " << substr);

		Line line(ss.str());

		Log2("Before appending spaces, line is: " << line);

		if (line.length() < _width) {
			Line rest(' ', _width - line.length());
			line += rest;
		}

		Log1("Current editbox line: " << line);
		if (LogLevelEnabled(2)) {
			Line temp_line(_text);
			Log2("Entire editbox line: " << temp_line);
			Log2("Cursor at " << _cursor << ", window at " << _window << ", text width " << _text_width);
		}

		_brush.draw_line(_text_height + 1, line);

		move(_text_height + 1, _cursor - _window + _prompt.length());
		refresh();

		_input.wait_for_input();
	}

	return std::string(_text);
}

void Editbox::on_printable_input(char c)
{
	char s[2];
	s[0] = c;
	s[1] = 0;
	_text.insert(_cursor, s);
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

void Editbox::on_delete(char c)
{
	if (_cursor == _text.length()) {
		return;
	}

	if (_cursor == 0) {
		_text = _text.substr(1);
	} else {
		std::string part1 = _text.substr(0, _cursor);
		std::string part2 = _text.substr(_cursor + 1);

		_text = part1 + part2;
	}
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
	if (_cursor - _window >= _text_width) {
		_window++;
	}
}

void Editbox::on_home(char c)
{
	_cursor = 0;
	_window = 0;
}

void Editbox::on_end(char c)
{
	_cursor = _text.length();
	_window = _text.length() - _text_width + 1;
}

void Editbox::on_tab(char c)
{
	_mode++;
	if (_mode == _modes.end()) {
		_mode = _modes.begin();
	}
}

