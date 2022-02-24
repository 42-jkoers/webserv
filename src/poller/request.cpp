#include "request.hpp"

Request::Request(const pollfd& pfd, const std::string& raw) : _fd(pfd.fd) _raw(raw) {
	if (pfd.revents != POLLIN)
		exit_with::message("Unexpected revents value");
	_response_code = 200;
	_parse_request();
}

Request::~Request() {
}

bool Request::_is_end_of_http_request(const std::string& s) {
	if (s.find("\r\n\r\n") == std::string::npos)
		return 0;
	return 1;
}

// A recipient that receives whitespace between the start-line and the first header field MUST either reject
// the message as invalid or consume each whitespace-preceded line without further processing of it

// header-field = field-name ":" OWS field-value OWS
// each header field name is case-insensitive
// whitespace in between field name and ":": 400 (Bad Request)
// ows = optional whitespaces = *(SP / HTAB)
int Request::_parse_header_fields() {
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
	std::stringstream ss;
	std::string		  key;
	std::string		  value;
	int				  semi_colon;
	int				  sp;

	semi_colon = 0;
	sp = 0;
	ss << _raw;
	// while (ss >> key >> value) {
	// 	semi_colon = key.find_first_of(':');
	// 	if (semi_colon == std::string::npos)

	// 		_request_headers[key] = value;
	// }
	return 0;
}

int Request::_is_valid_request_line() { // TO DO: invalid request line: 400 bad request/301 moved permanently
	std::array<std::string, 3> methods = {"GET", "POST", "DELETE"};

	if (std::find(methods.begin(), methods.end(), _request_line["method"]) == methods.end()) // TO DO: check delete
		return 0;
	if (_request_line["HTTP_version"].compare("HTTP/1.1") != 0)
		return 0;
	return 1;
}

int Request::_set_code_and_return(int code) {
	_response_code = code;
	return 1;
}

// Request-Line = Method SP Request-URI SP HTTP-Version CRLF
int Request::_parse_request_line() {
	size_t					   end;
	size_t					   sp;
	std::array<std::string, 3> components = {"method", "URI", "HTTP_version"};

	end = _raw.find("\r\n");
	if (end == std::string::npos)
		return _set_code_and_return(301);
	std::string line = _raw.substr(0, end);

	for (size_t i = 0; i < components.size(); i++) {
		sp = line.find_first_of(' ');
		if ((components[i] != "HTTP_version" && sp == std::string::npos) ||
			(components[i] == "HTTP_version" && sp != std::string::npos))
			return _set_code_and_return(301);
		_request_line[components[i]] = line.substr(0, sp);
		line.erase(0, sp + 1);
	}
	if (!_is_valid_request_line())
		return _set_code_and_return(301);
	return 0;
}

void Request::_parse_request() {
	if (_parse_request_line() == 1)
		return;
	if (_parse_header_fields() == 1)
		return;
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

uint32_t Request::get_response_code() const {
	return _response_code;
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
