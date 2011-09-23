#include "brush.h"
#include "log.h"

static __attribute__((unused)) const char* MODULE_NAME = "brush";

Brush::Color* Brush::text_color = 0;
Brush::Color* Brush::cursor_color = 0;
Brush::Color* Brush::status_bar_color = 0;

u32 Brush::Color::_last_pair = 1;

void Brush::init_colors()
{
	Brush::text_color = new Brush::Color("text color");
	Brush::cursor_color = new Brush::Color("cursor color", Brush::Color::BLACK, Brush::Color::YELLOW);
	Brush::status_bar_color = new Brush::Color("status bar color", Brush::Color::BLACK, Brush::Color::WHITE);
}

Brush::Color::Color(std::string name)
	: _name(name)
	, _fg(Brush::Color::NO_COLOR)
	, _bg(Brush::Color::NO_COLOR)
{
	_pair = 0;
}

Brush::Color::Color(std::string name, Brush::Color::ColorValue foreground, Brush::Color::ColorValue background)
	: _name(name)
	, _fg(foreground)
	, _bg(background)
{
	_pair = _last_pair++;
	init_pair(_pair, _fg, _bg);
}

Brush::Color::~Color()
{

}

void Brush::Color::seton()
{
	attron(COLOR_PAIR(_pair));
}

void Brush::Color::setoff()
{
	attroff(COLOR_PAIR(_pair));
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

std::ostream& operator<<(std::ostream& os, Brush::Color* color)
{
	os << "[Color: " << color->name() << "]";
	return os;
}

