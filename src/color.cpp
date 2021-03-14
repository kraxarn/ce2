#include "ce/color.hpp"

#include <SDL.h>

using namespace ce;

color::color(uint8_t red, uint8_t green, uint8_t blue)
	: red(red),
	green(green),
	blue(blue)
{
}

color::color(uint16_t value)
	: red(value & 0xff0000),
	green(value & 0x00ff00),
	blue(value & 0x0000ff)
{
}

color::color()
	: color(0x0, 0x0, 0x0)
{
}

uint8_t color::get_red() const
{
	return red;
}

uint8_t color::get_green() const
{
	return green;
}

uint8_t color::get_blue() const
{
	return blue;
}

SDL_Color color::to_sdl_color() const
{
	return SDL_Color{
		red, green, blue, 0xff
	};
}
