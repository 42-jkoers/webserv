#include "router.hpp"

Router::~Router() {
}

Router::Router() {
	return;
}

bool Router::_has_server_name(std::vector<Config::Server>::iterator server, std::string server_name) {
	for (std::vector<std::string>::iterator it = server->server_name.begin(); it != server->server_name.end(); ++it) {
		if (*it == server_name) {
			return 1;
		}
	}
	return 0;
}

/*
first server with correct port is the default server
*/
void Router::_link_client_server(Request& request) {
	int found = 0;
	int i = 0;

	// std::cout << "connected port is " << request.port << std::endl;
	for (std::vector<Config::Server>::iterator it = g_config._servers.begin(); it != g_config._servers.end(); ++it) { // loop over servers
		for (std::vector<uint32_t>::iterator it2 = it->port.begin(); it2 != it->port.end(); ++it2) {				  // loop over ports
			// std::cout << "checking port[" << *it2 << "]" << std::endl;
			if (*it2 == request.port) {
				// std::cout << *it2 << " found at server " << i << std::endl;
				if (_has_server_name(it, request.field_value("host", 0))) {
					found = 1;
					request.server_index = i;
				} else if (found == 0) {
					found = 1;
					request.server_index = i;
					if (_has_server_name(it, request.field_value("host", 0)))
						return;
				}
			}
		}
		i++;
	}
	// std::cout << "correct server is found to be " << request.server_index << std::endl;
}

// TODO: check the locations
// allowed methods -> method not allowed

/*
Routes request to the right server
connected port -> server names
*/
int Router::route(Client& client) {
	Request& request = client.request;

	if (client.request.response_code != 200) {
		Response::text(request, request.response_code, "");
		return 1;
	}
	_link_client_server(request);
	if (request.path.compare("favicon.ico") == 0)
		Response::text(request, 404, "");
	else if (request.uri.find("/cgi/input") != std::string::npos)
		Response::cgi(request, "./cgi/input", "", request.query);
	else if (request.uri.find("/cgi/index.sh") != std::string::npos)
		Response::cgi(request, "./cgi/index.sh", "", request.query);
	else if (request.uri.find("/form") != std::string::npos)
		Response::file(request, "./html/form.html");
	else if (request.field_contains("user-agent", "curl"))
		Response::text(request, 200, "Hello curl\n");
	else
		Response::file(request, "./html/upload.html");
	return 0;
}
