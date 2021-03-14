#pragma once

#include <iostream>

#include "ce/impl/drawable.hpp"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace ce
{
	/**
	 * Application window
	 */
	class window
	{
	public:
		/**
		 * Create and show a new window
		 * @param title Window title
		 * @param x X position
		 * @param y Y position
		 * @param w Width
		 * @param h Height
		 */
		window(const std::string &title, int x, int y, int w, int h);

		/**
		 * Create and show a new centered window
		 * @param title Window title
		 * @param w Width
		 * @param h Height
		 */
		window(const std::string &title, int w, int h);

		/**
		 * Destroy window and free resources
		 */
		~window();

		/**
		 * Clear window
		 * @param r Red
		 * @param g Green
		 * @param b Blue
		 * @return True if successful
		 */
		bool clear(uint8_t r = 0x00, uint8_t g = 0x00, uint8_t b = 0x00);

		/**
		 * Render to screen
		 */
		void render();

		/**
		 * Render texture to window
		 * @param texture Texture to render
		 * @param x X position
		 * @param y Y position
		 */
		void draw(SDL_Texture *texture, int x, int y);

		/**
		 * Draw something to the window
		 * @param drawable Drawable to render
		 * @param x X position
		 * @param y Y position
		 */
		void draw(drawable &drawable);

		/**
		 * Window width
		 */
		int width();

		/**
		 * Window height
		 */
		int height();

		/**
		 * Window is open
		 */
		bool is_open() const;

		/**
		 * Get the underlying renderer
		 */
		SDL_Renderer *get_renderer();

		/**
		 * Get the underlying window handle
		 */
		SDL_Window *get_window_handle();

		void tick();

	private:
		SDL_Window *sdl_window = nullptr;
		SDL_Renderer *sdl_renderer = nullptr;
		bool running = true;
	};
}