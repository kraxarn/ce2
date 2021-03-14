#include "ce/engine.hpp"

#include <SDL.h>

#include "ce/format.hpp"
#include "ce/version.hpp"

std::string ce::engine::get_version()
{
	SDL_version version;
	SDL_GetVersion(&version);

	return fmt::format("CrowEngine {} (SDL {}.{}.{})",
		CE_VERSION_STR,
		version.major, version.minor, version.patch);
}
