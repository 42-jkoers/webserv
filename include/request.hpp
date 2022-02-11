#pragma once

#include "main.hpp"
#include <map>

class Request {
  public:
	Request(std::string m, std::string u, std::string v);
	~Request(void);

	// getters
	std::map<std::string, std::string> get_request_line(void) const;
	std::map<std::string, std::string> get_request_header_fields(void) const;
	// setters
  private:
	std::map<std::string, std::string> _request_line;
	std::map<std::string, std::string> _request_header_fields;
};

std::ostream& operator<<(std::ostream& output, Request const& rhs);
Request		  parse_request(std::string str);
