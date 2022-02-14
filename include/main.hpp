#pragma once
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <cstdlib>
#include <string.h>

#define BUFFER_SIZE 4096
#define SERVER_BACKLOG 100
#define PORT 8081
typedef int fd_t;

namespace exit_with {

void e_perror(const std::string& msg);

} // namespace exit_with

namespace cpp11 {

template <typename T>
std::string to_string(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

} // namespace cpp11

fd_t		create_socket();
void		listen_on_socket(fd_t fd, unsigned int port, struct sockaddr_in& address);
fd_t		accept_from_fd(fd_t fd, const struct sockaddr_in& address);
std::string read_request(fd_t fd);
void		response(fd_t fd, uint32_t response_code, const std::string& message);
std::string get_client_address(struct sockaddr_in& address);
