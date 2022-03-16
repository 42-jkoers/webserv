#include "main.hpp"
#include "config_parser.hpp"
#include "file_system.hpp"
#include "poller.hpp"
#include "request.hpp"
#include "response.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>

#define PORT 8080

void on_request(Client& client) {
	client.print();

	Response response(client.request.get_fd(), 200);
	response.send_response("Hello World!\n");
}

int main(int argc, char** argv) {
	Config config(argc, argv);

	Poller poller(mode_ipv6, config.get_port(), -1);
	poller.start(on_request);
	return 0;
}
