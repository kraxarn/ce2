#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "ce/color.hpp"

struct TTF_Font;
struct SDL_Surface;

namespace ce
{
	/**
	 * Font that can be used with text
	 */
	class font
	{
	public:
		/**
		 * Load font from data
		 * @param data Data to load
		 * @param size Point size
		 */
		explicit font(const std::vector<uint8_t> &data, int size);

		/**
		 * Destroy the font and free resources
		 * @note Texts using the font can still be used, but not updated
		 */
		~font();

		/**
		 * Render text with font
		 * @returns Rendered text, or nullptr on failure
		 * @note Target is responsible for freeing the memory allocated
		 */
		SDL_Surface *render_text(const std::string &text, const color &color) const;

	private:
		TTF_Font *ttf_font = nullptr;
	};
}
