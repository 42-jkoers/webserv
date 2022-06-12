#pragma once

#include "main.hpp"

class Route {
  public:
	enum class Status {
		IN_PROGRESS,
		DONE,
		ERROR
	};

	Route(const std::string& header,
		  fd_t				 file_fd,
		  size_t			 file_size)
		: header(header),
		  file_fd(file_fd),
		  file_size(file_size),
		  file_offset(0) {}

	Route(const std::string& header) : Route(header, -1, 0) {}
	Route() {}

	Status send(fd_t client_fd) {
		if (header.size()) {
			if (write(client_fd, header.data(), header.size()) != (ssize_t)header.size())
				return end(Status::ERROR);
			header.clear();
		}

		off_t sent_len;
		sendfile(file_fd, client_fd, file_offset, &sent_len, NULL, 0);
		// std::cout << "send: " << sent_len << " offset: " << file_offset << " size: " << file_size << std::endl;
		file_offset += sent_len;
		if (file_offset >= file_size)
			return end(Status::DONE);
		else
			return end(Status::IN_PROGRESS);
	}

  private:
	std::string header;
	fd_t		file_fd;
	off_t		file_size;
	off_t		file_offset;

	//
	Status end(Status status) {
		if (file_fd >= 0 && status != Status::IN_PROGRESS)
			close(file_fd);
		return status;
	}
};
