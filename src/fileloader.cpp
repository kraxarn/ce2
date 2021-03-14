#include "ce/assets/fileloader.hpp"

using namespace ce;

file_loader::file_loader(std::string folder)
	: folder_name(std::move(folder))
{
}

std::vector<std::uint8_t> file_loader::load(const std::string &path)
{
	std::ifstream file(fmt::format("{}/{}", folder_name, path), std::ios::binary);
	return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}
