#pragma once
#include "main.hpp"

class Request {
  public:
	Request(const pollfd& pfd, const std::string& raw);
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

enum Read_status {
	NOT_DONE,
	DONE
};

class Poller {
  public:
	Poller(IP_mode ip_mode, uint32_t port, int timeout);
	void start(void (*on_request)(Request& request));
	~Poller();

  private:
	struct pollfd			   _create_pollfd(int fd, short events);
	void					   _accept_clients();
	Read_status				   _read_request(const pollfd& pfd, std::string& buffer);
	fd_t					   _server_socket;
	std::vector<struct pollfd> _pollfds;
	std::vector<std::string>   _buffers;
	int						   _timeout;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
