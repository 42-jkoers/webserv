#pragma once
#include "main.hpp"
#include "request.hpp"

class Client {
  public:
	Client(){};
	Client(uint16_t port);
	~Client() {}

	enum Parse_status {
		INCOMPLETE,
		READING_HEADER,
		READING_HEADER_DONE,

		HEADER_DONE,
		// send 100-continue
		READING_BODY_HEADER,
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
	int			 server_index;

  private:
	Parse_status _parse_status;
	size_t		 _body_size;
	std::string	 _buf;

	void		 _parse(size_t bytes_read);
	Chunk_status _append_chunk(size_t bytes_read);
};
