#include "client.hpp"

Client::Client(const std::string& ip, uint16_t port) {
	_parse_status = INCOMPLETE;
	_body_size = 0;
	_buf_read.clear();
	_buf_write.clear();
	request = Request(ip, port);
}

Client::Parse_status Client::parse_status() const {
	return _parse_status;
}

void Client::on_pollevent(struct pollfd pfd) {
	if (_parse_status == ERROR)
		return;
	if (request.response_code != 200) {
		_parse_status = FINISHED;
		return;
	}
	if (pfd.revents & POLLIN)
		on_pollevent_read(pfd);
}

void Client::on_pollevent_read(struct pollfd pfd) {
	request.set_fd(pfd.fd);

	static char buf[4096];
	ssize_t		bytes_read = read(pfd.fd, buf, sizeof(buf));

	if (bytes_read < 0) {
		_parse_status = ERROR;
		return;
	}
	if (bytes_read == 0) {
		_parse_status = _parse_status >= HEADER_DONE ? FINISHED : ERROR;
		return;
	}
	for (ssize_t i = 0; i < bytes_read; i++)
		_buf_read.push_back(buf[i]);
	_parse();
	if (_parse_status >= FINISHED)
		return;

	if (_parse_status == HEADER_DONE &&
		pfd.revents & POLLOUT) {
		std::string resp = "HTTP/1.1 100 Continue\r\nHTTP/1.1 200 OK\r\n\r\n";
		if (write(pfd.fd, resp.data(), resp.length()) != (ssize_t)resp.length()) {
			_parse_status = ERROR;
			return;
		}
	}
}

Client::Chunk_status Client::_append_chunk() {
	if (_buf_read == "\r\n" && request.body.size() == 0) {
		_buf_read.clear();
		return CS_IN_PROGRESS;
	}
	size_t block_size;
	size_t hex_len = parse_hex(block_size, _buf_read.data(), '\r');
	if (hex_len <= 0) {
		request.response_code = 400;
		return CS_ERROR;
	}
	hex_len += 2; // also include \r\n suffix
	if (block_size == 0) {
		_buf_read.clear();
		return CS_NULL_BLOCK_REACHED;
	}
	if (block_size <= _buf_read.size() - hex_len) {
		size_t boundary = ~0;
		if (request.field_contains("content-type", "boundary=")) {
			boundary = _buf_read.find(request.field_multipart_boundary());
			if (boundary != std::string::npos && boundary >= 8)
				boundary -= 8;
		}

		request.body.insert(request.body.end(),
							_buf_read.begin() + hex_len,
							_buf_read.begin() + hex_len + std_ft::min(block_size, boundary));
		_buf_read.erase(_buf_read.begin(), _buf_read.begin() + hex_len + block_size + 2); // also remove \r\n suffix
	} else
		return CS_IN_PROGRESS;

	if (_buf_read.size() && _buf_read.find("\r\n") != std::string::npos)
		return _append_chunk();
	return CS_IN_PROGRESS;
}

bool request_has_body(const Request& request) {
	if (request.field_exists("content-length"))
		return true;
	if (request.field_is("transfer-encoding", "chunked"))
		return true;
	return false;
}

void Client::_parse() {
	if (_parse_status <= READING_HEADER) {
		_parse_status = READING_HEADER;
		if (_buf_read.find("\r\n\r\n") != std::string::npos)
			_parse_status = READING_HEADER_DONE;
		else
			return;
	}

	if (_parse_status == READING_HEADER_DONE) {
		request.parse_header(_buf_read.data());
		_buf_read.erase(_buf_read.begin(), _buf_read.begin() + _buf_read.find("\r\n\r\n") + 4);
		if (request.response_code != 200) {
			_parse_status = FINISHED;
			return;
		}
		if (!request_has_body(request)) {
			_parse_status = FINISHED;
			return;
		}
		_parse_status = HEADER_DONE;
	}

	if (_parse_status == HEADER_DONE) {
		if (request.field_contains("content-type", "boundary="))
			_parse_status = READING_BODY_HEADER;
		else
			_parse_status = READING_BODY;
	}

	_body_size += _buf_read.size();
	if (_body_size > request.associated_server().client_max_body_size) {
		request.response_code = 413;
		_parse_status = FINISHED;
		return;
	}
	if (_parse_status == READING_BODY_HEADER) {
		static const std::string header_end_str = "\r\n\r\n";
		size_t					 header_end = _buf_read.find(header_end_str);
		size_t					 start = 0;
		size_t					 end;
		std::string				 line;

		if (header_end == std::string::npos)
			return;
		while ((end = _buf_read.find("\r\n", start)) != header_end) {
			line = _buf_read.substr(start, end - start);
			request.parse_line(line, false);
			start = end + 2;
		}
		_buf_read.erase(_buf_read.begin(), _buf_read.begin() + header_end + header_end_str.size());
		_parse_status = READING_BODY;
	}

	if (_buf_read.size() == 0)
		return;

	if (_parse_status == READING_BODY && request.field_is("transfer-encoding", "chunked")) {
		Chunk_status cs = _append_chunk();
		if (cs == CS_NULL_BLOCK_REACHED || cs == CS_ERROR)
			_parse_status = FINISHED;
		return;
	}

	if (_parse_status == READING_BODY && request.field_contains("content-type", "boundary=")) {
		size_t boundary = _buf_read.find(request.field_multipart_boundary());
		if (boundary != std::string::npos && boundary >= 4)
			boundary -= 4; // the boundary is prefixed with "\r\n--", remove that here
		request.body.insert(request.body.end(), _buf_read.begin(), _buf_read.begin() + std_ft::min(_buf_read.size(), boundary));
		_buf_read.clear();
		if (boundary != std::string::npos)
			_parse_status = FINISHED;
		return;
	}

	if (_parse_status == READING_BODY && request.field_exists("content-length")) {
		request.body.insert(request.body.end(), _buf_read.begin(), _buf_read.end());
		if (_body_size >= request.body.size())
			_parse_status = FINISHED;
		_buf_read.clear();
		return;
	}
	if (_parse_status >= HEADER_DONE)
		std::cout << "body type not supported" << std::endl;
}
