#pragma once
#include "main.hpp"
#include <cstring>
#include <exception>

namespace fs {

std::string read_file(const std::string& path);

template <typename T>
bool write_file(const std::string& path, const std::vector<T>& data) { // returns true on success
	std::fstream file;
	file.open(path.c_str());
	if (!file.is_open())
		return false;
	file.write(data.data(), data.size() * sizeof(T));
	file.close();
	return true;
}

std::vector<std::string> list_dir(const std::string& path);

} // namespace fs
