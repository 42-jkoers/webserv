#include "main.hpp"
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8081

int main() {
	fd_t			   server_fd = create_socket();
	struct sockaddr_in address;

	listen_on_socket(server_fd, PORT, address);
	while (true) {
		fd_t		request_fd = accept_from_fd(server_fd, address);
		std::string request_headers = read_request(request_fd);

		write_response(request_fd, "HTTP/1.0 200 OK\r\n\r\nHello World!");
		close(request_fd);

		std::cout << "request headers: " << std::endl;
		std::cout << request_headers << std::endl;
	}
	return 0;
}
