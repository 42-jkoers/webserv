#pragma once
#include "main.hpp"

class Response {
  public:
	Response(fd_t fd);
	~Response();
	void send_response(uint32_t code, const std::string& message);
	void send_cgi(const std::string& path, const std::string& path_info, const std::string& query_string);

  private:
	fd_t _fd;

	// disabled
	Response(const Response& cp);
	Response& operator=(const Response& cp);
};
