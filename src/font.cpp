#include "ce/font.hpp"

#include <stdexcept>

#include <SDL.h>
#include <SDL_ttf.h>

ce::font::font(const std::vector<uint8_t> &data, int size)
{
	auto rw = SDL_RWFromMem((void *) data.data(), data.size());
	if (rw == nullptr)
		throw std::runtime_error(SDL_GetError());

	ttf_font = TTF_OpenFontRW(rw, true, size);
	if (ttf_font == nullptr)
		throw std::runtime_error(TTF_GetError());
}

ce::font::~font()
{
	if (ttf_font != nullptr)
		TTF_CloseFont(ttf_font);
}

SDL_Surface *ce::font::render_text(const std::string &text, const ce::color &color) const
{
	return TTF_RenderText_Solid(ttf_font, text.c_str(), color.to_sdl_color());
}
