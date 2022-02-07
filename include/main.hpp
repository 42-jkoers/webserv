#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define BUFFER_SIZE 4096
typedef int fd_t;

namespace exit_with {

void e_perror(const std::string& msg);

} // namespace exit_with

fd_t		create_socket();
void		listen_on_socket(fd_t fd, unsigned int port, struct sockaddr_in& address);
fd_t		accept_from_fd(fd_t fd, const struct sockaddr_in& address);
std::string read_request(fd_t fd);
void		write_response(fd_t fd, const std::string& message);
