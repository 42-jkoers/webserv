#pragma once
#include "config_parser.hpp"
#include <arpa/inet.h>
#include <array>
#include <cassert>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <map>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#define BUFFER_SIZE 16384
#define SERVER_BACKLOG 100
#define MAX_CLIENTS 5

typedef int fd_t;
class Config;

enum IP_mode {
	mode_ipv4,
	mode_ipv6
};

namespace exit_with {

void e_perror(const std::string& msg);
void e_errno(const std::string& msg);
void message(const std::string& msg);

} // namespace exit_with
void					 print_escaped(const char* s, size_t n, std::string label = "");
std::vector<const char*> vector_to_c_array(const std::vector<std::string>& v);
std::string				 to_lower(const std::string& s);
std::string				 to_upper(const std::string& s);
std::vector<std::string> ft_split(const std::string& s, const std::string& charset);

namespace Color {

static const char DEFAULT[] = "\033[0m";
static const char RED[] = "\033[31m";
static const char GREEN[] = "\033[32m";
static const char BLUE[] = "\033[34m";

} // namespace Color

namespace std_ft {

template <typename T>
std::string to_string(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

template <class T>
const T& max(const T& a, const T& b) {
	return a > b ? a : b;
}

template <class T>
const T& min(const T& a, const T& b) {
	return a < b ? a : b;
}

} // namespace std_ft

// Wrappers for c standard functions
namespace cpp {

int			execve(const std::string& path, const std::vector<std::string>& arg, const std::vector<std::string>& envp);
std::string inet_ntop(struct sockaddr address);

} // namespace cpp

std::string readFile(const std::string& path);

// Someone should probably move this
std::string get_client_address(struct sockaddr_in& address);
void		log_pollfd(const struct pollfd& pfd);
void		log_event(short event);

// true on success
template <typename T>
bool parse_int(T& output, const std::string& str) { // todo std::is_integral
	char			  c;
	std::stringstream ss(str);
	ss >> output;
	return !(ss.fail() || ss.get(c));
}

// @return length of parsed string, 0 on error
// @param ending is the char that should come after the hex string
template <typename T>
size_t parse_hex(T& output, const std::string& str, char ending) { // todo std::is_integral
	char			  c;
	std::stringstream ss;
	ss << std::hex << str;
	ss >> output;

	if (ss.eof())
		return 0;
	ss.get(c);
	if (ss.fail() || c != ending)
		return 0;
	return str.find(ending);
}

// true on success
template <typename T>
bool parse_hex(T& output, const std::string& str) { // todo std::is_integral
	char			  c;
	std::stringstream ss;
	ss << std::hex << str;
	ss >> output;
	return !(ss.fail() || ss.get(c));
}
