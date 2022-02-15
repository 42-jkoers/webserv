#pragma once
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
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

std::string get_client_address(struct sockaddr_in& address);
