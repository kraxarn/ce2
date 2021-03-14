#include "ce/graphics/text.hpp"

#include <SDL.h>

using namespace ce;

text::text(const font &text_font)
	: text_font(text_font)
{
}

text::~text()
{
	if (text_surface != nullptr)
		SDL_FreeSurface(text_surface);

	if (texture != nullptr)
		SDL_DestroyTexture(texture);
}

void text::set_text(const std::string &text)
{
	render_text = text;
	update_text_surface();
}

void text::set_color(const ce::color &color)
{
	text_color = color;
	update_text_surface();
}

void text::update_text_surface()
{
	if (text_surface != nullptr)
		SDL_FreeSurface(text_surface);
	text_surface = text_font.render_text(render_text, text_color);

	// Texture will need to be re-rendered when next needed
	if (texture != nullptr)
		SDL_DestroyTexture(texture);
	texture = nullptr;
}

int text::width() const
{
	return text_surface == nullptr
		? -1
		: text_surface->w;
}

int text::height() const
{
	return text_surface == nullptr
		? -1
		: text_surface->h;
}

SDL_Texture *text::get_texture(SDL_Renderer *renderer)
{
	if (texture == nullptr)
		texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	return texture;
}
