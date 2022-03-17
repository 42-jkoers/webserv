#include "request.hpp"

Request::Request() {
	std::cout << "Request constructor called" << std::endl;
	_reset();
}

Request::~Request() {
}

void Request::_reset() {
	_response_code = 200;
	_CRLF = "\r\n";
	_whitespaces = " \t";
	_vchar_no_delimiter = "abcdefghijklmnopqrstuvwxyz0123456789!#$%&’*+-.^_‘| ̃";
}

void Request::parse_header(const pollfd& pfd, const std::string& raw) {
	_reset();
	_fd = pfd.fd;
	_raw = raw;
	if (_parse_request_line() == 1)
		return;
	if (_parse_header_fields() == 1)
		return;
	if (_parse_field_values() == 1)
		return;
}

int Request::_parse_field_values() {
	return 0;
}

int Request::_duplicate_name(std::string name) {
	for (std::vector<Header_field>::const_iterator it = _header_fields.begin(); it != _header_fields.end(); ++it) {
		if (it->_name == name)
			return 1;
	}
	return 0;
}

/*
A recipient that receives whitespace between the start-line and the first header field MUST either reject
the message as invalid or consume each whitespace-preceded line without further processing of it

TODO: starting with ws and obs-fold difference: maybe difference in key and value (p25)
field values are parsed after whole header section has been processed

header-field = field-name ":" OWS field-value OWS
each header field name is case-insensitive
ows = optional whitespaces = *(SP / HTAB)
*/
int Request::_parse_header_fields() { // TODO: set return code and return in case of error
	size_t		start;
	size_t		colon;
	size_t		end;
	std::string line;
	std::string name;
	std::string value;

	end = _raw.find(_CRLF);					  // skip request line
	while (end != _raw.find(_CRLF + _CRLF)) { // until last line
		// get line
		_whitespaces = " \t";
		start = end + 2;
		end = _raw.find(_CRLF, start);
		line = _raw.substr(start, end - start);	 // line contains all up to \r\n
		if (line[0] == ' ' || line[0] == '\t') { // skip whitespace-preceded line
			continue;
		}
		// get name
		colon = line.find_first_of(":");
		name = line.substr(0, colon); // if colon is string::npos, all characters until the end of the string
		name = _str_tolower(name);
		if (name.compare("host") == 0) {
			if (_duplicate_name(name)) // if there is already a host in the header class, error
				return _set_code_and_return(400);
		}
		if (colon == std::string::npos) { // skip line without ':'
			continue;
		}
		if (line.find_first_of(_whitespaces) < colon) { // check for whitespace in between field name and ":": 400 (Bad Request)
			return _set_code_and_return(400);
		}
		// get value
		start = line.find_first_not_of(_whitespaces, colon + 1); // skip optional whitespaces
		if (start == std::string::npos) {						 // skip line without value
			continue;
		}
		value = line.substr(start, line.find_last_not_of(_whitespaces) - start + 1); // remove trailing whitespaces
		// value = _str_tolower(value); // values may be case-sensitive?
		// save name and value
		Header_field new_header_field(name, value);
		_header_fields.push_back(new_header_field);
	}
	return 0;
}

int Request::_set_code_and_return(int code) {
	_response_code = code;
	return 1;
}

// Request-Line = Method SP Request-URI SP HTTP-Version CRLF
int Request::_parse_request_line() {
	size_t					 end;
	size_t					 prev;
	size_t					 delimiter;
	std::string				 line;
	std::vector<std::string> components;
	std::vector<std::string> methods;

	components.push_back("method");
	components.push_back("URI");
	components.push_back("HTTP_version");
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
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

bool Request::has_name(const std::string& name) const { // is the key in one of the Header_fields
	// iterate over Header_fields and get names.
	for (std::vector<Header_field>::const_iterator it = _header_fields.begin(); it != _header_fields.end(); ++it) {
		if (it->_name == name)
			return 1;
	}
	return 0;
}

std::string Request::_str_tolower(std::string& str) {
	char		c;
	int			i;
	std::string new_str;

	i = 0;
	while (str[i]) {
		c = str[i];
		c = std::tolower(c);
		new_str += c;
		i++;
	}
	return new_str;
}

// getters
std::map<std::string, std::string> Request::get_request_line() const {
	return _request_line;
}

std::vector<char> Request::get_body() const {
	return _body;
}
uint32_t Request::get_response_code() const {
	return _response_code;
}

fd_t Request::get_fd() const {
	return _fd;
}

size_t Request::get_content_length() const {
	size_t content_length;
	content_length = 0;
	for (std::vector<Header_field>::const_iterator it = _header_fields.begin(); it != _header_fields.end(); ++it) {
		if (it->_name == "content-length") {
			assert(parse_int(content_length, it->_name));
			break;
		}
		assert(it != _header_fields.end()); // if statement is false, assert
	}
	return content_length;
}

std::string Request::get_value(const std::string& name) const {
	std::string value;
	for (std::vector<Header_field>::const_iterator it = _header_fields.begin(); it != _header_fields.end(); ++it) {
		if (it->_name == name) {
			value = it->_values[0];
			break;
		}
		assert(it != _header_fields.end()); // if statement is false, assert
	}
	return value;
}

std::string Request::get_value(const std::string& name, size_t index) const {
	std::string value;
	for (std::vector<Header_field>::const_iterator it = _header_fields.begin(); it != _header_fields.end(); ++it) {
		if (it->_name == name) {
			value = it->_values[index];
			break;
		}
		assert(it != _header_fields.end()); // if statement is false, assert
	}
	return value;
}

std::ostream& operator<<(std::ostream& output, Request const& rhs) {
	std::map<std::string, std::string> request_line = rhs.get_request_line();
	std::vector<char>				   body = rhs.get_body();

	output << "Request line-------------" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = request_line.begin(); it != request_line.end(); ++it) {
		output << it->first << ": " << it->second << std::endl;
	}
	output << "Request header fields----" << std::endl;
	for (std::vector<Header_field>::const_iterator it = rhs._header_fields.begin(); it != rhs._header_fields.end(); ++it) {
		output << it->_name << ":";
		output << " " << it->_raw_value;
		for (size_t i = 0; i < it->_size_values; i++) {
			output << " [" << it->_values[i] << "]";
		}
		output << std::endl;
	}
	output << "Request body-------------" << std::endl;
	for (std::vector<char>::const_iterator it = body.begin(); it != body.end(); ++it) {
		output << *it << std::endl;
	}
	return output;
}
