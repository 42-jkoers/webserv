#include "main.hpp"
#include <sys/poll.h>

void log_pollfd(const struct pollfd& pfd) {
	std::cout << "     fd: " << pfd.fd << std::endl
			  << " events: ";
	if (pfd.events & POLLIN)
		std::cout << "POLLIN";
	if (pfd.events & POLLHUP)
		std::cout << "POLLHUP";
	if (pfd.events & POLLERR)
		std::cout << "POLLERR";
	std::cout << std::endl
			  << "revents: ";
	if (pfd.revents & POLLIN)
		std::cout << "POLLIN";
	if (pfd.revents & POLLHUP)
		std::cout << "POLLHUP";
	if (pfd.revents & POLLERR)
		std::cout << "POLLERR";
	std::cout << std::endl
			  << std::endl;
}
