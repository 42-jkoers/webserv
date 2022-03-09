#include "main.hpp"
#include <sys/poll.h>

void log_event(short event) {
	if (event & POLLIN)
		std::cout << "POLLIN ";
	if (event & POLLPRI)
		std::cout << "POLLPRI ";
	if (event & POLLOUT)
		std::cout << "POLLOUT ";
	if (event & POLLRDNORM)
		std::cout << "POLLRDNORM ";
	// if (event & POLLWRNORM) // same as POLLOUT
	// 	std::cout << "POLLWRNORM ";
	if (event & POLLRDBAND)
		std::cout << "POLLRDBAND ";
	if (event & POLLWRBAND)
		std::cout << "POLLWRBAND ";
	if (event & POLLERR)
		std::cout << "POLLERR ";
	if (event & POLLHUP)
		std::cout << "POLLHUP ";
	if (event & POLLNVAL)
		std::cout << "POLLNVAL ";
}

void log_pollfd(const struct pollfd& pfd) {
	std::cout << "     fd: " << pfd.fd << "\n"
			  << " events: ";
	log_event(pfd.events);
	std::cout << "\nrevents: ";
	log_event(pfd.revents);
	std::cout << "\n " << std::endl;
}
