#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace ce
{
	/**
	 * Engine utilities
	 */
	class engine
	{
	public:
		/**
		 * Get version of engine and linked SDL
		 * @return "crowengine {version} (SDL {version})"
		 */
		static std::string version();

	private:
		engine() = default;
	};
}
