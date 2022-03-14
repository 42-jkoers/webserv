#pragma once
#include "main.hpp"
#include "request.hpp"
#include "response.hpp"

class Buffer {
  public:
	Buffer();
	enum Read_status {
		UNSET,
		TEMPORALLY_UNIAVAILABLE,
		IN_PROGRESS,
		DONE
	};

	enum Body_type {
		EMPTY,
		MULTIPART,
		CHUNKED
	};

	enum Parse_status {
		INCOMPLETE = 0,
		HEADER_IN_PROGRESS = 1,

		HEADER_DONE = 2,
		// send 100-continue
		WAITING_FOR_BODY = 4,
		// while (read() > 0)
		BODY_IN_PROGRESS = 5,

		BODY_DONE = 6,
		FINISHED = 7,

		ERROR = 8
	};

	enum Chunk_status {
		CS_IN_PROGRESS,
		CS_NULL_BLOCK_REACHED,
		CS_ERROR
	};

	Read_status		  read_pollfd(const pollfd& pfd);
	Parse_status	  parse_status() const;
	void			  reset();
	void			  print() const;

	Request			  request;
	std::vector<char> body;

  private:
	Read_status		  _read_status;
	Parse_status	  _parse_status;
	Body_type		  _body_type;
	ssize_t			  _bytes_to_read;
	std::vector<char> _buf;

	bool			  _is_end_of_http_request(const std::string& s);
	void			  _parse(size_t bytes_read, const pollfd& pfd);
	Chunk_status	  _append_chunk(size_t bytes_read);
};

class Poller {
  public:
	Poller(IP_mode ip_mode, uint16_t port, int timeout);
	void start(void (*on_request)(Request& request));
	~Poller();

  private:
	struct pollfd			   _create_pollfd(int fd, short events);
	void					   _accept_clients();

	void					   _on_new_pollfd(pollfd& pfd, void (*on_request)(Request& request));
	fd_t					   _server_socket;
	std::vector<struct pollfd> _pollfds;
	std::map<fd_t, Buffer>	   _buffers;
	int						   _timeout;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
