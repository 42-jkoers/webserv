#include "request.hpp"
#include "constants.hpp"
#include "router.hpp"

Request::Request(const std::string& ip, uint16_t port) {
	response_code = 200;
	fd = -1;
	this->ip = ip;
	this->port = port;
	_crlf = "\r\n";
	_whitespaces = " \t";
	header_fields.clear();
}

// TODO: add absolute form checker: should start with http://, otherwise: bad request
void Request::parse_header(const std::string& raw) {
	_raw = raw;
	if (_parse_request_line() == 1)
		return;
	if (_parse_header_fields() == 1)
		return;
	if (_parse_field_values() == 1)
		return;
	if (field_exists("content-length")) {
		ssize_t content_length;
		if (!parse_int(content_length, field_value("content-length")) || content_length < 0) {
			response_code = 400;
			return;
		}
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

	if (!field_exists("host"))
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

int Request::parse_line(const std::string& line, bool overwrite) {
	size_t		colon;
	std::string name;
	std::string value;
	size_t		start;

	// get name
	colon = line.find_first_of(":");
	name = line.substr(0, colon); // if colon is string::npos, all characters until the end of the string
	name = to_lower(name);
	if (name.compare("host") == 0) {
		if (field_exists(name)) // if there is already a host in the header class, error
			return _set_response_code(400);
	}
	if (colon == std::string::npos) { // skip line without ':'
		return 0;
	}
	if (line.find_first_of(_whitespaces) < colon) { // check for whitespace in between field name and ":": 400 (Bad Request)
		return _set_response_code(400);
	}
	if (!overwrite && field_exists(name))
		return 0;
	// get value
	start = line.find_first_not_of(_whitespaces, colon + 1); // skip optional whitespaces
	if (start == std::string::npos) {						 // skip line without value
		return 0;
	}
	value = line.substr(start, line.find_last_not_of(_whitespaces) - start + 1); // remove trailing whitespaces, values can be case-sensitive
	// save name and value
	Header_field new_header_field(name, value);
	header_fields[name] = new_header_field;
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
	size_t		end;
	std::string line;

	end = _raw.find(_crlf);					  // skip request line
	while (end != _raw.find(_crlf + _crlf)) { // until last line
		_whitespaces = " \t";
		start = end + 2;
		end = _raw.find(_crlf, start);
		line = _raw.substr(start, end - start);	 // line contains all up to \r\n
		if (line[0] == ' ' || line[0] == '\t') { // skip whitespace-preceded line
			continue;
		}
		if (parse_line(line))
			return 1;
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
origin form: URI from path onwards
absolute form: URI including scheme onwards
for HTTP // are required
https://en.wikipedia.org/wiki/Uniform_Resource_Identifier
*/
int Request::_parse_URI() {
	method = _request_line["method"];
	uri = _request_line["URI"];
	http_version = _request_line["HTTP_version"];
	size_t prev;

	prev = 0;
	if (uri.find("/") != std::string::npos && uri[0] == '/') { // origin form
		prev = uri.find_first_of("?");
		path = uri.substr(0, prev);
		if (prev == std::string::npos)
			return 0;
		prev++;
		query = uri.substr(prev);
	} else if (uri.find(":") != std::string::npos && uri[0] != ':') { // absolute form
		absolute_form += uri;
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

	components.push_back("method");
	components.push_back("URI");
	components.push_back("HTTP_version");
	prev = 0;
	end = _raw.find(_crlf);
	if (end == std::string::npos)
		return _set_response_code(400);
	for (size_t i = 0; i < components.size(); i++) {
		delimiter = _raw.find_first_of(" \r", prev);
		if (components[i] != "HTTP_version" && delimiter == end)
			return _set_response_code(400);
		if (delimiter - prev <= 0)
			return _set_response_code(400); // TODO: if URI is empty -> error; but this should probably be checked later
		_request_line[components[i]] = _raw.substr(prev, delimiter - prev);
		prev = delimiter + 1;
	}
	if (delimiter != end)
		return _set_response_code(400);
	if (!g_constants.is_valid_method(_request_line["method"])) // TODO: invalid request line, decide: 400 bad request/301 moved permanently
		return _set_response_code(400);
	if (_request_line["HTTP_version"].compare("HTTP/1.1") != 0)
		return _set_response_code(505); // TODO: generate representation why version is not supported & what is supported [RFC7231; 6.6.6]
	if (_parse_URI() == 1)
		return 1;
	return 0;
}

bool Request::field_exists(const std::string& field) const {
	return header_fields.find(to_lower(field)) != header_fields.end();
}

// only call this function if you know field_exists
const Header_field& Request::field(const std::string& _field) const {
	assert(field_exists(_field));
	return header_fields.find(to_lower(_field))->second;
}

const std::string& Request::field_value(const std::string& _field) const {
	return field(_field).raw_value;
}

// only call this function if you know field_exists
const std::string& Request::field_value(const std::string& _field, size_t index) const {
	return field(_field).values.at(index);
}

bool Request::field_is(const std::string& _field, const std::string& value) const {
	if (!field_exists(value))
		return false;
	return field(_field).raw_value == value;
}

// can be called if field does not exists
bool Request::field_contains(const std::string& _field, const std::string& part) const {
	if (!field_exists(_field))
		return false;

	return field(_field).raw_value.find(part) != std::string::npos;
}

ssize_t Request::field_content_length() const {
	ssize_t content_length;
	assert(parse_int(content_length, field_value("content-length")));
	return content_length;
}

std::string Request::field_multipart_boundary() const {
	static const std::string prefix = "boundary=";
	assert(field_contains("content-type", prefix));
	size_t start = field_value("content-type").find(prefix) + prefix.size();
	size_t length = field_value("content-type").find("\r\n", start);
	return field_value("content-type").substr(start, length);
}

// returns empty string if filename is not found
std::string Request::field_filename() const {
	static const std::string prefix = "filename=\"";
	if (!field_contains("content-disposition", prefix))
		return "";
	size_t start = field_value("content-disposition").find(prefix) + prefix.size();
	size_t length = field_value("content-disposition").find("\"", start);
	return field_value("content-disposition").substr(start, length - start);
}

const Config::Server& Request::associated_server() const {
	assert(field_exists("host"));
	return g_router.find_server(port, field_value("host"));
}

const std::string Request::associated_server_name(std::vector<std::string> server_names) const {
	std::string		   host = field_value("host");
	static std::string empty = "";
	for (std::string& name : server_names) {
		if (name == host)
			return name;
	}
	if (server_names.empty()) {
		return empty;
	}
	return server_names.at(0);
}

const Config::Location& Request::associated_location() const {
	return g_router.find_location(path, associated_server());
}

std::ostream& operator<<(std::ostream& output, Request const& rhs) {
	std::cout << "================== REQUEST ==================" << std::endl;

	output << "uri:           [" << rhs.uri << "]" << std::endl;
	output << "method:        [" << rhs.method << "]" << std::endl;
	output << "path:          [" << rhs.path << "]" << std::endl;
	output << "ip:            [" << rhs.ip << "]" << std::endl;
	output << "port:          [" << rhs.port << "]" << std::endl;
	output << "query:         [" << rhs.query << "]" << std::endl;
	output << "absolute_form: [" << rhs.absolute_form << "]" << std::endl;
	output << "boundary       [" << (rhs.field_contains("content-type", "boundary=") ? rhs.field_multipart_boundary() : "") << "]" << std::endl;
	output << "http_version:  [" << rhs.http_version << "]" << std::endl;
	output << "response_code: [" << rhs.response_code << "]" << std::endl;

	output << "\n=== HEADERS ===" << std::endl;
	for (std::map<std::string, Header_field>::const_iterator it = rhs.header_fields.begin(); it != rhs.header_fields.end(); ++it) {
		output << "[" << it->first << "]:";
		for (std::vector<std::string>::const_iterator it2 = it->second.values.begin(); it2 != it->second.values.end(); ++it2) {
			output << " [" << *it2 << "]";
		}
		output << std::endl;
	}
	output << "==== BODY =====" << std::endl;
	print_escaped(rhs.body.data(), rhs.body.size());
	output << "================ REQUEST END ================\n " << std::endl;

	return output;
}
