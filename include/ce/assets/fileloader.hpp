#pragma once

#include <fstream>
#include <utility>

#include "ce/assets/assetloader.hpp"
#include "ce/format.hpp"

namespace ce
{
	/**
	 * Loads assets from file in a folder
	 */
	class file_loader: public asset_loader
	{
	public:
		/**
		 * Construct a new file loader
		 * @param folder Folder to load files from
		 */
		explicit file_loader(std::string folder);

	protected:
		std::vector<std::uint8_t> load(const std::string &path) override;

	private:
		std::string folder_name;
	};
}
