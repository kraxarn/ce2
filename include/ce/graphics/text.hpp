#pragma once

#include <string>

#include "ce/impl/drawable.hpp"
#include "ce/font.hpp"
#include "ce/color.hpp"

struct SDL_Surface;
struct SDL_Texture;

namespace ce
{
	/**
	 * Text that can be drawn to the screen
	 */
	class text: public drawable
	{
	public:
		/**
		 * Create new text from a font
		 * @param font Font to use
		 */
		explicit text(const font &font);

		/**
		 * Destroy text and free all resources
		 * @note Does not destroy the font
		 */
		~text();

		/**
		 * Change text to show
		 * @param text Text
		 */
		void set_text(const std::string &text);

		/**
		 * Change color of text
		 * @param color Color
		 */
		void set_color(const color &color);

		/**
		 * Width of text
		 * @returns Width, or -1 on failure
		 */
		int width() const;

		/**
		 * Height of text
		 * @returns Height, or -1 on failure
		 */
		int height() const;

		SDL_Texture *get_texture(SDL_Renderer *renderer) override;

	private:
		const font &text_font;
		std::string render_text;
		color text_color;
		SDL_Surface *text_surface = nullptr;
		SDL_Texture *texture = nullptr;

		void update_text_surface();
	};
}
