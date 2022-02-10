#include "main.hpp"
#include <netinet/in.h>
#include <sstream>

// returns fd to socket
fd_t create_socket() {
	fd_t fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == 0)
		exit_with::e_perror("Cannot create socket");
	return fd;
}

void listen_on_socket(fd_t fd, unsigned int port, struct sockaddr_in& address) {

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	bzero(address.sin_zero, sizeof(address.sin_zero));

	if (bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0)
		exit_with::e_perror("Cannot bind to port");
	if (listen(fd, 10) < 0) // TODO: change number of clients
		exit_with::e_perror("Cannot listen on port");
}

fd_t accept_from_fd(fd_t fd, const struct sockaddr_in& address) {
	socklen_t socklen = sizeof(address);
	fd_t	  socket_fd = accept(fd, (struct sockaddr*)&address, &socklen);
	if (socket_fd < 0)
		exit_with::e_perror("Cannot accept");
	return socket_fd;
}

bool is_end_of_http_request(const std::string& s) { // TODO: better?
	if (s.size() < 4)
		return true;
	return strncmp(s.data() + (s.size() - 4), "\r\n\r\n", 4) == 0;
}

std::string read_request(fd_t fd) {
	std::string str;
	static char buf[BUFFER_SIZE + 1];
	ssize_t		bytes_read;

	do {
		bytes_read = read(fd, buf, BUFFER_SIZE);
		if (bytes_read == -1)
			exit_with::e_perror("Cannot read from fd");
		buf[bytes_read] = '\0';
		str += buf;
	} while (!is_end_of_http_request(str));
	return str;
}

void write_response(fd_t fd, const std::string& message) {
	write(fd, message.c_str(), message.length());
}
