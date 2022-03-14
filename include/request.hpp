#pragma once
#include "main.hpp"
#include <netinet/in.h>

class Request {
  public:
	Request();

	void							   parse_header(const pollfd& pfd, const std::string& raw);
	std::string						   get_user_agent() const;
	std::map<std::string, std::string> get_request_headers() const;
	bool							   has_key(const std::string& key) const;
	size_t							   get_content_length() const;
	std::string						   get_transfer_encoding() const;
	fd_t							   get_fd() const;
	uint32_t						   get_response_code() const;

	void							   parse_body(std::vector<char>& v);

	std::map<std::string, std::string> get_request_line() const;
	std::map<std::string, std::string> get_body() const;

	~Request();

  private:
	fd_t							   _fd;
	std::string						   _raw;
	std::string						   _whitespaces;
	std::string						   _CRLF;
	bool							   _is_end_of_http_request(const std::string& s);
	void							   _parse_request();
	int								   _parse_request_line();
	int								   _parse_header_fields();
	int								   _set_code_and_return(int ret);
	void							   _skip_ws(size_t& i);
	uint32_t						   _response_code;
	std::map<std::string, std::string> _request_line;
	std::map<std::string, std::string> _request_headers;
	std::map<std::string, std::string> _body;

	// disabled
	// Request(const Request& cp); // TODO
	// Request& operator=(const Request& cp);
};

std::ostream& operator<<(std::ostream& output, Request const& rhs);
