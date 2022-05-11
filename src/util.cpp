#include "main.hpp"
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <unistd.h>

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
std::vector<const char*> vector_to_c_array(const std::vector<std::string>& v) {
	std::vector<const char*> cv;
	cv.reserve(v.size() + 1);
	for (size_t i = 0; i < v.size(); i++)
		cv.push_back(v[i].c_str());
	cv.push_back(NULL);
	return cv;
}

std::string to_lower(const std::string& s) {
	std::string out;
	out.reserve(s.size());
	for (size_t i = 0; i < s.size(); i++)
		out += std::tolower(s[i]);
	return out;
}

std::string to_upper(const std::string& s) {
	std::string out;
	out.reserve(s.size());
	for (size_t i = 0; i < s.size(); i++)
		out += std::toupper(s[i]);
	return out;
}

// splits s in any of the delimiters in delim
std::vector<std::string> ft_split(const std::string& s, const std::string& charset) {
	std::vector<std::string> out;
	size_t					 start;
	size_t					 end = 0;

	while ((start = s.find_first_not_of(charset, end)) != std::string::npos) {
		end = std::string::npos;
		for (size_t i = 0; i < charset.size(); i++)
			end = std_ft::min(end, s.find(charset[i], start));

		out.push_back(s.substr(start, end - start));
	}
	return out;
}

// Wrappers for c standard funcions
namespace cpp {

int execve(const std::string& path, const std::vector<std::string>& arg, const std::vector<std::string>& envp) {
	std::vector<const char*> argv_c = vector_to_c_array(arg);
	std::vector<const char*> envp_c = vector_to_c_array(envp);

	return ::execve(path.c_str(), (char* const*)envp_c.data(), (char* const*)envp_c.data());
}

} // namespace cpp
