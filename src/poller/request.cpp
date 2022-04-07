#include "request.hpp"

Request::Request() {
	// std::cout << "Request constructor called" << std::endl;
	reset();
}

Request::~Request() {
}

void Request::reset() {
	response_code = 200;
	port = 8080; // default if no port is specified in host header field
	_crlf = "\r\n";
	_whitespaces = " \t";
	header_fields.clear();
}

void Request::parse_header(const pollfd& pfd, const std::string& raw) {
	reset();
	fd = pfd.fd;
	_raw = raw;
	if (_parse_request_line() == 1)
		return;
	if (_parse_header_fields() == 1)
		return;
	if (_parse_field_values() == 1)
		return;
	// TODO: some checks to another class; only request error codes here
	if (has_name("content-length") && get_content_length() < 0) {
		response_code = 400;
		return;
	}
	if (_parse_host() == 1)
		return;
}

/*
Header field parsing:

header-field	= field-name ":" OWS field-value OWS
field-name		= token
field-value		= *( field-content / obs-fold )
field-content	= field-vchar [ 1*( SP / HTAB ) field-vchar ]
field-vchar		= VCHAR / obs-text
obs-fold		= CRLF 1*( SP / HTAB ) field
				; obsolete line folding
				; see Section 3.2.4
*/
int Request::_parse_field_values() {
	size_t		comma;
	size_t		start;
	std::string value;

	for (std::map<std::string, Header_field>::iterator it = header_fields.begin(); it != header_fields.end(); ++it) {
		comma = 0;
		while (comma != std::string::npos) {
			start = comma;
			if (comma != 0)
				start++;
			comma = it->second.raw_value.find_first_of(",", start);				 // if comma is string::npos, all characters until the end of the string
			start = it->second.raw_value.find_first_not_of(_whitespaces, start); // skip optional whitespaces
			if (start == std::string::npos || start == comma)					 // if field value ends with only ',' or empty field value -> continue
				continue;
			value = it->second.raw_value.substr(start, comma - start);
			it->second.add_value(value);
		}
	}
	return 0;
}

/*
Host = uri-host [ ":" port ]

A server MUST respond with a 400 (Bad Request) status code to any
HTTP/1.1 request message that lacks a Host header field and to any
request message that contains more than one Host header field or a
Host header field with an invalid field-value
*/
int Request::_parse_host() {
	size_t colon;

	if (!has_name("host"))
		return _set_response_code(400);
	std::map<std::string, Header_field>::iterator it = header_fields.find("host");
	// check valid field-value
	if (it->second.size_values != 1)
		return _set_response_code(400);
	colon = it->second.values[0].find_first_of(":");
	it->second.host = it->second.values[0].substr(0, colon); // if colon is string::npos, all characters until the end of the string
	if (colon + 1 == std::string::npos) {
		it->second.host = it->second.values[0].substr(0, colon + 1);
	}
	if (colon != std::string::npos) {
		colon++;
		if (parse_int(it->second.port, it->second.values[0].substr(colon)) == 0)
			return _set_response_code(400);
	}
	return 0; // TODO: not sure if correct server name is needed
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

	end = _raw.find(_crlf);					  // skip request line
	while (end != _raw.find(_crlf + _crlf)) { // until last line
		// get line
		_whitespaces = " \t";
		start = end + 2;
		end = _raw.find(_crlf, start);
		line = _raw.substr(start, end - start);	 // line contains all up to \r\n
		if (line[0] == ' ' || line[0] == '\t') { // skip whitespace-preceded line
			continue;
		}
		// get name
		colon = line.find_first_of(":");
		name = line.substr(0, colon); // if colon is string::npos, all characters until the end of the string
		name = _str_tolower(name);
		if (name.compare("host") == 0) {
			if (has_name(name)) // if there is already a host in the header class, error
				return _set_response_code(400);
		}
		if (colon == std::string::npos) { // skip line without ':'
			continue;
		}
		if (line.find_first_of(_whitespaces) < colon) { // check for whitespace in between field name and ":": 400 (Bad Request)
			return _set_response_code(400);
		}
		// get value
		start = line.find_first_not_of(_whitespaces, colon + 1); // skip optional whitespaces
		if (start == std::string::npos) {						 // skip line without value
			continue;
		}
		value = line.substr(start, line.find_last_not_of(_whitespaces) - start + 1); // remove trailing whitespaces, values can be case-sensitive
		// save name and value
		Header_field new_header_field(name, value);
		header_fields[name] = new_header_field;
	}
	return 0;
}

int Request::_set_response_code(int code) {
	response_code = code;
	return 1;
}

/*
URI = scheme ":" ["//" authority] path ["?" query] ["#" fragment]
authority = [userinfo "@"] host [":" port]
request-URI: path ["?" query]
https://en.wikipedia.org/wiki/Uniform_Resource_Identifier
*/
int Request::_parse_URI() {
	method = _request_line["method"];
	uri_raw = _request_line["URI"];
	http_version = _request_line["HTTP_version"];
	size_t prev;

	prev = 0;
	if (uri_raw.find("/") != std::string::npos && uri_raw[0] == '/') { // origin form
		prev = uri_raw.find_first_of("?");
		path = uri_raw.substr(0, prev);
		if (prev == std::string::npos)
			return 0;
		prev++;
		queries = uri_raw.substr(prev);
	} else if (uri_raw.find(":") != std::string::npos && uri_raw[0] != ':') { // absolute form
		absolute_form += uri_raw;
	} else {
		return _set_response_code(400);
	}
	return 0;
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
	end = _raw.find(_crlf);
	if (end == std::string::npos)
		return _set_response_code(301);
	for (size_t i = 0; i < components.size(); i++) {
		delimiter = _raw.find_first_of(" \r", prev);
		if (components[i] != "HTTP_version" && delimiter == end)
			return _set_response_code(301);
		if (delimiter - prev <= 0)
			return _set_response_code(400); // TODO: if URI is empty -> error; but this should probably be checked later
		_request_line[components[i]] = _raw.substr(prev, delimiter - prev);
		prev = delimiter + 1;
	}
	if (delimiter != end)
		return _set_response_code(301);
	if (std::find(methods.begin(), methods.end(), _request_line["method"]) == methods.end()) // TODO: invalid request line, decide: 400 bad request/301 moved permanently
		return _set_response_code(301);
	if (_request_line["HTTP_version"].compare("HTTP/1.1") != 0)
		return _set_response_code(505); // TODO: generate representation why version is not supported & what is supported [RFC7231; 6.6.6]
	if (_parse_URI() == 1)
		return 1;
	return 0;
}

bool Request::has_name(const std::string& name) const { // is the name in one of the Header_fields
	if (header_fields.find(name) != header_fields.end())
		return 1;
	return 0;
}

bool Request::has_value(const std::string& name, const std::string& value) const { // is the value in one of the values of the Header_field's name
	if (!has_name(name))
		return false;
	for (std::vector<std::string>::const_iterator it = header_fields.find(name)->second.values.begin(); it != header_fields.find(name)->second.values.end(); ++it) {
		if (*it == value)
			return 1;
	}
	return 0;
}

std::string Request::get_value(const std::string& name) const {
	std::map<std::string, Header_field>::const_iterator it = header_fields.find(name);
	assert(it != header_fields.end());
	return it->second.values[0];
}

std::string Request::get_value(const std::string& name, size_t index) const {
	std::map<std::string, Header_field>::const_iterator it = header_fields.find(name);
	assert(it != header_fields.end());
	assert(index < it->second.size_values);
	return it->second.values[index];
}

size_t Request::get_content_length() const {
	size_t content_length;
	content_length = 0;
	std::map<std::string, Header_field>::const_iterator it = header_fields.find("content-length");
	assert(it != header_fields.end());
	assert(parse_int(content_length, it->second.values[0]));
	return content_length;
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

void Request::append_to_body(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end) {
	body.insert(body.end(), begin, end);
}

std::ostream& operator<<(std::ostream& output, Request const& rhs) {
	std::map<std::string, std::string> request_line = rhs.get_request_line();

	output << "Request line-------------" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = request_line.begin(); it != request_line.end(); ++it) {
		output << "[" << it->first << "]: "
			   << "[" << it->second << "]" << std::endl;
	}
	if (!rhs.path.empty()) {
		output << "Path: ";
		output << "[" << rhs.path << "]" << std::endl;
	}
	if (!rhs.queries.empty()) {
		output << "Queries: ";
		output << "[" << rhs.queries << "]" << std::endl;
	}
	if (!rhs.absolute_form.empty()) {
		output << "Absolute form: ";
		output << "[" << rhs.absolute_form << "]" << std::endl;
	}
	output << "Request header fields----" << std::endl;
	for (std::map<std::string, Header_field>::const_iterator it = rhs.header_fields.begin(); it != rhs.header_fields.end(); ++it) {
		output << "[" << it->first << "]:";
		for (std::vector<std::string>::const_iterator it2 = it->second.values.begin(); it2 != it->second.values.end(); ++it2) {
			output << " [" << *it2 << "]";
		}
		output << std::endl;
	}
	output << "Request body-------------" << std::endl;
	for (std::vector<char>::const_iterator it = rhs.body.begin(); it != rhs.body.end(); ++it) {
		output << *it << std::endl;
	}
	return output;
}
