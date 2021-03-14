#include "ce/log.hpp"

#include <SDL.h>

using namespace ce;

void log::log_message(int priority, const std::string &message)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, static_cast<SDL_LogPriority>(priority),
		"%s", message.c_str());
}

void ce::log::critical(const std::string &message)
{
	log_message(SDL_LOG_PRIORITY_CRITICAL, message);
}

void ce::log::debug(const std::string &message)
{
	log_message(SDL_LOG_PRIORITY_DEBUG, message);
}

void ce::log::error(const std::string &message)
{
	log_message(SDL_LOG_PRIORITY_ERROR, message);
}

void ce::log::info(const std::string &message)
{
	log_message(SDL_LOG_PRIORITY_INFO, message);
}

void ce::log::verbose(const std::string &message)
{
	log_message(SDL_LOG_PRIORITY_VERBOSE, message);
}

void ce::log::warn(const std::string &message)
{
	log_message(SDL_LOG_PRIORITY_WARN, message);
}
