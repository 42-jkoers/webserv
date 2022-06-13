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

#ifdef __APPLE__
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#else
#include <sys/sendfile.h>
#endif

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
			exit_with::perror("accept() failed");
		if (newfd < 0)
			break;
		if (fcntl(newfd, F_SETFL, O_NONBLOCK) == -1)
			exit_with::perror("Cannot set non blocking");

		const struct pollfd client_pfd = constructors::pollfd(newfd, POLLIN | POLLOUT);
		add_fd(client_pfd, Client(cpp::inet_ntop(address), server.port));
	}
}

void Poller::on_poll(pollfd pfd, Client& client) {
	client.on_pollevent(pfd);
	if (client.parse_status() < Client::Parse_status::FINISHED)
		return;

	if (client.parse_status() == Client::Parse_status::ERROR) {
		close_fd(pfd);
		return;
	}

	Route route = g_router.route(client);
	if (route.send(pfd.fd) != Route::Status::IN_PROGRESS) {
		close_fd(pfd);
		return;
	}
	_clients.erase(pfd.fd);
	add_fd(pfd, route);
}

void Poller::on_poll(pollfd pfd) {
	switch (_fd_types[pfd.fd]) {
	case Fd_type::SERVER:
		accept_clients(_servers[pfd.fd]);
		break;

	case Fd_type::CLIENT:
		on_poll(pfd, _clients[pfd.fd]);
		break;
	case Fd_type::ROUTE:
		if (_routes[pfd.fd].send(pfd.fd) != Route::Status::IN_PROGRESS)
			close_fd(pfd);
		break;
	default:
		break;
	}
}

void Poller::start() {
	while (true) {
		int rc = poll(_pollfds.data(), _pollfds.size(), -1);
		if (rc < 0)
			exit_with::perror("poll() failed");
		if (rc == 0)
			exit_with::perror("poll() timeout");
		for (size_t i = 0; i < _pollfds.size(); i++) {
			if (_pollfds[i].revents == 0)
				continue;
			on_poll(_pollfds[i]);
		}
		// removing closed fds from array by shifting them to the left
		std::vector<struct pollfd>::iterator valid = _pollfds.begin();
		std::vector<struct pollfd>::iterator current = _pollfds.begin();
		while (current != _pollfds.end()) {
			if (_fd_types.find(current->fd) == _fd_types.end() || _fd_types.at(current->fd) == Fd_type::CLOSED) {
				_fd_types.erase(current->fd);
				++current;
				continue;
			}
			*valid++ = *current++;
		}
		_pollfds.erase(valid, _pollfds.end());
	}
}

Poller::~Poller() {}
