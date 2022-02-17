#include "main.hpp"
#include <fcntl.h>
#include <fstream>
#include <iostream>

namespace exit_with {

void e_perror(const std::string& msg) {
	perror(msg.c_str());
	exit(EXIT_FAILURE);
}

void message(const std::string& msg) {
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}

} // namespace exit_with

std::string readFile(const std::string& path) {
	std::ifstream	  file;
	std::stringstream ss;

	file.open(path.c_str());
	if (!file.is_open())
		exit_with::e_perror("Could not open file");
	ss << file.rdbuf();
	file.close();
	return ss.str();
}
