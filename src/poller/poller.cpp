#include "poller.hpp"
#include "main.hpp"
#include "response.hpp"
#include <fcntl.h>
#include <limits>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>

struct sockaddr_in6 get_address6(uint16_t port) {
	struct sockaddr_in6 address;
	memset(&address, 0, sizeof(address));
	address.sin6_family = AF_INET6;
	memcpy(&address.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	address.sin6_port = htons(port);
	return address;
}

struct sockaddr_in get_address(uint16_t port) {
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	return address;
}

// returns fd to socket
static fd_t create_server_socket(IP_mode ip_mode, uint16_t port) {
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

Poller::Poller(IP_mode ip_mode, uint16_t port, int timeout) : _timeout(timeout) {
	_server_socket = create_server_socket(ip_mode, port);
	_pollfds.reserve(1);
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
	if (_buffers.size() < static_cast<size_t>(pfd.fd + 1))
		_buffers.resize(pfd.fd + 1); // do not change this to reserve(), that one does not call the constructors of the elements
	_buffers[pfd.fd].read_pollfd(pfd);
	if (_buffers[pfd.fd].parse_status() == Buffer::FINISHED) {
		_buffers[pfd.fd].print();
		Response response(pfd.fd, 200);
		response.send_response("Hello World!\n");

		_buffers[pfd.fd].reset();
		close(pfd.fd); // TODO: only when keepalive is true
		pfd.fd = FD_CLOSED;
	}
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

Buffer::Parse_status Buffer::parse_status() const { return _parse_status; }

// TODO: fix this horrible unstable abomination of what is not even allowed to be called "code"
Buffer::Read_status Buffer::read_pollfd(const pollfd& pfd) {
	static char buf[BUFFER_SIZE + 1];
	ssize_t		bytes_read;

	while (true) {
		bytes_read = read(pfd.fd, buf, BUFFER_SIZE);
		if (bytes_read == 0)
			break;
		if (bytes_read < 0)
			return TEMPORALLY_UNIAVAILABLE;
		buf[bytes_read] = '\0';
		// std::cout << buf << std::endl;
		// std::cout << "=======" << std::endl;
		_parse(buf, bytes_read);
		if (_parse_status <= HEADER_DONE)
			break;
		if (_parse_status == FINISHED)
			return _read_status;
	}
	if (_parse_status == HEADER_DONE &&
		pfd.revents & POLLOUT) {
		std::string resp = "HTTP/1.1 100 Continue\r\nHTTP/1.1 200 OK\r\n\r\n";
		write(pfd.fd, resp.data(), resp.length());
		_parse_status = WAITING_FOR_BODY;
	}
	return _read_status;
}
enum Chunk_status {
	CS_NO_NULL_BLOCK,
	CS_NULL_BLOCK_REACHED,
	CS_ERROR
};

Chunk_status append_chunk(std::vector<char>& body, char* buf, size_t buffer_size) {
	assert(buffer_size >= 5);
	if (!strcmp(buf, "0\r\n\r\n"))
		return CS_NULL_BLOCK_REACHED;

	size_t block_size;
	assert(parse_hex(block_size, buf, '\r'));
	assert(block_size < buffer_size);

	// remove length definition of block
	char* start = strchr(buf, '\r') + 2;
	buffer_size -= static_cast<size_t>(start - buf);
	buf = start;

	for (size_t i = 0; i < block_size; i++) // TODO
		body.push_back(buf[i]);
	buf += block_size + 2;
	buffer_size -= block_size + 2; // ignore last 2 bytes
	if (buffer_size)
		return append_chunk(body, buf, buffer_size);
	return CS_NO_NULL_BLOCK;
}

void Buffer::_parse(char* buf, ssize_t bytes_read) {
	if (_parse_status <= HEADER_IN_PROGRESS) {
		header += buf;
		const std::string cl = "Content-Length: ";
		size_t			  p;
		if ((p = header.find(cl)) != std::string::npos) {
			const size_t line_end = header.find("\r\n", p);
			assert(line_end != std::string::npos);
			std::string len_str = std::string(header.begin() + p + cl.length(), header.begin() + line_end);
			assert(parse_int(_bytes_to_read, len_str));
			_body_type = MULTIPART;
		}
		if (header.find("transfer-encoding:chunked") != std::string::npos) {
			_body_type = CHUNKED;
		}
		if (header.find("\r\n\r\n") != std::string::npos) {
			if (_body_type == EMPTY)
				_parse_status = FINISHED;
			else
				_parse_status = HEADER_DONE;
		}

		return;
	}
	if (_parse_status >= WAITING_FOR_BODY) {
		if (_body_type == MULTIPART) {
			for (ssize_t i = 0; i < bytes_read; i++) // TODO
				body.push_back(buf[i]);
			_bytes_to_read -= bytes_read;
			if (_bytes_to_read <= 0) { // TODO: what the fuck
				_parse_status = FINISHED;
				return;
			}
		}
		if (_body_type == CHUNKED) {
			Chunk_status cs = append_chunk(body, buf, bytes_read);
			assert(cs != CS_ERROR);
			if (cs == CS_NULL_BLOCK_REACHED)
				_parse_status = FINISHED;
		}
	}
}

void Buffer::print() const {
	std::cout << "========== Header ==============\n";
	std::cout << header;
	std::cout << "========== Body ================\n " << std::endl;
	write(1, body.data(), body.size());
	std::cout << "========== End Body ============\n";
	std::cout << std::endl;
}

void Buffer::reset() {
	_read_status = UNSET;
	_parse_status = INCOMPLETE;
	_body_type = EMPTY;
	_bytes_to_read = -1;
	header = "";
	body.clear();
	header.shrink_to_fit();
	body.shrink_to_fit();
}
