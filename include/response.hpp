#pragma once
#include "main.hpp"

class Response {
  public:
	Response(fd_t fd, uint32_t ret);
	~Response();
	void send_response(const std::string& message);

  private:
	fd_t	 _fd;
	uint32_t _response_code;

	// disabled
	Response(const Response& cp);
	Response& operator=(const Response& cp);
};
