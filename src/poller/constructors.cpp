#include "main.hpp"
#include "poller.hpp"
#include "response.hpp"
#include <fcntl.h>
#include <limits>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>

namespace constructors {

struct sockaddr_in6 sockaddr6(uint16_t port) {
	struct sockaddr_in6 address;
	memset(&address, 0, sizeof(address));
	address.sin6_family = AF_INET6;
	memcpy(&address.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	address.sin6_port = htons(port);
	return address;
}

struct sockaddr_in sockaddr(uint16_t port) {
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	return address;
}

// returns fd to socket
fd_t server_socket(IP_mode ip_mode, uint16_t port) {
	fd_t fd = socket(ip_mode == mode_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		exit_with::e_perror("Cannot create socket");
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
		exit_with::e_perror("setsockopt() failed");
	if (ioctl(fd, FIONBIO, (char*)&on) < 0)
		exit_with::e_perror("ioctl() failed");

	int rc;
	if (ip_mode == mode_ipv6) {
		struct sockaddr_in6 address = sockaddr6(port);
		rc = bind(fd, (struct sockaddr*)&address, sizeof(address));
	} else {
		struct sockaddr_in address = sockaddr(port);
		rc = bind(fd, (struct sockaddr*)&address, sizeof(address));
	}
	if (rc < 0)
		exit_with::e_perror("Cannot bind to port");

	if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) == -1)
		exit_with::e_perror("Cannot set non blocking");
	if (listen(fd, 128) < 0) // TODO: what should this number be? 128 is maximum
		exit_with::e_perror("Cannot listen on port");

	return fd;
}

struct pollfd pollfd(int fd, short events) {
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	return pfd;
}

} // namespace constructors
