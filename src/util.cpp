#include "main.hpp"
#include <fcntl.h>
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
