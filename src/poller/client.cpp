#include "client.hpp"

Client::Client() { // TODO: disable
	reset();
}

Client::Parse_status Client::parse_status() const { return _parse_status; }

// TODO: fix this horrible unstable abomination of what is not even allowed to be called "code"
void Client::read_pollfd(const pollfd& pfd) {
	ssize_t		bytes_read;
	static char buf[4096];

	while (true) {
		bytes_read = read(pfd.fd, buf, sizeof(buf));
		if (bytes_read == 0)
			break;
		if (bytes_read < 0)
			return;
		for (ssize_t i = 0; i < bytes_read; i++)
			_buf.push_back(buf[i]);
		_parse(static_cast<size_t>(bytes_read), pfd);
		if (_parse_status <= HEADER_DONE)
			break;
		if (_parse_status == FINISHED)
			return;
	}
	if (_parse_status == HEADER_DONE &&
		pfd.revents & POLLOUT) {
		std::string resp = "HTTP/1.1 100 Continue\r\nHTTP/1.1 200 OK\r\n\r\n";
		write(pfd.fd, resp.data(), resp.length());
		_parse_status = WAITING_FOR_BODY;
	}
}

Client::Chunk_status Client::_append_chunk(size_t bytes_read) {
	size_t block_size;
	size_t hex_len = parse_hex(block_size, _buf.data(), '\r');
	assert(hex_len > 0); // if parse_hex is successful
	hex_len += 2;		 // also include \r\n suffix
	if (block_size == 0) {
		_buf.clear(); // TODO: end values
		return CS_NULL_BLOCK_REACHED;
	}
	if (block_size <= _buf.size() - hex_len) {
		request.append_to_body(_buf.begin() + hex_len, _buf.begin() + hex_len + block_size);
		_buf.erase(_buf.begin(), _buf.begin() + hex_len + block_size + 2); // also remove \r\n suffix
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
		} else if (request.has_name("transfer-encoding") && request.has_value("transfer-encoding", "chunked"))
			_body_type = CHUNKED;
		else {
			_parse_status = FINISHED;
		}
		return;
	}
	if (_parse_status >= WAITING_FOR_BODY) {
		if (_body_type == MULTIPART) {
			request.append_to_body(_buf.begin(), _buf.end());
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

void Client::reset() {
	_parse_status = INCOMPLETE;
	_body_type = EMPTY;
	_bytes_to_read = -1;
	_buf.clear();
}
