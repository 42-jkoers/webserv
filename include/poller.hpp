#pragma once
#include "main.hpp"

class Request {
  public:
	Request(const pollfd& pfd);
	void		send_response(uint32_t response_code, const std::string& message);
	std::string get_user_agent();
	std::string get_();


	std::string raw;

  private:
	fd_t _fd;
	bool _is_end_of_http_request(const std::string& s);

	// disabled
	Request(const Request& cp);
	Request& operator=(const Request& cp);
};

fd_t create_server_socket(IP_mode ip_mode, uint32_t port);
class Poller {
  public:
	Poller(fd_t server_socket, int timeout);
	void start(void (*on_request)(Request& request));
	~Poller();

  private:
	struct pollfd			   _create_pollfd(int fd, short events);
	void					   _accept_clients();
	fd_t					   _server_socket;
	std::vector<struct pollfd> _pollfds;
	int						   _timeout;
	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
