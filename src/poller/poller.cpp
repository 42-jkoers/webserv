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
	fd_t server_socket = constructors::server_socket(ip_mode, port);
	_pollfds.push_back(constructors::pollfd(server_socket, POLLIN | POLLOUT));
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

			// New incoming connection
			_pollfds.push_back(constructors::pollfd(newfd, POLLIN | POLLOUT));
		}
	}
}

#define FD_CLOSED -1
void Poller::_on_new_pollfd(pollfd& pfd, void (*on_request)(Client& client)) {
	if (_clients.find(pfd.fd) == _clients.end())
		_clients[pfd.fd] = Client();
	Client& client = _clients[pfd.fd];

	client.read_pollfd(pfd);
	if (client.parse_status() == Client::FINISHED) {
		if (client.request.response_code >= 203) {
			Response::text(client.request, client.request.response_code, "Error!\n"); // TODO
		} else
			on_request(client);
		client.reset();
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
