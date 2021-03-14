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
