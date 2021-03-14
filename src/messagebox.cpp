
#include "ce/ui/messagebox.hpp"

#include <SDL.h>

bool ce::message_box::show(uint32_t flags,
	const std::string &title,
	const std::string &message,
	window *window)
{
	auto sdl_window = window != nullptr
		? window->get_window_handle()
		: nullptr;
	auto result = SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), sdl_window);
	if (!result)
		std::cerr << SDL_GetError() << std::endl;
	return result;
}

bool ce::message_box::error(const std::string &title, const std::string &message)
{
	return show(SDL_MESSAGEBOX_ERROR, title, message);
}

bool ce::message_box::warning(const std::string &title, const std::string &message)
{
	return show(SDL_MESSAGEBOX_WARNING, title, message);
}

bool ce::message_box::information(const std::string &title, const std::string &message)
{
	return show(SDL_MESSAGEBOX_INFORMATION, title, message);
}
