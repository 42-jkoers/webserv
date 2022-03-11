#include "request.hpp"

Request::Request(const pollfd& pfd, const std::string& raw) : _fd(pfd.fd), _raw(raw) {
	if (pfd.revents != POLLIN)
		exit_with::message("Unexpected revents value ");
	_response_code = 200;
	_CRLF = "\r\n";
	_whitespaces = " \t";
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

// TODO: starting with ws and obs-fold difference: maybe difference in key and value (p25)
// field values are parsed after whole header section has been processed

// header-field = field-name ":" OWS field-value OWS
// each header field name is case-insensitive
// ows = optional whitespaces = *(SP / HTAB)
int Request::_parse_header_fields() { // TODO: enum parsing, add functions
	size_t		start;
	size_t		colon;
	size_t		end;
	std::string line;
	std::string name;
	std::string value;

	end = _raw.find(_CRLF);					  // skip request line
	while (end != _raw.find(_CRLF + _CRLF)) { // until last line
		// get line
		start = end + 2;
		end = _raw.find(_CRLF, start);
		line = _raw.substr(start, end - start);	 // line contains all up to \r\n
		if (line[0] == ' ' || line[0] == '\t') { // skip whitespace-preceded line
			continue;
		}
		// get name
		colon = line.find_first_of(":");
		if (colon == std::string::npos) { // skip line without ':'
			continue;
		}
		if (line.find_first_of(_whitespaces) < colon) { // check for whitespace in between field name and ":": 400 (Bad Request)
			return _set_code_and_return(400);
		}
		name = line.substr(0, colon);
		// get value
		start = line.find_first_not_of(_whitespaces, colon + 1); // skip optional whitespaces
		if (start == std::string::npos) {						 // skip line without value
			continue;
		}
		value = line.substr(start, line.find_last_not_of(_whitespaces) - start + 1); // remove trailing whitespaces
		// save name and value
		_request_headers[name] = value;
	}
	return 0;
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
	std::array<std::string, 3> methods = {"GET", "POST", "DELETE"};

	prev = 0;
	end = _raw.find(_CRLF);
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
	if (std::find(methods.begin(), methods.end(), _request_line["method"]) == methods.end()) // TODO: invalid request line, decide: 400 bad request/301 moved permanently
		return _set_code_and_return(301);
	if (_request_line["HTTP_version"].compare("HTTP/1.1") != 0)
		return _set_code_and_return(505); // TODO: generate representation why version is not supported & what is supported [RFC7231; 6.6.6]
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

// int Request::_parse_header_fields() { // TODO: enum parsing, add functions
// 	size_t		start;
// 	size_t		delimiter;
// 	size_t		end;
// 	size_t		ws;
// 	std::string key;
// 	std::string value;
// 	std::string line;

// 	end = _raw.find(_CRLF);					// skip request line
// 	while (end != _raw.find(_CRLF + _CRLF)) { // until last line
// 		// get line
// 		start = end + 2;
// 		end = _raw.find(_CRLF, start);
// 		line = _raw.substr(start, end - start);	 // line contains all up to \r\n
// 		if (line[0] == ' ' || line[0] == '\t') { // skip whitespace-preceded line
// 			continue;
// 		}

// 		// get key
// 		delimiter = line.find_first_of(":");
// 		if (delimiter == std::string::npos) {
// 			std::cout << "[" << line << "] Error: no ':' in header field or empty field" << std::endl; // TODO: last line gives error; should no delimiter error?
// 			continue;
// 		}
// 		ws = line.find_first_of(_whitespaces); // check for whitespace in between field name and ":": 400 (Bad Request)
// 		if (ws != std::string::npos && ws < delimiter) {
// 			std::cout << "[" << line << "] Error: ws between field name and ':'" << std::endl;
// 			return _set_code_and_return(400);
// 		}
// 		key = line.substr(0, delimiter);

// 		// get value
// 		start = line.find_first_not_of(_whitespaces, delimiter + 1); // skip optional whitespaces
// 		if (start == std::string::npos) {
// 			std::cout << "[" << line << "] Error: empty header field value" << std::endl;
// 			continue;
// 		}
// 		value = line.substr(start, line.find_last_not_of(_whitespaces) - start + 1); // remove trailing whitespaces
// 		std::cout << "[" << key << "] [" << value << "]" << std::endl;
// 		_request_headers[key] = value;
// 	}
// 	return 0;
// }

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
