#pragma once
#include "main.hpp"
#include "request.hpp"

class Client {
  public:
	Client(){};
	Client(const std::string& ip, uint16_t port);
	~Client() {}

	enum Parse_status {
		INCOMPLETE,			 // Invalid state
							 //
		READING_HEADER,		 // \r\n\r\n not yet reached
		READING_HEADER_DONE, // \r\n\r\n reached
		HEADER_DONE,		 // header successfully parsed
							 //
		READING_BODY_HEADER, // \r\n\r\n not yet reached
		READING_BODY,		 // \r\n\r\n reached
		FINISHED,			 // responded to request
							 //
		ERROR				 // something went wrong in any of the above steps
	};

	enum Chunk_status {
		CS_IN_PROGRESS,
		CS_NULL_BLOCK_REACHED,
		CS_ERROR
	};

	void		 on_pollevent(struct pollfd pfd);
	void		 on_pollevent_read(struct pollfd pfd);
	Parse_status parse_status() const;
	void		 reset();

	Request		 request;

  private:
	Parse_status _parse_status;
	size_t		 _body_size;
	std::string	 _buf_read;
	std::string	 _buf_write;

	void		 _parse();
	Chunk_status _append_chunk();
};
