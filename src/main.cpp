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
	const Request& req = client.request;
	if (req.uri.find("favicon.ico") != std::string::npos) {
		Response::text(req, 404, "");
		return;
	}
	std::cout << req << std::endl;

	if (req.method == "POST" && req.uri.find("/upload") != std::string::npos) {
		fs::write_file("./file_uploads/" + req.field_filename(), req.body); // TODO: read filename from Request class
		Response::text(req, 200, "File upload successful");
	} else if (req.uri.find("/cgi/input") != std::string::npos)
		Response::cgi(req, "./cgi/input", "", req.query);
	else if (req.uri.find("/cgi/index.sh") != std::string::npos)
		Response::cgi(req, "./cgi/index.sh", "", req.query);
	else if (req.uri.find("/form") != std::string::npos)
		Response::file(req, "./html/form.html");
	else if (req.field_contains("user-agent", "curl"))
		Response::text(req, 200, "Hello curl\n");
	else
		Response::file(req, "./html/upload.html");
}

int main(int argc, char** argv) {
	g_config = Config(argc, argv);
	Poller poller;

	for (std::vector<Config::Server>::iterator server = g_config._servers.begin(); server != g_config._servers.end(); ++server) {
		for (std::vector<uint32_t>::iterator port = server->port.begin(); port != server->port.end(); ++port) {
			poller.add_server(mode_ipv6, *port);
		}
	}
	std::cout << "started" << std::endl;
	poller.start(on_request);
	return 0;
}
