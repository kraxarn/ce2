#include "ce/engine.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include "ce/format.hpp"
#include "ce/version.hpp"

ce::engine::engine()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(SDL_GetError());

	if (TTF_Init() != 0)
		throw std::runtime_error(TTF_GetError());
}

ce::engine::~engine()
{
	TTF_Quit();
	SDL_Quit();
}

std::string ce::engine::get_version()
{
	SDL_version version;
	SDL_GetVersion(&version);

	return fmt::format("CrowEngine {} (SDL {}.{}.{})",
		CE_VERSION_STR,
		version.major, version.minor, version.patch);
}
