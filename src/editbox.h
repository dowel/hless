#ifndef __EDITBOX_H__
#define __EDITBOX_H__

#include <string>

#include "brush.h"
#include "size_aware.h"
#include "input.h"
#include "interruptible.h"

class Editbox : public SizeAware, public Interruptible
{
public:
	typedef std::list<std::string> ModesList;

	Editbox(std::string prompt);
	Editbox(std::string prompt, ModesList modes);

	void init();
	void recalc_prompt();

	std::string run();

	void on_printable_input(char c);
	void on_enter(char c);
	void on_backspace(char c);
	void on_delete(char c);
	void on_left(char c);
	void on_right(char c);
	void on_home(char c);
	void on_end(char c);
	void on_tab(char c);

	std::string get_mode() { return *_mode; }

private:
	std::string _original_prompt;
	std::string _prompt;
	Brush _brush;
	std::string _text;
	u32 _text_width;
	InputProcessor _input;
	u32 _cursor; // position of the cursor relative to the beginning of the line.
	u32 _window; // position in the buffer where visible part of the edited line begins.
	bool _done;
	ModesList _modes;
	ModesList::iterator _mode;
};

#endif

