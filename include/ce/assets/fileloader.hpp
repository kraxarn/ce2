#pragma once

#include <fstream>

#include "ce/assets/assetloader.hpp"
#include "ce/format.hpp"

namespace ce
{
	/**
	 * Loads assets from file in the data/ folder
	 */
	class file_loader: public asset_loader
	{
	protected:
		std::vector<std::uint8_t> load(const std::string &path) override;
	};
}
