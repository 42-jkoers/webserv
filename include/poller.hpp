#pragma once
#include "main.hpp"
#include "request.hpp"

class Poller {
  public:
	Poller(IP_mode ip_mode, uint32_t port, int timeout);
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
