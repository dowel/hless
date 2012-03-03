#include "brush.h"
#include "log.h"

static __attribute__((unused)) const char* MODULE_NAME = "brush";

Color* Brush::text_color = 0;
Color* Brush::cursor_color = 0;
Color* Brush::status_bar_color = 0;
Color* Brush::border_color = 0;
Color* Brush::progress_done_color = 0;
Color* Brush::progress_remains_color = 0;
Color* Brush::active_button_color = 0;

u32 Color::_last_pair = 1;

void Brush::init_colors()
{
	Brush::text_color = new Color("text color");
	Brush::cursor_color = new Color("cursor color", Color::BLACK, Color::YELLOW);
	Brush::status_bar_color = new Color("status bar color", Color::BLACK, Color::WHITE);
	Brush::border_color = new Color("border color", Color::YELLOW, Color::BLUE);
	Brush::progress_done_color = new Color("progress bar color", Color::BLACK, Color::YELLOW);
	Brush::progress_remains_color = new Color("progress bar color", Color::WHITE, Color::BLACK);
	Brush::active_button_color = new Color("button color", Color::BLACK, Color::YELLOW);
}

Color::Color(std::string name)
	: _name(name)
	, _fg(Color::NO_COLOR)
	, _bg(Color::NO_COLOR)
{
	_pair = 0;
}

Color::Color(std::string name, Color::ColorValue foreground, Color::ColorValue background)
	: _name(name)
	, _fg(foreground)
	, _bg(background)
{
	_pair = _last_pair++;
	init_pair(_pair, _fg, _bg);
}

Color::~Color()
{

}

void Color::seton()
{
	attron(COLOR_PAIR(_pair));
}

void Color::setoff()
{
	attroff(COLOR_PAIR(_pair));
}

void Color::seton(WINDOW* window)
{
	wattron(window, COLOR_PAIR(_pair));
}

void Color::setoff(WINDOW* window)
{
	wattroff(window, COLOR_PAIR(_pair));
}

void Brush::draw_text(u32 x, u32 y, Line& line, Color* color)
{
	Log2("Drawing text at " << x << "/" << y << " with " << color);
	Log3("The text is: " << line.get(64) << "...");
	color->seton();
	mvaddstr(y, x, line.get());
	color->setoff();
}

void Brush::draw_line(u32 y, Line& line, Color* color)
{
	draw_text(0, y, line, color);
}

void Brush::draw_text(u32 x, u32 y, Line& line, ColorRanges& ranges)
{
	ColorRanges::iterator it;
	u32 pos = 0;
	for (it = ranges.begin(); it != ranges.end(); it++) {
		ColorRange* range = *it;
		if (range->get_start() < pos) {
			Line sub = line.substr(pos, pos - range->get_start());
			draw_text(x + pos, y, sub);
			pos = range->get_start();
		}

		Line sub = line.substr(pos, range->get_length());
		draw_text(x + pos, y, sub, range->get_color());
		pos += range->get_length();
	}

	// Now drawing the tail that could have remained...
	Line sub = line.substr(pos, line.length() - pos);
	draw_text(x + pos, y, sub);
}

void Brush::draw_line(u32 y, Line& line, ColorRanges& ranges)
{
	draw_text(0, y, line, ranges);
}

std::ostream& operator<<(std::ostream& os, Color* color)
{
	os << "[Color: " << color->name() << "]";
	return os;
}

