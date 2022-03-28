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
			Response response(pfd.fd, client.request.response_code);
			response.send_response("Error!\n"); // TODO
		}
		else
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

// Buffer

Client::Client() { // TODO: disable
	reset();
}

Client::Parse_status Client::parse_status() const { return _parse_status; }

// TODO: fix this horrible unstable abomination of what is not even allowed to be called "code"
Client::Read_status Client::read_pollfd(const pollfd& pfd) {
	ssize_t		bytes_read;
	static char buf[4096];

	while (true) {
		bytes_read = read(pfd.fd, buf, sizeof(buf));
		if (bytes_read == 0)
			break;
		if (bytes_read < 0)
			return TEMPORALLY_UNIAVAILABLE;
		for (ssize_t i = 0; i < bytes_read; i++)
			_buf.push_back(buf[i]);
		_parse(static_cast<size_t>(bytes_read), pfd);
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

Client::Chunk_status Client::_append_chunk(size_t bytes_read) {
	size_t block_size;
	size_t hex_len = parse_hex(block_size, _buf.data(), '\r');
	assert(hex_len > 0); // if parse_hex is successful
	if (block_size == 0) {
		_buf.clear(); // TODO: end values
		return CS_NULL_BLOCK_REACHED;
	}
	if (block_size <= _buf.size() - (hex_len + 2)) {
		_buf.erase(_buf.begin(), _buf.begin() + static_cast<ssize_t>(hex_len + 2));
		body.insert(body.end(), _buf.begin(), _buf.begin() + static_cast<ssize_t>(block_size));
		_buf.erase(_buf.begin(), _buf.begin() + static_cast<ssize_t>(block_size + 2));
	}
	if (_buf.size())
		return _append_chunk(bytes_read);
	return CS_IN_PROGRESS;
}

// void write_body_to_file(const std::string& root, const std::vector<char>& data) {
// }

void Client::_parse(size_t bytes_read, const pollfd& pfd) {
	if (_parse_status <= HEADER_IN_PROGRESS &&
		_buf.size() > 4 &&
		!strncmp(&_buf.data()[_buf.size() - 4], "\r\n\r\n", 4)) {
		request.parse_header(pfd, _buf.data());
		_buf.clear();
		_parse_status = HEADER_DONE;
		if (request.has_name("content-length")) {
			_body_type = MULTIPART;
			_bytes_to_read = static_cast<ssize_t>(request.get_content_length());
		} else if (request.has_value("transfer-encoding", "chunked"))
			_body_type = CHUNKED;
		else {
			_parse_status = FINISHED;
			print();
		}
		return;
	}
	if (_parse_status >= WAITING_FOR_BODY) {
		if (_body_type == MULTIPART) {
			body.insert(body.end(), _buf.begin(), _buf.end());
			_buf.clear();
			_bytes_to_read -= bytes_read;
			if (_bytes_to_read <= 0) { // TODO: what the fuck
				_parse_status = FINISHED;
				return;
			}
		}
		if (_body_type == CHUNKED) {
			Chunk_status cs = _append_chunk(bytes_read);
			assert(cs != CS_ERROR);
			if (cs == CS_NULL_BLOCK_REACHED)
				_parse_status = FINISHED;
		}
		return;
	}
	_body_type = EMPTY;
	_parse_status = HEADER_IN_PROGRESS;
}

void Client::print() const {
	std::cout << "========== Header ==============\n";
	std::cout << request << std::endl;
	std::cout << "========== Body ================\n " << std::endl;
	write(1, body.data(), body.size());
	std::cout << "========== End Body ============\n";
	std::cout << std::endl;
}

void Client::reset() {
	_read_status = UNSET;
	_parse_status = INCOMPLETE;
	_body_type = EMPTY;
	_bytes_to_read = -1;
	_buf.clear();
	body.clear();
	body.shrink_to_fit();
}
