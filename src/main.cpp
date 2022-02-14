#include "main.hpp"
#include "poller.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>

#define PORT 8081

void on_request(Request& request) {
	std::cout << request.raw << std::endl;
	request.send_response(200, "Hello World!");
}

int main() {
	fd_t   listen_fd = create_server_socket(mode_ipv6, PORT);
	Poller poller(listen_fd, 500000);

	poller.start(on_request);
	return 0;
}
