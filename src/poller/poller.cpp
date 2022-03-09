#include "poller.hpp"
#include "main.hpp"
#include "response.hpp"
#include <fcntl.h>
#include <limits>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>

struct sockaddr_in6 get_address6(uint32_t port) {
	struct sockaddr_in6 address;
	memset(&address, 0, sizeof(address));
	address.sin6_family = AF_INET6;
	memcpy(&address.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	address.sin6_port = htons(port);
	return address;
}

struct sockaddr_in get_address(uint32_t port) {
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	return address;
}

// returns fd to socket
static fd_t create_server_socket(IP_mode ip_mode, uint32_t port) {
	fd_t fd = socket(ip_mode == mode_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		exit_with::e_perror("Cannot create socket");
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
		exit_with::e_perror("setsockopt() failed");
	if (ioctl(fd, FIONBIO, (char*)&on) < 0)
		exit_with::e_perror("ioctl() failed");

	int rc;
	if (ip_mode == mode_ipv6) {
		struct sockaddr_in6 address = get_address6(port);
		rc = bind(fd, (struct sockaddr*)&address, sizeof(address));
	} else {
		struct sockaddr_in address = get_address(port);
		rc = bind(fd, (struct sockaddr*)&address, sizeof(address));
	}
	if (rc < 0)
		exit_with::e_perror("Cannot bind to port");

	if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) == -1)
		exit_with::e_perror("Cannot set non blocking");
	if (listen(fd, 128) < 0) // TODO: what should this number be? 128 is maximum
		exit_with::e_perror("Cannot listen on port");

	return fd;
}

Poller::Poller(IP_mode ip_mode, uint32_t port, int timeout) : _timeout(timeout) {
	_server_socket = create_server_socket(ip_mode, port);
	_pollfds.reserve(1);
	log_pollfd(_pollfds[0]); // log server socket
	_pollfds.push_back(_create_pollfd(_server_socket, POLLIN | POLLOUT));
}

void Poller::_accept_clients() {
	while (true) {
		int newfd = accept(_server_socket, NULL, NULL);
		if (newfd < 0 && errno != EWOULDBLOCK) // TODO errno is not allowed
			exit_with::e_perror("accept() failed");
		if (newfd < 0)
			break;

		// New incoming connection
		_pollfds.push_back(_create_pollfd(newfd, POLLIN | POLLOUT));
	}
}

#define FD_CLOSED -1
void Poller::_on_new_pollfd(pollfd& pfd, void (*on_request)(Request& request)) {
	_buffers.reserve(pfd.fd + 1);
	if (_buffers[pfd.fd].read_pollfd(pfd) != Buffer::DONE)
		return;
	Request r(pfd, _buffers[pfd.fd].data);
	on_request(r);
	_buffers[pfd.fd].reset();
	close(pfd.fd);
	pfd.fd = FD_CLOSED;
}

void Poller::start(void (*on_request)(Request& request)) {
	while (true) {
		int rc = poll(_pollfds.data(), _pollfds.size(), _timeout);
		if (rc < 0)
			exit_with::e_perror("poll() failed");
		if (rc == 0)
			exit_with::e_perror("poll() timeout");
		_accept_clients();
		for (std::vector<struct pollfd>::iterator fd = _pollfds.begin() + 1; fd != _pollfds.end(); ++fd) {
			if (fd->revents == 0)
				continue;
			log_pollfd(*fd); // log the event on the poll fd
			_on_new_pollfd(*fd, on_request);
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

struct pollfd Poller::_create_pollfd(int fd, short events) {
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	return pfd;
}

Poller::~Poller() {} // TODO: close fds etc.

// Buffer

Buffer::Buffer() { // TODO: disable
	reset();
}

// TODO: fix this horrible unstable abomination of what is not even allowed to be called "code"
Buffer::Status Buffer::read_pollfd(const pollfd& pfd) {
	static char		  buf[BUFFER_SIZE + 1];
	ssize_t			  bytes_read;
	const std::string cl = "Content-Length: ";

	while (true) {
		bytes_read = read(pfd.fd, buf, BUFFER_SIZE);
		if (bytes_read == 0)
			break;
		if (bytes_read < 0)
			return TEMPORALLY_UNIAVAILABLE;
		buf[bytes_read] = '\0';
		data += buf;

		_bytes_to_read -= bytes_read;
		size_t p;
		if (_status == UNSET && (p = data.find(cl)) != std::string::npos) {
			std::string len_str = std::string(data.begin() + p + cl.length(), data.begin() + data.find("\r\n", p));
			assert(parse_int(_bytes_to_read, len_str));
			_status = MULTIPART;
		}
		if (_status == MULTIPART && _bytes_to_read <= 0) { // TODO: what the fuck
			_status = DONE;
			return _status;
		}
		if (_status == UNSET && _bytes_to_read <= 0 && data.find("\r\n\r\n") != std::string::npos) {
			_status = DONE;
			return _status;
		}
	}
	if (_status == MULTIPART && pfd.revents & POLLOUT) { // TODO: do not repeat code
		std::string resp = "GET / HTTP/1.1 100 Continue\n\r\n\r";
		write(pfd.fd, resp.data(), resp.length());
	}
	return _status;
}

void Buffer::reset() {
	_status = UNSET;
	_bytes_to_read = -1;
	data = "";
	data.shrink_to_fit();
}
