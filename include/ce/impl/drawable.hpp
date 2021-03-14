#pragma once

struct SDL_Texture;

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
		virtual SDL_Texture *get_texture() = 0;

	protected:
		drawable() = default;
	};
}
