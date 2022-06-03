#include "poller.hpp"
#include "file_system.hpp"
#include "main.hpp"
#include "response.hpp"
#include "router.hpp"
#include <fcntl.h>
#include <limits>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>

Router g_router;

Poller::Poller() {}

void Poller::add_server(IP_mode ip_mode, const char* str_addr, uint16_t port) {
	for (const std::pair<const fd_t, Server>& i : _servers)
		if (i.second.port == port)
			return;

	fd_t				server_socket = constructors::server_socket(ip_mode, str_addr, port);
	const struct pollfd pfd = constructors::pollfd(server_socket, POLLIN | POLLOUT);

	add_fd(pfd, Server(pfd.fd, port));
}

void Poller::accept_clients(const Server& server) {
	while (true) {
		struct sockaddr address;
		socklen_t		address_len = sizeof(address);
		int				newfd = accept(server.fd, &address, &address_len);

		if (newfd < 0 && errno != EWOULDBLOCK)
			exit_with::e_perror("accept() failed");
		if (newfd < 0)
			break;

		const struct pollfd client_pfd = constructors::pollfd(newfd, POLLIN | POLLOUT);
		add_fd(client_pfd, Client(cpp::inet_ntop(address), server.port));
	}
}

#define FD_CLOSED -1

void Poller::on_poll(pollfd& pfd) {
	if (_fd_types[pfd.fd] == Type::SERVER) {
		accept_clients(_servers[pfd.fd]);
	}

	else if (_fd_types[pfd.fd] == Type::CLIENT) {
		Client& client = _clients[pfd.fd];
		client.on_pollevent(pfd);
		if (client.parse_status() == Client::Parse_status::FINISHED) {
			_fd_types[pfd.fd] = Type::RESPONSE_WRITE;

			// fd_t response_fd = g_router.route(client);
			// TODO header before pipe file
			// add_fd(response, Response(pfd.fd));
			std::string file = fs::read_file("./www/helloworld_response.txt");
			write(pfd.fd, file.data(), file.size());
			// dup2(client.request.fd, response_fd);
			_clients.erase(pfd.fd);
			close(pfd.fd);
		}

		else if (client.parse_status() == Client::Parse_status::ERROR) {
			_fd_types.erase(pfd.fd);
			_clients.erase(pfd.fd);
			close(pfd.fd);
			pfd.fd = FD_CLOSED;
		}
	}
}

void Poller::start() {
	while (true) {
		int rc = poll(_pollfds.data(), _pollfds.size(), -1);
		if (rc < 0)
			exit_with::e_perror("poll() failed");
		if (rc == 0)
			exit_with::e_perror("poll() timeout");
		for (size_t i = 0; i < _pollfds.size(); i++) {
			if (_pollfds[i].revents == 0)
				continue;
			on_poll(_pollfds[i]);
		}
		// removing closed fds from array by shifting them to the left
		std::vector<struct pollfd>::iterator valid = _pollfds.begin();
		std::vector<struct pollfd>::iterator current = _pollfds.begin();
		while (current != _pollfds.end()) {
			if (current->fd == FD_CLOSED) {
				++current;
				continue;
			}
			*valid++ = *current++;
		}
		_pollfds.erase(valid, _pollfds.end());
	}
}

Poller::~Poller() {} // TODO: close fds etc.
