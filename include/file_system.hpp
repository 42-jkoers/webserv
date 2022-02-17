#pragma once
#include "main.hpp"
#include <cstring>
#include <exception>

namespace fs {

std::string				 read_file(const std::string& path);
std::vector<std::string> list_dir(const std::string& path);

} // namespace fs
