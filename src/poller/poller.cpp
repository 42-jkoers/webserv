#include "poller.hpp"
#include "main.hpp"
#include "response.hpp"
#include <fcntl.h>
#include <limits>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>

Poller::Poller() {
	_n_servers = 0;
}

void Poller::add_server(IP_mode ip_mode, uint16_t port) {
	fd_t				server_socket = constructors::server_socket(ip_mode, port);
	const struct pollfd pfd = constructors::pollfd(server_socket, POLLIN | POLLOUT);

	_pollfds.push_back(pfd);
	_server_ports.push_back(port);
	_n_servers++;
}

void Poller::_accept_clients() {
	for (size_t i = 0; i < _n_servers; i++) {
		while (true) {
			int newfd = accept(_pollfds[i].fd, NULL, NULL);
			if (newfd < 0 && errno != EWOULDBLOCK) // TODO errno is not allowed
				exit_with::e_perror("accept() failed");
			if (newfd < 0)
				break;

			const struct pollfd client_pfd = constructors::pollfd(newfd, POLLIN | POLLOUT);
			_pollfds.push_back(client_pfd);
			if (_clients.find(client_pfd.fd) == _clients.end())
				_clients[client_pfd.fd] = Client(_server_ports[i]);
		}
	}
}

#define FD_CLOSED -1
void Poller::_on_new_pollfd(pollfd& pfd, void (*on_request)(Client& client)) {
	Client& client = _clients[pfd.fd];

	client.read_pollfd(pfd);
	if (client.parse_status() == Client::FINISHED) {
		on_request(client);
	}
	if (client.parse_status() == Client::FINISHED ||
		client.parse_status() == Client::ERROR) {
		_clients.erase(pfd.fd);
		close(pfd.fd); // TODO: only when keepalive is true
		pfd.fd = FD_CLOSED;
	}
}

void Poller::start(void (*on_request)(Client& client)) {
	assert(_n_servers > 0);
	while (true) {
		int rc = poll(_pollfds.data(), _pollfds.size(), -1);
		if (rc < 0)
			exit_with::e_perror("poll() failed");
		if (rc == 0)
			exit_with::e_perror("poll() timeout");
		_accept_clients();
		for (size_t i = _n_servers; i < _pollfds.size(); i++) {
			if (_pollfds[i].revents == 0)
				continue;
			_on_new_pollfd(_pollfds[i], on_request);
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
