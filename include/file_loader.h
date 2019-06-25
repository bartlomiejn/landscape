#ifndef LANDSCAPE_FILE_LOADER_H
#define LANDSCAPE_FILE_LOADER_H

#include <string>

class FileLoader
{
public:
	explicit FileLoader(std::string &&name);
	std::string read();
private:
	std::string filename;
};

#endif //LANDSCAPE_FILE_LOADER_H
