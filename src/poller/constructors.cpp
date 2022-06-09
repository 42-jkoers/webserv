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
	address.sin6_port = htons(port);
	memcpy(&address.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	return address;
}

struct sockaddr_in sockaddr(const char* str_addr, uint16_t port) {
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	in_addr_t addr = inet_addr(str_addr);
	if (addr == (in_addr_t)(-1))
		exit_with::perror("Error creating socket address");
	address.sin_addr.s_addr = addr;
	return address;
}

// returns fd to socket
fd_t server_socket(IP_mode ip_mode, const char* str_addr, uint16_t port) {
	fd_t fd = socket(ip_mode == mode_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		exit_with::perror("Cannot create socket");
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
		exit_with::perror("setsockopt() failed");
	if (ioctl(fd, FIONBIO, (char*)&on) < 0)
		exit_with::perror("ioctl() failed");

	int rc;
	if (ip_mode == mode_ipv6) {
		struct sockaddr_in6 address = sockaddr6(port);
		rc = bind(fd, (struct sockaddr*)&address, sizeof(address));
	} else {
		struct sockaddr_in address = sockaddr(str_addr, port);
		rc = bind(fd, (struct sockaddr*)&address, sizeof(address));
	}
	if (rc < 0)
		exit_with::perror("Cannot bind to port");

	// if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) == -1) // hopefully this is equivalent to the line below
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		exit_with::perror("Cannot set non blocking");
	if (listen(fd, 128) < 0) // TODO: what should this number be? 128 is maximum
		exit_with::perror("Cannot listen on port");

	return fd;
}

struct pollfd pollfd(int fd, short events) {
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	return pfd;
}

} // namespace constructors
