#include "main.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>

#define PORT 8081

int main() {
	fd_t   listen_fd = create_server_socket(mode_ipv6, PORT);
	Poller poller(listen_fd, 500000);

	poller.start();
	return 0;
}
