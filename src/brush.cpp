#include "brush.h"
#include "log.h"

static __attribute__((unused)) const char* MODULE_NAME = "brush";

Brush::Color* Brush::text_color = 0;
Brush::Color* Brush::cursor_color = 0;

u32 Brush::Color::_last_pair = 0;

void Brush::init_colors()
{
	Brush::text_color = new Brush::Color("text color", Brush::Color::WHITE, Brush::Color::BLACK);
	Brush::cursor_color = new Brush::Color("cursor color", Brush::Color::BLACK, Brush::Color::YELLOW);
}

Brush::Color::Color(std::string name, Brush::Color::ColorValue foreground, Brush::Color::ColorValue background)
	: _name(name)
	, _fg(foreground)
	, _bg(background)
{
	_last_pair++;
	_pair = _last_pair;
	init_pair(_last_pair, _fg, _bg);
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
	Log1("Asked to draw text at " << x << "/" << y << " with " << color);
	Log2("The text is: " << line.get_text(64));
	color->seton();
	mvaddstr(y, x, line.get_text());
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

