#include <iostream>

#include "status_bar.h"

StatusBar::StatusBar()
{

}

StatusBar::~StatusBar()
{

}

voi StatusBar::update_screen_size(u32 width, u32 height)
{
	_width = width;
	_height = height;
}
