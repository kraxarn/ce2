#pragma once

struct SDL_Texture;
struct SDL_Renderer;

namespace ce
{
	/**
	 * Something can can be drawn to the screen
	 */
	class drawable
	{
	public:
		/**
		 * Get drawable texture
		 */
		virtual SDL_Texture *get_texture(SDL_Renderer *renderer) = 0;

		/**
		 * Set a new position
		 */
		void set_position(int x, int y);

		/**
		 * Move relative to the current position
		 */
		void move(int x, int y);

		/**
		 * X position
		 */
		int x = 0;

		/**
		 * Y position
		 */
		int y = 0;

	protected:
		drawable() = default;
	};
}
