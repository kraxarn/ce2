#pragma once

#include <string>

#include "ce/window.hpp"

namespace ce
{
	/**
	 * Show simple message boxes
	 */
	class message_box
	{
	public:
		/**
		 * Show an error dialog
		 */
		static bool error(const std::string &title, const std::string &message);

		/**
		 * Show a warning dialog
		 */
		static bool warning(const std::string &title, const std::string &message);

		/**
		 * Show an information dialog
		 */
		static bool information(const std::string &title, const std::string &message);

	private:
		message_box() = default;

		static bool show(uint32_t flags, const std::string &title, const std::string &message,
			window *window = nullptr);
	};
}
