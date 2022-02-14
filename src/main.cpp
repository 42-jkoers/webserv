#include "main.hpp"
#include "config_parser.hpp"
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
	fd_t			   server_fd = create_socket();
	struct sockaddr_in address;

	config			   config;
	config_parser(config);
	listen_on_socket(server_fd, PORT, address);
	std::cout << "Listening on: http://localhost:" << PORT << std::endl;
	while (true) {
		fd_t		request_fd = accept_from_fd(server_fd, address);
		std::string request_headers = read_request(request_fd);
		std::string client_address = get_client_address(address);

		std::cout << "Client address:" << std::endl;
		response(request_fd, 200, "Hello World!");

		close(request_fd);

		std::cout << "request headers: " << std::endl;
		std::cout << request_headers << std::endl;
	}
	return 0;
}
