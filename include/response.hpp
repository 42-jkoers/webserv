#pragma once
#include "main.hpp"

class Response {
  public:
	Response(fd_t fd);
	~Response();
	void send_response(uint32_t response_code, const std::string& message);

  private:
	fd_t _fd;

	// disabled
	Response(const Response& cp);
	Response& operator=(const Response& cp);
};
