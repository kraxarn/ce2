#include "ce/assets/fileloader.hpp"

using namespace ce;

file_loader::file_loader(std::string folder)
	: folder_name(std::move(folder))
{
}

std::vector<std::uint8_t> file_loader::load(const std::string &path)
{
	auto file_path = fmt::format("{}/{}", folder_name, path);
	std::ifstream file(file_path, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error(fmt::format("failed to load file: \"{}\"", file_path));

	return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}
