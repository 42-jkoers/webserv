#include "request.hpp"

Request::Request(const pollfd& pfd, const std::string& raw) : _fd(pfd.fd), _raw(raw) {
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

void Request::_skip_ws(size_t& i) {
	while (_raw[i] == ' ' || _raw[i] == '\t') {
		i++;
	}
}

// A recipient that receives whitespace between the start-line and the first header field MUST either reject
// the message as invalid or consume each whitespace-preceded line without further processing of it

// header-field = field-name ":" OWS field-value OWS
// each header field name is case-insensitive
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
	size_t		start;
	size_t		delimiter;
	size_t		end;
	size_t		ws;
	std::string key;
	std::string value;
	std::string line;

	delimiter = 0;
	start = _raw.find("\r\n") + 2; // skip request line
	std::cout << _raw << std::endl;
	while (delimiter != std::string::npos) {
		end = _raw.find("\r\n", start);
		if (end == std::string::npos) {
			std::cout << "Empty request" << std::endl;
			return 0;
		}
		line = _raw.substr(start, end - start); // line contains all up to \r\n
		std::cout << "[" << line << "]" << std::endl;
		if (line[0] == ' ' || line[0] == '\t') { // skip whitespace-preceded line
			start = end + 2;
			continue;
		}
		delimiter = line.find_first_of(":");
		if (delimiter == std::string::npos) {
			std::cout << "Error: no delimiter in header field" << std::endl; // TO DO: last line gives error; should no delimiter error?
			start = end + 2;
			continue;
		}
		ws = line.find_first_of(" \t");
		if (ws != std::string::npos && ws < delimiter) { // whitespace in between field name and ":": 400 (Bad Request)
			std::cout << "Error: ws between field name and ':'" << std::endl;
			return _set_code_and_return(400);
		}
		key = line.substr(0, delimiter);
		start = line.find_first_not_of(" \t", delimiter + 1); // skip optional whitespaces
		if (start == std::string::npos) {
			std::cout << "Error: empty header field value" << std::endl;
			start = end + 2;
			continue;
		}
		ws = line.find_first_of(" \t", start);
		if (ws == std::string::npos) { // no OWS between : and key
			value = line.substr(start);
		} else { // OWS between : and key -> start after OWS
			value = line.substr(start, start - ws);
		}
		std::cout << "[" << key << "] [" << value << "]" << std::endl;
		_request_headers[key] = value;
		start = end + 2;
	}
	return 0;
}

int Request::_is_valid_request_line() { // TO DO: invalid request line, decide: 400 bad request/301 moved permanently
	std::array<std::string, 3> methods = {"GET", "POST", "DELETE"};

	if (std::find(methods.begin(), methods.end(), _request_line["method"]) == methods.end())
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
	size_t					   prev;
	size_t					   delimiter;
	std::string				   line;
	std::array<std::string, 3> components = {"method", "URI", "HTTP_version"};

	prev = 0;
	end = _raw.find("\r\n");
	if (end == std::string::npos)
		return _set_code_and_return(301);
	for (size_t i = 0; i < components.size(); i++) {
		delimiter = _raw.find_first_of(" \r", prev);
		if (components[i] != "HTTP_version" && delimiter == end)
			return _set_code_and_return(301);
		_request_line[components[i]] = _raw.substr(prev, delimiter - prev);
		prev = delimiter + 1;
	}
	if (delimiter != end)
		return _set_code_and_return(301);
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
