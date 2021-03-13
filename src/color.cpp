#include "ce/color.hpp"

using namespace ce;

color::color(uint8_t red, uint8_t green, uint8_t blue)
	: red(red),
	green(green),
	blue(blue)
{
}

color::color(uint16_t value)
	: red(value & 0b110000),
	green(value & 0b001100),
	blue(value & 0b000011)
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
