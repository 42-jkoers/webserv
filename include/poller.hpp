#pragma once
#include "main.hpp"
#include "request.hpp"

class Poller {
  public:
	Poller(IP_mode ip_mode, uint32_t port, int timeout);
	void start(void (*on_request)(Request& request), Config& config);
	~Poller();

  private:
	struct pollfd _create_pollfd(int fd, short events);
	void		  _accept_clients();

	enum Read_status {
		NOT_DONE,
		DONE
	};
	Read_status				   _read_request(const pollfd& pfd, std::string& buffer);
	void					   _on_new_pollfd(pollfd& pfd, void (*on_request)(Request& request));
	bool					   _is_end_of_http_request(const std::string& s);
	fd_t					   _server_socket;
	std::vector<struct pollfd> _pollfds;
	std::vector<std::string>   _buffers;
	int						   _timeout;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
