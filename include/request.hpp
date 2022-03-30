#pragma once
#include "header_field.hpp"
#include "main.hpp"
#include <netinet/in.h>

class Request {
  public:
	Request();
	~Request();
	void							   parse_header(const pollfd& pfd, const std::string& raw);
	bool							   has_name(const std::string& name) const;
	size_t							   get_content_length() const;
	std::map<std::string, std::string> get_request_line() const;
	std::vector<char>				   get_body() const;
	std::string						   get_value(const std::string& name) const;
	std::string						   get_value(const std::string& name, size_t index) const;
	bool							   has_value(const std::string& name, const std::string& value) const;
	void							   reset();
	// read-only variables
	std::map<std::string, Header_field> header_fields;
	uint32_t							response_code;
	uint32_t							port;
	fd_t								fd;
	std::string							method;
	std::string							uri_raw;
	std::string							http_version;
	std::string							path;
	std::map<std::string, std::string>	queries;
	std::string							fragment;

  private:
	std::string						   _raw;
	std::string						   _whitespaces;
	std::string						   _crlf;
	std::string						   _vchar_no_delimiter;
	std::map<std::string, std::string> _request_line;
	std::vector<char>				   _body;
	int								   _parse_request_line();
	int								   _parse_header_fields();
	int								   _parse_field_values();
	int								   _parse_host();
	std::string						   _str_tolower(std::string& str);
	int								   _set_code_and_return(int ret);
	int								   _parse_URI();

	// disabled
	// Request(const Request& cp); // TODO
	// Request& operator=(const Request& cp);
};

std::ostream& operator<<(std::ostream& output, Request const& rhs);
