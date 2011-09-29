#ifndef __EDITBOX_H__
#define __EDITBOX_H__

#include <string>
#include <boost/shared_array.hpp>

#include "brush.h"
#include "size_aware.h"
#include "input.h"

class Editbox : public SizeAware
{
public:
	Editbox(std::string prompt);

	std::string run();

	void on_printable_input(char c);
	void on_enter(char c);
	void on_backspace(char c);
	void on_left(char c);
	void on_right(char c);

private:
	std::string _prompt;
	Brush _brush;
	std::string _text;
	u32 _text_width;
	InputProcessor _input;
	u32 _cursor; // position of the cursor relative to the beginning of the line.
	u32 _window; // position in the buffer where visible part of the edited line begins.
	bool _done;
};

#endif

