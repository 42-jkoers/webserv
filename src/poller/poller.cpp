#include "poller.hpp"
#include "main.hpp"
#include "response.hpp"
#include <fcntl.h>
#include <limits>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>

Poller::Poller() {}

void Poller::add_server(IP_mode ip_mode, const char* str_addr, uint16_t port) {
	for (const std::pair<const fd_t, Fdinfo>& i : _fdinfo)
		if (i.second.type == Fdinfo::Type::SERVER && i.second.server.port == port)
			return;

	fd_t				server_socket = constructors::server_socket(ip_mode, str_addr, port);
	const struct pollfd pfd = constructors::pollfd(server_socket, POLLIN | POLLOUT);

	_pollfds.push_back(pfd);
	_fdinfo[pfd.fd] = Fdinfo(Fdinfo::Type::SERVER);
	_fdinfo[pfd.fd].server = Fdinfo::Server(port);
}

void Poller::_on_server(const pollfd& pfd) {
	while (true) {
		struct sockaddr address;
		socklen_t		address_len = sizeof(address);
		int				newfd = accept(pfd.fd, &address, &address_len);

		if (newfd < 0 && errno != EWOULDBLOCK)
			exit_with::e_perror("accept() failed");
		if (newfd < 0)
			break;

		const struct pollfd client_pfd = constructors::pollfd(newfd, POLLIN | POLLOUT);
		_pollfds.push_back(client_pfd);
		_fdinfo[client_pfd.fd] = Fdinfo(Fdinfo::Type::CLIENT);
		_fdinfo[client_pfd.fd].client = Client(cpp::inet_ntop(address), _fdinfo[pfd.fd].server.port);
	}
}

#define FD_CLOSED -1
void Poller::_on_client(pollfd& pfd, void (*on_request)(Client& client)) {
	Client& client = _fdinfo[pfd.fd].client;
	client.on_pollevent(pfd);
	if (client.parse_status() >= Client::FINISHED) {
		on_request(client);
	}
	if (client.parse_status() >= Client::FINISHED) {
		_fdinfo.erase(pfd.fd);
		close(pfd.fd); // TODO: only when keepalive is true
		pfd.fd = FD_CLOSED;
	}
}

void Poller::_on_response(const pollfd& pfd) {
	(void)pfd;
}

void Poller::_on_poll(pollfd& pfd, void (*on_request)(Client& client)) {
	if (_fdinfo[pfd.fd].type == Fdinfo::Type::SERVER)
		_on_server(pfd);
	else if (_fdinfo[pfd.fd].type == Fdinfo::Type::CLIENT)
		_on_client(pfd, on_request);
	else if (_fdinfo[pfd.fd].type == Fdinfo::Type::RESPONSE)
		_on_response(pfd);
}

void Poller::start(void (*on_request)(Client& client)) {
	while (true) {
		int rc = poll(_pollfds.data(), _pollfds.size(), -1);
		if (rc < 0)
			exit_with::e_perror("poll() failed");
		if (rc == 0)
			exit_with::e_perror("poll() timeout");
		for (size_t i = 0; i < _pollfds.size(); i++) {
			if (_pollfds[i].revents == 0)
				continue;
			_on_poll(_pollfds[i], on_request);
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
