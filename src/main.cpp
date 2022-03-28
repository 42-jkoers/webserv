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
	Response response(client.request.fd);
	std::cout << client.request.get_request_line()["URI"] << std::endl;

	if (client.request.get_request_line()["URI"].find(".py") != std::string::npos)
		response.send_cgi("./html/index.sh", "", "");
	else if (client.request.get_value("user-agent").find("curl") != std::string::npos)
		response.send_response(200, "Hello curl\n");
	else
		response.send_response(200, fs::read_file("./html/upload.html"));
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
