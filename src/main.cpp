#include "main.hpp"
#include "config_parser.hpp"
#include "constants.hpp"
#include "file_system.hpp"
#include "poller.hpp"
#include "request.hpp"
#include "response.hpp"
#include "router.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>

Config			g_config;
const Constants g_constants;
//

void on_request(Client& client) {
	Router		   router;
	const Request& req = client.request;

	std::cout << req << std::endl;
	router.route(client);
}

int main(int argc, char** argv) {
	g_config = Config(argc, argv);
	Poller poller;

	for (std::vector<Config::Server>::iterator server = g_config._servers.begin(); server != g_config._servers.end(); ++server) {
		for (std::vector<uint32_t>::iterator port = server->port.begin(); port != server->port.end(); ++port) {
			poller.add_server(mode_ipv4, "127.0.0.1", *port); // TODO: do not put localhost, if listen directive provides different ip address in config file
		}
	}
	std::cout << "started" << std::endl;
	poller.start(on_request);
	return 0;
}
