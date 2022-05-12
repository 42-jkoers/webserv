#include "client.hpp"

Client::Client(const std::string& ip, uint16_t port) {
	_parse_status = INCOMPLETE;
	_body_size = 0;
	_buf.clear();
	request = Request(ip, port);
}

Client::Parse_status Client::parse_status() const { return _parse_status; }

// TODO: fix this horrible unstable abomination of what is not even allowed to be called "code"
void Client::read_pollfd(const pollfd& pfd) {
	request.set_fd(pfd.fd);

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
		_parse(bytes_read);
		if (_parse_status == HEADER_DONE)
			break;
		if (_parse_status >= FINISHED)
			return;
	}
	if (_parse_status == HEADER_DONE && // TODO
		pfd.revents & POLLOUT) {
		std::string resp = "HTTP/1.1 100 Continue\r\nHTTP/1.1 200 OK\r\n\r\n";
		write(pfd.fd, resp.data(), resp.length());
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
		request.body.insert(request.body.end(), _buf.begin() + hex_len, _buf.begin() + hex_len + block_size);
		_buf.erase(_buf.begin(), _buf.begin() + hex_len + block_size + 2); // also remove \r\n suffix
	}
	if (_buf.size())
		return _append_chunk(bytes_read);
	return CS_IN_PROGRESS;
}

// void write_body_to_file(const std::string& root, const std::vector<char>& data) {
// }

bool request_has_body(const Request& request) {
	if (request.field_exists("content-length"))
		return true;
	if (request.field_is("transfer-encoding", "chunked"))
		return true;
	return false;
}

void Client::_parse(size_t bytes_read) {
	if (_parse_status <= READING_HEADER) {
		_parse_status = READING_HEADER;
		if (_buf.find("\r\n\r\n") != std::string::npos)
			_parse_status = READING_HEADER_DONE;
	}

	if (_parse_status == READING_HEADER_DONE) {
		request.parse_header(_buf.data());
		_buf.erase(_buf.begin(), _buf.begin() + _buf.find("\r\n\r\n") + 4);
		if (request.response_code != 200) {
			_parse_status = ERROR;
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

	if (_parse_status == READING_BODY_HEADER) {
		static const std::string header_end_str = "\r\n\r\n";
		size_t					 header_end = _buf.find(header_end_str);
		size_t					 start = 0;
		size_t					 end;
		std::string				 line;

		if (header_end == std::string::npos)
			return;
		while ((end = _buf.find("\r\n", start)) != header_end) {
			line = _buf.substr(start, end - start);
			request.parse_line(line, false);
			start = end + 2;
		}
		_buf.erase(_buf.begin(), _buf.begin() + header_end + header_end_str.size());
		_parse_status = READING_BODY;
	}

	if (_parse_status == READING_BODY && request.field_is("transfer-encoding", "chunked")) {
		Chunk_status cs = _append_chunk(bytes_read);
		assert(cs != CS_ERROR);
		if (cs == CS_NULL_BLOCK_REACHED)
			_parse_status = FINISHED;
		return;
	}

	if (_parse_status == READING_BODY && request.field_contains("content-type", "boundary=")) {
		size_t boundary = _buf.find(request.field_multipart_boundary());
		if (boundary != std::string::npos && boundary >= 4)
			boundary -= 4; // the boundary is prefixed with "\r\n--", remove that here
		request.body.insert(request.body.end(), _buf.begin(), _buf.begin() + std_ft::min(_buf.size(), boundary));
		_buf.clear();
		if (boundary != std::string::npos)
			_parse_status = FINISHED;
		return;
	}

	if (_parse_status == READING_BODY && request.field_exists("content-length")) {
		request.body.insert(request.body.end(), _buf.begin(), _buf.end());
		_body_size += _buf.size();
		if (_body_size >= request.body.size())
			_parse_status = FINISHED;
		_buf.clear();
		return;
	}
	if (_parse_status >= HEADER_DONE)
		std::cout << "body type not supported" << std::endl;
}
