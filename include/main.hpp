#pragma once
#include "config_parser.hpp"
#include <arpa/inet.h>
#include <array>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define BUFFER_SIZE 4096
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
void message(const std::string& msg);

} // namespace exit_with

namespace cpp11 {

template <typename T>
std::string to_string(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

} // namespace cpp11

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
