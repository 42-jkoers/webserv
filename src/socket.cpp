#include "main.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>

std::string get_client_address(struct sockaddr_in& address) {
	static char buf[BUFFER_SIZE + 1];
	std::string str;

	if (inet_ntop(AF_INET, &address, buf, BUFFER_SIZE) == NULL)
		exit_with::e_perror("Cannot get address");
	buf[BUFFER_SIZE] = '\0';
	str = buf;
	return str;
}
