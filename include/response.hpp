#pragma once
#include "main.hpp"

class Response {
  public:
	Response(fd_t fd);
	~Response();
	void text(uint32_t code, const std::string& message);
	void cgi(const std::string& path, const std::string& path_info, const std::string& query_string);
	void file(const std::string& path);

  private:
	fd_t _fd;

	// disabled
	Response(const Response& cp);
	Response& operator=(const Response& cp);
};
