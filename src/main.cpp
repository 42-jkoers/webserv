#include "main.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>

#define PORT 8081

int main() {
	fd_t listen_fd = create_socket();

	// struct sockaddr_in address;
	// bzero(&address, sizeof(address));
	// address.sin_family = AF_INET;
	// address.sin_addr.s_addr = INADDR_ANY;
	// address.sin_port = htons(PORT);
	struct sockaddr_in6 address;
	memset(&address, 0, sizeof(address));
	address.sin6_family = AF_INET6;
	memcpy(&address.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	address.sin6_port = htons(PORT);

	if (bind(listen_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
		exit_with::e_perror("Cannot bind to port");
	if (fcntl(listen_fd, F_SETFD, fcntl(listen_fd, F_GETFD, 0) | O_NONBLOCK) == -1)
		exit_with::e_perror("Cannot set non blocking");
	if (listen(listen_fd, 32) < 0) // TODO: change number of clients
		exit_with::e_perror("Cannot listen on port");

	struct pollfd pollfds[BUFFER_SIZE];
	bzero(pollfds, sizeof(pollfds));
	pollfds[0].fd = listen_fd;
	pollfds[0].events = POLLIN;
	nfds_t n_pollfds = 1;

	std::cout << "Listening on: http://localhost:" << PORT << std::endl;

	while (true) {
		std::cout << "Waiting on poll(), n_pollfds: " << n_pollfds << std::endl;
		int rc = poll(pollfds, n_pollfds, 50000);
		std::cout << "l" << std::endl;
		if (rc < 0)
			exit_with::e_perror("poll() failed");
		if (rc == 0)
			exit_with::e_perror("poll() timeout");

		const size_t current_n_pollfds = n_pollfds;
		for (size_t i = 0; i < current_n_pollfds; i++) {
			if (pollfds[i].fd < 0)
				continue;

			if (pollfds[i].revents == 0)
				continue;
			if (pollfds[i].revents != POLLIN)
				exit_with::message("Unexpected revents value");
			if (pollfds[i].fd == listen_fd) { // accepting queued incoming connections
				while (true) {				  // while there are incoming connections
					int newfd = accept(listen_fd, NULL, NULL);
					if (newfd < 0) {
						if (errno != EWOULDBLOCK)
							exit_with::e_perror("accept() failed");
						break;
					}
					std::cout << "New incoming connection - " << newfd << std::endl;
					pollfds[n_pollfds].fd = newfd;
					pollfds[n_pollfds].events = POLLIN;
					n_pollfds++;
				}
			} else { // reading
				std::cout << "Descriptor " << pollfds[i].fd << " is readable " << std::endl;
				std::string request = read_request(pollfds[i].fd);
				// response(pollfds[i].fd, 200, "Hello World!");
				close(pollfds[i].fd);
				pollfds[i].fd = -1;
				std::cout << "request:\n " << request << std::endl;
			}
		}
	}
	return 0;
}
