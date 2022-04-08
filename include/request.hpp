#pragma once
#include "header_field.hpp"
#include "main.hpp"
#include <netinet/in.h>

class Request {
  public:
	Request();
	~Request();
	void							   parse_header(const pollfd& pfd, const std::string& raw);
	void							   append_to_body(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);

	bool							   field_exits(const std::string& field) const;
	const Header_field&				   field(const std::string& field) const;
	const std::string&				   field_value(const std::string& _field, size_t index = 0) const;
	bool							   field_is(const std::string& field, const std::string& value) const;
	bool							   field_contains(const std::string& field, const std::string& part) const;
	size_t							   field_content_length() const;

	std::map<std::string, std::string> get_request_line() const { return _request_line; };
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
	std::string							absolute_form;
	std::string							queries;
	std::string							fragment;
	size_t								server_index;
	std::vector<char>					body;

  private:
	std::string						   _raw;
	std::string						   _whitespaces;
	std::string						   _crlf;
	std::map<std::string, std::string> _request_line;
	int								   _parse_request_line();
	int								   _parse_header_fields();
	int								   _parse_field_values();
	int								   _parse_host();
	int								   _set_response_code(int ret);
	int								   _parse_URI();

	// disabled
	// Request(const Request& cp); // TODO
	// Request& operator=(const Request& cp);
};

std::ostream& operator<<(std::ostream& output, Request const& rhs);
