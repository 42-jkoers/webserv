#pragma once
#include "main.hpp"
#include <netinet/in.h>

class Request {
  public:
	Request(const pollfd& pfd, const std::string& raw);
	~Request();
	// getters
	std::string						   get_user_agent() const;
	std::map<std::string, std::string> get_request_line() const;
	std::map<std::string, std::string> get_request_headers() const;
	std::map<std::string, std::string> get_body() const;
	uint32_t						   get_response_code() const;
	fd_t							   get_fd() const;

  private:
	fd_t							   _fd;
	std::string						   _raw;
	bool							   _is_end_of_http_request(const std::string& s);
	void							   _parse_request();
	int								   _parse_request_line();
	int								   _parse_header_fields();
	int								   _is_valid_request_line();
	int								   _set_code_and_return(int ret);
	void							   _skip_ws(size_t& i);
	uint32_t						   _response_code;
	std::map<std::string, std::string> _request_line;
	std::map<std::string, std::string> _request_headers;
	std::map<std::string, std::string> _body;

	// disabled
	Request(const Request& cp);
	Request& operator=(const Request& cp);
};

std::ostream& operator<<(std::ostream& output, Request const& rhs);
