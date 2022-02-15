#pragma once
#include "main.hpp"
#include <map>

class Request {
  public:
	Request(const pollfd& pfd);
	void		send_response(uint32_t response_code, const std::string& message);
	std::string raw;
	~Request();
	// getters
	std::string						   get_user_agent() const;
	std::map<std::string, std::string> get_request_line(void) const;
	std::map<std::string, std::string> get_request_header_fields(void) const;

  private:
	fd_t							   _fd;
	bool							   _is_end_of_http_request(const std::string& s);
	void							   _read_request(const pollfd& pfd);
	std::map<std::string, std::string> _request_line;
	std::map<std::string, std::string> _request_header_fields;

	// disabled
	Request(const Request& cp);
	Request& operator=(const Request& cp);
};

std::ostream& operator<<(std::ostream& output, Request const& rhs);
