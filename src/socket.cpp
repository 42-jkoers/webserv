#include "main.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>

// returns fd to socket
fd_t create_socket() {
	fd_t fd = socket(AF_INET6, SOCK_STREAM, 0);
	if (fd < 0)
		exit_with::e_perror("Cannot create socket");
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
		exit_with::e_perror("setsockopt() failed");
	if (ioctl(fd, FIONBIO, (char*)&on) < 0)
		exit_with::e_perror("ioctl() failed");

	return fd;
}

// void listen_on_socket(fd_t fd, unsigned int port, uint32_t clients, struct sockaddr_in& address) {
// 	bzero(&address, sizeof(address));
// 	address.sin_family = AF_INET;
// 	address.sin_addr.s_addr = INADDR_ANY;
// 	address.sin_port = htons(port);

// 	if (bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0)
// 		exit_with::e_perror("Cannot bind to port");
// 	// if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) == -1)
// 	// 	exit_with::e_perror("Cannot set non blocking");
// 	if (listen(fd, clients) < 0) // TODO: change number of clients
// 		exit_with::e_perror("Cannot listen on port");
// }

// std::vector<struct pollfd> accept_from_fd(fd_t socket_fd, uint32_t clients) {
// 	std::vector<struct pollfd> poll_fds;
// 	struct sockaddr_in		   client;
// 	socklen_t				   address_len = sizeof(client);

// 	for (size_t i = 0; i < clients; i++) {
// 		bzero(&client, sizeof(client));
// 		struct pollfd poll_fd;
// 		poll_fd.fd = accept(socket_fd, (struct sockaddr*)&client, &address_len);
// 		if (poll_fd.fd < 0)
// 			exit_with::e_perror("Cannot accept");
// 		poll_fd.events = POLLIN;
// 		poll_fds.push_back(poll_fd);
// 	}
// 	sleep(1);
// 	return poll_fds;
// }

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
		if (bytes_read == 0)
			break;
		buf[bytes_read] = '\0';
		str += buf;
	} while (!is_end_of_http_request(str));
	return str;
}
