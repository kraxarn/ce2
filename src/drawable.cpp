#include "ce/impl/drawable.hpp"

using namespace ce;

void drawable::set_position(int x_pos, int y_pos)
{
	x = x_pos;
	y = y_pos;
}

void drawable::move(int x_pos, int y_pos)
{
	x += x_pos;
	y += y_pos;
}
