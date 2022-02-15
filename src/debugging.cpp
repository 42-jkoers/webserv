#include "main.hpp"
#include <sys/poll.h>

void log_pollfd(const struct pollfd& pfd) {
	std::cout << "     fd: " << pfd.fd << "\n"
			  << "revents: " << pfd.revents << "\n"
			  << " events: ";
	if (pfd.revents & POLLIN)
		std::cout << "POLLIN ";
	if (pfd.revents & POLLHUP)
		std::cout << "POLLHUP ";
	if (pfd.revents & POLLERR)
		std::cout << "POLLERR";
	std::cout << "\n " << std::endl;
}
