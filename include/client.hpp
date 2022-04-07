#pragma once
#include "main.hpp"
#include "request.hpp"

class Client {
  public:
	Client();

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

	void		 read_pollfd(const pollfd& pfd);
	Parse_status parse_status() const;
	void		 reset();

	Request		 request;

  private:
	Parse_status	  _parse_status;
	Body_type		  _body_type;
	ssize_t			  _bytes_to_read;
	std::vector<char> _buf;

	bool			  _is_end_of_http_request(const std::string& s);
	void			  _parse(size_t bytes_read, const pollfd& pfd);
	Chunk_status	  _append_chunk(size_t bytes_read);
};
