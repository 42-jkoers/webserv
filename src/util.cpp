#include "main.hpp"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <ostream>

namespace exit_with {

void e_perror(const std::string& msg) {
	perror(msg.c_str());
	exit(EXIT_FAILURE);
}

void message(const std::string& msg) {
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}

void e_errno(const std::string& msg) {
	std::cout << msg << std::endl;
	std::cout << std::strerror(errno) << std::endl;
	exit(EXIT_FAILURE);
}

} // namespace exit_with

void print_escaped(const char* s, size_t n, std::string label) {
	static const std::string hidden = "\a\b\f\n\r\t\v";
	static const std::string print = "abfnrtv";
	size_t					 h;

	if (label.length())
		std::cout << label << ": <";
	for (size_t i = 0; i < n; i++) {
		if ((h = hidden.find(s[i])) != std::string::npos) {
			std::cout << Color::BLUE << "\\" << print[h] << Color::DEFAULT;
			if (s[i] == '\n')
				std::cout << '\n';
		} else if (!std::isprint(s[i]))
			std::cout << Color::BLUE << "0x" << std::hex << std::uppercase << static_cast<int>(s[i]) << Color::DEFAULT;
		else
			std::cout << s[i];
	}
	if (label.length())
		std::cout << ">" << std::endl;
	else
		std::cout << std::ends;
}

// Returns null terminated array of pointers to strings
// Can be used like this: char **env = vector_to_c_array(v).data();
std::vector<const char*> vector_to_c_array(const std::vector<const std::string>& v) {
	std::vector<const char*> cv;
	cv.reserve(v.size() + 1);
	for (size_t i = 0; i < v.size(); i++)
		cv.push_back(v[i].c_str());
	cv.push_back(NULL);
	return cv;
}
