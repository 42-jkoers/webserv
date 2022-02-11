#include "request.hpp"

// to do: do not set optional stuff?
// find out all args and which of them are optional
Request::Request(std::string m, std::string u, std::string v) {
	_request_line["method"] = m;
	_request_line["URI"] = u;
	_request_line["version"] = v;
	return;
}

Request::~Request(void) {
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
		std::cout << it->first << "		" << it->second << std::endl;
	}
	return output;
}

Request parse_request(std::string str) {
	Request r("GET", "localhost", "HTTP/1.1");

	std::cout << r << std::endl;
	(void)str;
	return r;
}
