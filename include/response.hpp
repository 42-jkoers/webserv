#pragma once
#include "main.hpp"

class Response {
  public:
	Response(fd_t fd, int ret);
	~Response();
	void		send_response(const std::string& message);
	std::string get_index(Config& config);

  private:
	fd_t	  _fd;
	u_int32_t _response_code;

	// disabled
	Response(const Response& cp);
	Response& operator=(const Response& cp);
};
