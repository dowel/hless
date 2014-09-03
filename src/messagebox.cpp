#include <functional>

#include "messagebox.h"

static __attribute__((unused)) const char* MODULE_NAME = "messagebox";

MessageBox::MessageBox(std::string st)
	: _msg(st)
{
	update_terminal_size();

	InputProcessor::InputSequence tmp;

	_input.register_input_sequence({10}, KEY_HANDLER(MessageBox::on_enter));
}

void MessageBox::show()
{
	InterruptibleGuard ig(*this);

	u32 x = _maxx / 4;
	u32 y = _maxy / 2 - 2;
	u32 width = _maxx / 2;

	while (!_interrupted) {
		_border.set_geometry(x, y, width, 4);
		_border.draw(Brush::border_color);
		show_text(_msg, x + 1, y + 1, width - 2);
		show_ok(x + 1, y + 2, width - 2);
		refresh();
		_input.wait_for_input();
	}
}

void MessageBox::show(u32 timeout)
{

}

void MessageBox::show_text(std::string text, u32 x, u32 y, u32 width)
{
	u32 len = text.length();
	Line l1(' ', (width - len) / 2);
	Line l3(text);
	Line l2(' ', width - l1.length() - l3.length());
	Line res;

	res += l1;
	res += text;
	res += l2;

	_brush.draw_text(x, y, res, Brush::border_color);
}

void MessageBox::show_ok(u32 x, u32 y, u32 width)
{
	Line l1(' ', width / 2 - 1);
	Line l2("OK");
	Line res;

	res += l1;
	res += l2;
	res += l1;

	ColorRanges ranges;
	ColorRange before(0, width / 2 - 6, Brush::border_color);
	ColorRange ok(width / 2 - 6, 12, Brush::active_button_color);
	ColorRange after(width / 2 + 6, width / 2 - 6, Brush::border_color);

	ranges.push_back(&before);
	ranges.push_back(&ok);
	ranges.push_back(&after);

	_brush.draw_text(x, y, res, ranges);
}

MessageBox::Button MessageBox::show_ok_cancel()
{
	return OK_BUTTON;
}

void MessageBox::on_enter(char c)
{
	_interrupted = true;
}

