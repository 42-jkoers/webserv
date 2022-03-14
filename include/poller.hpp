#pragma once
#include "main.hpp"
#include "request.hpp"
#include "response.hpp"

class Read_buffer {
  public:
	Read_buffer();
	ssize_t		append(fd_t fd);
	void		free_n(size_t n);
	void		reset();
	void		copy_to_vector(std::vector<char>& v, size_t n);
	void		copy_to_string(std::string& s);
	void		copy_to_string(std::string& s, size_t n);

	void		print(size_t max = 999999999);

	size_t		size() const { return _size; };
	const char* data() const { return _data; };

  private:
	size_t _size;
	char   _data[BUFFER_SIZE * 2 + 1];

	// disabled
	// Read_buffer(const Read_buffer& cp);
	// Read_buffer& operator=(const Read_buffer& cp);
};

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
	Read_status	 _read_status;
	Parse_status _parse_status;
	Body_type	 _body_type;
	ssize_t		 _bytes_to_read;
	Read_buffer	 _read_buffer;

	bool		 _is_end_of_http_request(const std::string& s);
	void		 _parse(size_t bytes_read, const pollfd& pfd);
	Chunk_status _append_chunk(size_t bytes_read);

	// disabled
	// Buffer(const Buffer& cp);
	// Buffer& operator=(const Buffer& cp);
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
	std::vector<Buffer>		   _buffers;
	int						   _timeout;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
