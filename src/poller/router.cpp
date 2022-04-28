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

	// TODO: cpp11 iterators
	for (std::vector<Config::Server>::iterator it = g_config._servers.begin(); it != g_config._servers.end(); ++it) { // loop over servers
		for (std::vector<uint16_t>::iterator it2 = it->port.begin(); it2 != it->port.end(); ++it2) {				  // loop over ports
			if (*it2 == request.port) {
				if (_has_server_name(it, request.field_value("host"))) {
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
}

// TODO: check the locations -> 404 not found
// server class has a vector of locations
// first most specific prefix location
// /html is more specific than /
// location is prefix of URI; if URI starts with location -> this location is selected
void Router::_check_location(Request& request) {
	int				i = 0;
	int				location_index = 0;
	std::string		last_path = "";
	Config::Server& server = g_config._servers[request.server_index];

	location_index = 0;
	for (std::vector<Config::Location>::iterator it = server.location.begin(); it != server.location.end(); it++) {
		size_t found = request.path.find(it->_path);
		if (found != std::string::npos && found == 0) {
			if (last_path.size() < it->_path.size()) {
				request.location_index = i;
				last_path = it->_path;
			}
		}
		i++;
	}
}

// TODO: Check allowed methods -> method not allowed

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
	_check_location(request);
	std::cout << request.server_index << "|" << request.location_index << std::endl;
	if (request.path.compare("favicon.ico") == 0) // our server does not provide a favicon
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
