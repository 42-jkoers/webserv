#include "main.hpp"
#include "poller.hpp"
#include <map>
#include <netinet/in.h>

Request::Request(const pollfd& pfd) : _fd(pfd.fd) {
	if (pfd.revents != POLLIN)
		exit_with::message("Unexpected revents value");

	static char buf[BUFFER_SIZE + 1];
	ssize_t		bytes_read;
	do {
		bytes_read = read(pfd.fd, buf, BUFFER_SIZE);
		if (bytes_read == -1)
			exit_with::e_perror("Cannot read from fd");
		if (bytes_read == 0)
			break;
		buf[bytes_read] = '\0';
		this->raw += buf;
	} while (!_is_end_of_http_request(this->raw));
}

bool Request::_is_end_of_http_request(const std::string& s) { // TODO: better?
	if (s.size() < 4)
		return true;
	return strncmp(s.data() + (s.size() - 4), "\r\n\r\n", 4) == 0;
}

// TODO: optimize
void Request::send_response(uint32_t response_code, const std::string& message) {
	std::map<uint32_t, std::string> m; // TODO: make this static
	m[200] = "OK";
	m[201] = "Created";
	m[202] = "Accepted";
	m[204] = "No Content";
	m[301] = "Moved Permanently";
	m[302] = "Moved Temporarily";
	m[304] = "Not Modified";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[500] = "Internal Server Error";
	m[501] = "Not Implemented";
	m[502] = "Bad Gateway";
	m[503] = "Service Unavailable";

	std::string response = "HTTP/1.1 ";
	response += cpp11::to_string(response_code);
	response += " ";
	response += m[response_code];
	response += "\r\n\r\n";
	response += message;
	write(_fd, response.c_str(), response.length()); // TODO: error hadling
}
