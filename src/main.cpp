#include "main.hpp"
#include "config_parser.hpp"
#include "constants.hpp"
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

Config			g_config;
const Constants g_constants;
//

void on_request(Client& client) {
	// client.print();
	// std::cout << client.request << std::endl;

	if (client.request.uri_raw.find("/cgi/input") != std::string::npos)
		Response::cgi(client.request, "./cgi/input", "", "");
	else if (client.request.uri_raw.find("/cgi/index.sh") != std::string::npos)
		Response::cgi(client.request, "./cgi/index.sh", "", "");
	else if (client.request.field_value("user-agent").find("curl") != std::string::npos)
		Response::text(client.request, 200, "Hello curl\n");
	else
		Response::file(client.request, "./html/upload.html");
}

int main(int argc, char** argv) {
	g_config = Config(argc, argv);
	Poller poller;

	for (std::vector<Config::Server>::iterator server = g_config._server.begin(); server != g_config._server.end(); ++server) {
		for (std::vector<uint32_t>::iterator port = server->_port.begin(); port != server->_port.end(); ++port) {
			poller.add_server(mode_ipv6, *port);
		}
	}
	poller.start(on_request);
	return 0;
}
