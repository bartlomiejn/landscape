#include <file_loader.h>
#include <fstream>

FileLoader::FileLoader(std::string &&name)
{
	filename = std::move(name);
}

std::string
FileLoader::read()
{
	std::ifstream stream(filename);
	if (!stream)
		throw errno;
	std::string string(
		(std::istreambuf_iterator<char>(stream)),
		std::istreambuf_iterator<char>());
	return string;
}
