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

	protected:
		drawable() = default;
	};
}
