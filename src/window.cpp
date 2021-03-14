#include "ce/window.hpp"

#include <SDL.h>

using namespace ce;

window::window(const std::string &title, int x, int y, int w, int h)
{
	sdl_window = SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_SHOWN);
	if (sdl_window == nullptr)
		throw std::runtime_error(SDL_GetError());

	sdl_renderer = SDL_CreateRenderer(sdl_window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (sdl_renderer == nullptr)
		throw std::runtime_error(SDL_GetError());
}

window::window(const std::string &title, int w, int h)
	: window(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h)
{
}

window::~window()
{
	if (sdl_renderer != nullptr)
		SDL_DestroyRenderer(sdl_renderer);

	if (sdl_window != nullptr)
		SDL_DestroyWindow(sdl_window);
}

bool window::clear(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 0xff);
	return SDL_RenderClear(sdl_renderer);
}

void window::present()
{
	SDL_RenderPresent(sdl_renderer);
}

void window::render(SDL_Texture *texture, int x, int y)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;

	SDL_QueryTexture(texture, nullptr, nullptr, &dst.w, &dst.h);
	SDL_RenderCopy(sdl_renderer, texture, nullptr, &dst);
}

void window::render(ce::drawable &drawable, int x, int y)
{
	render(drawable.get_texture(sdl_renderer), x, y);
}

int window::w()
{
	int w;
	SDL_GetWindowSize(sdl_window, &w, nullptr);
	return w;
}

int window::h()
{
	int h;
	SDL_GetWindowSize(sdl_window, nullptr, &h);
	return h;
}

bool window::get_running()
{
	return running;
}

SDL_Renderer *window::get_renderer()
{
	return sdl_renderer;
}

SDL_Window *window::get_window_handle()
{
	return sdl_window;
}

void window::tick()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			running = false;
			break;
		}
	}
}
