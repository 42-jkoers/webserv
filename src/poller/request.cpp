#include "request.hpp"

Request::Request(const pollfd& pfd, const std::string& raw) : raw(raw), _fd(pfd.fd) {
	if (pfd.revents != POLLIN)
		exit_with::message("Unexpected revents value");
	_ret = 0;
	_parse_request();
}

Request::~Request() {
}

bool Request::_is_end_of_http_request(const std::string& s) {
	if (s.find("\r\n\r\n") == std::string::npos)
		return 0;
	return 1;
}

int Request::_is_valid_request_line() { // TO DO: invalid reqeust line: 400 bad request/301 moved permanently
	std::array<std::string, 3> methods = {"GET", "POST", "DELETE"};

	if (std::find(methods.begin(), methods.end(), _request_line["method"]) == methods.end())
		return 0;
	if (_request_line["HTTP_version"].compare("HTTP/1.1") != 0)
		return 0;
	return 1;
}

int Request::_set_ret_and_return(int ret) {
	_ret = ret;
	return 1;
}

// Request-Line = Method SP Request-URI SP HTTP-Version CRLF
int Request::_parse_request_line() {
	size_t					   end;
	size_t					   sp;
	std::array<std::string, 3> components = {"method", "URI", "HTTP_version"};

	end = raw.find("\r\n");
	if (end == std::string::npos)
		return _set_ret_and_return(301);
	std::string line = raw.substr(0, end);

	for (size_t i = 0; i < components.size(); i++) {
		sp = line.find_first_of(' ');
		if (components[i] != "HTTP_version" && sp == std::string::npos)
			return _set_ret_and_return(301);
		else if (components[i] == "HTTP_version" && sp != std::string::npos)
			return _set_ret_and_return(301);
		_request_line[components[i]] = line.substr(0, sp);
		line.erase(0, sp + 1);
	}
	if (!_is_valid_request_line())
		return _set_ret_and_return(301);
	return 0;
}

void Request::_parse_request() {
	// std::stringstream ss; // do not use streams for parsing
	// std::string		  key;
	// std::string		  value;
	// ss << buf;
	// std::getline(ss, _request_line["method"], ' ');
	// std::getline(ss, _request_line["URI"], ' ');
	// std::getline(ss, _request_line["HTTP_version"]);
	// while (ss >> key >> value) {
	// 	_request_headers[key] = value;
	// }
	if (_parse_request_line() == 1)
		return;
	// _parse_header_fields();
}

// getters
std::map<std::string, std::string> Request::get_request_line() const {
	return _request_line;
}

std::map<std::string, std::string> Request::get_request_headers() const {
	return _request_headers;
}

std::map<std::string, std::string> Request::get_body() const {
	return _body;
}

int Request::get_ret() const {
	return _ret;
}

fd_t Request::get_fd() const {
	return _fd;
}

std::ostream& operator<<(std::ostream& output, Request const& rhs) {
	std::map<std::string, std::string> request_line = rhs.get_request_line();
	std::map<std::string, std::string> request_headers = rhs.get_request_headers();
	std::map<std::string, std::string> body = rhs.get_body();

	output << "Request line-------------" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = request_line.begin(); it != request_line.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	output << "Request header fields----" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = request_headers.begin(); it != request_headers.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	output << "Request body-------------" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = body.begin(); it != body.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return output;
}
