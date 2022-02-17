#include "request.hpp"
#include "main.hpp"
#include "poller.hpp"
#include <map>
#include <netinet/in.h>

Request::Request(const pollfd& pfd, const std::string& raw) : raw(raw), _fd(pfd.fd) {
	_request_line["method"] = "GET";
	_request_line["URI"] = "locahost";
	_request_line["version"] = "HTTP/1.1";
	// _parse_request(pfd);
}

Request::~Request() {
}

std::map<std::string, std::string> Request::get_request_line(void) const {
	return _request_line;
}

std::map<std::string, std::string> Request::get_request_header_fields(void) const {
	return _request_header_fields;
}

std::ostream& operator<<(std::ostream& output, Request const& rhs) {
	std::map<std::string, std::string> rhs_request_line;

	rhs_request_line = rhs.get_request_line();
	output << "Request line: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = rhs_request_line.begin(); it != rhs_request_line.end(); ++it) {
		std::cout << it->first << " | " << it->second << std::endl;
	}
	return output;
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
