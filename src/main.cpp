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

	if (client.request.get_request_line()["URI"].find("cgi") != std::string::npos)
		Response::cgi(client.request, "./html/index.sh", "", "hello form bash");
	else if (client.request.has_value("user-agent", "curl"))
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
