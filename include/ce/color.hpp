#pragma once

#include <cstdint>

struct SDL_Color;

namespace ce
{
	class color
	{
	public:
		/**
		 * Construct a new color
		 * @param red Red, 0-255
		 * @param green Green 0-255
		 * @param blue Blue 0-255
		 */
		color(uint8_t red, uint8_t green, uint8_t blue);

		/**
		 * Construct a new color
		 * @param value Color as rr gg bb
		 */
		explicit color(uint16_t value);

		/**
		 * Construct a new black color
		 */
		color();

		/**
		 * Get the red component
		 */
		uint8_t get_red() const;

		/**
		 * Get the green component
		 */
		uint8_t get_green() const;

		/**
		 * Get the blue component
		 */
		uint8_t get_blue() const;

		/**
		 * Convert to SDL color
		 */
		SDL_Color to_sdl_color() const;

	private:
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};
}
