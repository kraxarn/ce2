#pragma once

#include <vector>
#include <cstdint>

struct TTF_Font;

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
		 * @note Does not free any texts that may still use the font
		 */
		~font();

	private:
		TTF_Font *ttf_font = nullptr;
	};
}
