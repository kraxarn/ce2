#include "ce/assets/assetloader.hpp"
#include "ce/format.hpp"

using namespace ce;

font asset_loader::get_font(const std::string &name, int size)
{
	return font(load(fmt::format("font/{}", name)), size);
}
