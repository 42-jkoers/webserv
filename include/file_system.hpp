#pragma once
#include "main.hpp"
#include <cstring>
#include <exception>

namespace fs {
std::string exception_str(const std::string& path);

void		mkdir(const std::string& path);
std::string read_file(const std::string& path);

void		write_file(const std::string& path, const std::string& data);
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
bool					 is_direcory(const std::string& path);

} // namespace fs

namespace path {

std::string normalize(std::string path);

bool		is_same(const std::string& path1, const std::string& path2);

std::string join(const std::string& path1, const std::string& path2);
std::string join(const std::string& path1, const std::string& path2, const std::string& path3);
std::string join(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4);

std::string absolute(const std::string& path);
} // namespace path
