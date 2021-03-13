#include "ce/assets/fileloader.hpp"

using namespace ce;

std::vector<std::uint8_t> file_loader::load(const std::string &path)
{
	std::ifstream file(fmt::format("data/{}", path), std::ios::binary);
	return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}
