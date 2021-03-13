#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "ce/font.hpp"

namespace ce
{
	/**
	 * Generic asset loader
	 * @note Abstract
	 */
	class asset_loader
	{
	public:
		/**
		 * Load a font
		 * @param name File name
		 * @param size Point size
		 */
		font get_font(const std::string &name, int size);

	protected:
		/**
		 * How to load assets, implemented by an asset loader
		 * @param path type/name.extension
		 * @return Loaded data
		 * @note Should throw on error
		 */
		virtual std::vector<std::uint8_t> load(const std::string &path) = 0;
	};
}
