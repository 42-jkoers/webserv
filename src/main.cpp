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

// usage: ./webserv [-t] [config_file.conf]
// -t: test configuration and exit
// if no config_file is specified, ./testing/default.conf is used
//
// steps:
// parse config file
// create server sockets and add to poll
// start poll
int main(int argc, char** argv) {
	if (argc == 1 || (argc == 2 && !strcmp(argv[1], "-t"))) {
		std::cout << "No conf file provided, using ./testing/default.conf" << std::endl;
		g_config = Config("testing/default.conf");
	} else if (argc == 2 && strcmp(argv[1], "-t"))
		g_config = Config(argv[1]);
	else if (argc == 3 && !strcmp(argv[1], "-t"))
		g_config = Config(argv[2]);
	else
		exit_with::message("Usage: ./webserv [-t] [config_file.conf]");

	if (argc > 1 && !strcmp(argv[1], "-t"))
		return 0;

	Poller poller;

	for (std::vector<Config::Server>::iterator server = g_config.servers.begin(); server != g_config.servers.end(); ++server) {
		for (size_t i = 0; i < server->ports.size(); ++i) {
			poller.add_server(mode_ipv4, server->ips[i].c_str(), server->ports[i]);
		}
	}

	if (argc == 1 || (argc > 1 && strcmp(argv[1], "-t"))) {
		poller.start();
	}
	return 0;
}
