#pragma once
#include "header_field.hpp"
#include "main.hpp"
#include <netinet/in.h>

class Request {
  public:
	Request();
	~Request();
	void							   parse_header(const pollfd& pfd, const std::string& raw);
	void							   parse_body(std::vector<char>& v);
	bool							   has_name(const std::string& name) const;
	std::string						   get_user_agent() const;
	size_t							   get_content_length() const;
	std::string						   get_transfer_encoding() const;
	fd_t							   get_fd() const;
	uint32_t						   get_response_code() const;
	std::map<std::string, std::string> get_request_line() const;
	std::vector<char>				   get_body() const;
	std::string						   get_value(const std::string& name) const;
	std::string						   get_value(const std::string& name, size_t index) const;
	void							   reset();
	// read-only variables
	std::vector<Header_field> header_fields;
	// template <typename T>
	// T Request::get_value(const std::string& name) const {
	// 	T value;

	// 	for (std::vector<Header_field>::const_iterator it = header_fields.begin(); it != header_fields.end(); ++it) {
	// 		if (it->name == name) {
	// 			if (it->type == integer)
	// 				assert(parse_int(content_length, it->values[0]));
	// 			else
	// 				value = it->values[0];
	// 			break;
	// 		}
	// 		assert(it != header_fields.end()); // if statement is false, assert
	// 	}
	// 	return value;
	// }

  private:
	fd_t							   _fd;
	std::string						   _raw;
	std::string						   _whitespaces;
	std::string						   _CRLF;
	std::string						   _vchar_no_delimiter;
	uint32_t						   _response_code;
	std::map<std::string, std::string> _request_line;
	std::vector<char>				   _body;
	int								   _parse_request_line();
	int								   _parse_header_fields();
	int								   _parse_field_values();
	int								   _duplicate_name(std::string name);
	std::string						   _str_tolower(std::string& str);
	int								   _set_code_and_return(int ret);

	// disabled
	// Request(const Request& cp); // TODO
	// Request& operator=(const Request& cp);
};

std::ostream& operator<<(std::ostream& output, Request const& rhs);
