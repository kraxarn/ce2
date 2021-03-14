#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace ce
{
	/**
	 * Engine
	 * @note Will be embedded into window soon-ish
	 */
	class engine
	{
	public:
		engine();

		static std::string get_version();

		~engine();
	};
}
