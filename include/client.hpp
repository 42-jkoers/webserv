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
		INCOMPLETE,
		READING_HEADER,
		READING_HEADER_DONE,

		HEADER_DONE,
		// send 100-continue
		READING_BODY,
		FINISHED,

		ERROR
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

	void			  _parse(size_t bytes_read, const pollfd& pfd);
	Chunk_status	  _append_chunk(size_t bytes_read);
};
