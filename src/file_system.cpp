#include "file_system.hpp"
#include <dirent.h>
#include <fstream>

std::string exception_str(const std::string& path) {
	std::string err;
	err += "\nCannot open \"";
	err += path;
	err += "\": ";
	err += std::strerror(errno);
	return err;
}

namespace fs {

std::string read_file(const std::string& path) {
	std::ifstream	  file;
	std::stringstream ss;

	file.open(path.c_str());
	if (!file.is_open())
		throw std::runtime_error(exception_str(path).c_str());
	ss << file.rdbuf();
	file.close();
	return ss.str();
}

std::vector<std::string> list_dir(const std::string& path) {
	std::vector<std::string> files;
	DIR*					 dir = opendir(path.c_str());
	if (!dir)
		throw std::runtime_error(exception_str(path).c_str());

	while (true) {
		struct dirent* entry = readdir(dir);
		if (!entry)
			break;
		// if (!strcmp(entry->d_name, ".") && !strcmp(entry->d_name, ".."))
		files.push_back(entry->d_name);
	}
	closedir(dir);
	return files;
}
} // namespace fs
