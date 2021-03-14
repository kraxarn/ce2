#include "ce/engine.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

ce::engine::engine()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		throw std::runtime_error(SDL_GetError());

	if (!TTF_Init())
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

	std::stringstream ss;

	// CrowEngine current has no version
	ss << "crowengine v1.0.0 (SDL "
		<< std::to_string(version.major) << "."
		<< std::to_string(version.minor) << "."
		<< std::to_string(version.patch) << ")";

	return ss.str();
}
