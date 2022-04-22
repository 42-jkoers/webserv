#pragma once
#include "main.hpp"
#include <cstring>
#include <exception>

namespace fs {
std::string exception_str(const std::string& path);

void		mkdir(const std::string& path);
std::string read_file(const std::string& path);

template <typename T>
void write_file(const std::string& path, const std::vector<T>& data) {
	size_t slash = path.find_last_of("/");
	if (slash != std::string::npos)
		fs::mkdir(path.substr(0, slash));

	std::fstream file;
	file.open(path.c_str(), std::ios::out);
	if (!file.is_open())
		throw std::runtime_error(fs::exception_str(path).c_str());
	file.write(data.data(), data.size() * sizeof(T));
	file.close();
}
void					 write_file(const std::string& path, const std::string& content);

std::vector<std::string> list_dir(const std::string& path);
bool					 path_exists(const std::string& path);

} // namespace fs
