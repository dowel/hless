#ifndef __MESSAGE_BOX_H__
#define __MESSAGE_BOX_H__

#include <string>

#include "size_aware.h"
#include "interruptible.h"
#include "border.h"
#include "brush.h"
#include "input.h"

class MessageBox : public SizeAware, public Interruptible
{
public:
	MessageBox(std::string);

	/**
	 * Will show the message box with OK button.
	 */
	void show();

	/**
	 * Will show the message box for specified period of time, without OK button.
	 * 
	 * @param timeout 
	 */
	void show(u32 timeout);

	enum Button {
		OK_BUTTON = 1,
		CANCEL_BUTTON
	};

	Button show_ok_cancel();

	void on_enter(char c);

private:
	void show_text(std::string text, u32 x, u32 y, u32 width);
	void show_ok(u32 x, u32 y, u32 width);

	std::string _msg;

	Border _border;
	Brush _brush;
	InputProcessor _input;
	bool _done;
};

#endif

