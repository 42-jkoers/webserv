#include "main.hpp"
#include "config_parser.hpp"
#include "file_system.hpp"
#include "poller.hpp"
#include "request.hpp"

#define PORT 8080

void on_request(Request& request) {
	std::cout << request.raw << std::endl;
	request.send_response(200, "Hello World!");
}

int main(int argc, char** argv) {
	Config config(argc, argv);

	Poller poller(mode_ipv6, config.get_port(), 50000);
	poller.start(on_request, config);
	return 0;
}
