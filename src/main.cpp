#include "main.hpp"
#include "config_parser.hpp"
#include "poller.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>

#define PORT 8081

void on_request(Request& request, config& config) {
	std::string request_line = get_html_request(config); // getting the right html file
	std::cout << request.raw << std::endl;
	request.send_response(200, request_line);
	(void)config;
}

int main(int argc, char **argv) {
	config config;

	if (argc != 2)
		exit_with::e_perror("Not the right amount of arguments");
	config_parser(config, argv);
	Poller poller(mode_ipv6, config.get_port(), 500000);
	poller.start(on_request, config);
	return 0;
}
