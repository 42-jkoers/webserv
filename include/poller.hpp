#pragma once
#include "main.hpp"
#include "request.hpp"

class Buffer {
  public:
	Buffer();
	enum Status {
		UNSET,
		NORMAL,
		MULTIPART,
		DONE
	};
	Status		read_pollfd(const pollfd& pfd);
	void		reset();

	std::string data;

  private:
	Status	_status;
	ssize_t _bytes_to_read;
	bool	_is_end_of_http_request(const std::string& s);
};

class Poller {
  public:
	Poller(IP_mode ip_mode, uint32_t port, int timeout);
	void start(void (*on_request)(Request& request));
	~Poller();

  private:
	struct pollfd			   _create_pollfd(int fd, short events);
	void					   _accept_clients();

	void					   _on_new_pollfd(pollfd& pfd, void (*on_request)(Request& request));
	fd_t					   _server_socket;
	std::vector<struct pollfd> _pollfds;
	std::vector<Buffer>		   _buffers;
	int						   _timeout;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
