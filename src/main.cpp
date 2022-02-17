#include "main.hpp"
#include "request.hpp"
#include "config_parser.hpp"
#include "poller.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>

#define PORT 8080

void on_request(Request& request, Config& config) {
	std::cout << request.raw << std::endl;
	std::string html_response = get_index(config); // getting the right html file
	request.send_response(200, html_response);
	(void)config;
}

int main(int argc, char** argv) {
	Config config(argc, argv);

	Poller poller(mode_ipv6, config.get_port(), 50000);
	poller.start(on_request, config);
	return 0;
}
