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
const Config::Server& Router::find_server(uint16_t port, const std::string& hostname) {
	int	   found = 0;
	size_t i = 0;
	size_t server_index;

	// TODO: cpp11 iterators
	for (std::vector<Config::Server>::iterator it = g_config._servers.begin(); it != g_config._servers.end(); ++it) { // loop over servers
		for (std::vector<uint16_t>::iterator it2 = it->port.begin(); it2 != it->port.end(); ++it2) {				  // loop over ports
			if (*it2 == port) {
				if (_has_server_name(it, hostname)) {
					return *it;
				} else if (found == 0) {
					server_index = i;
				}
				found = 1;
			}
		}
		i++;
	}
	return g_config._servers[server_index];
}

// TODO: check the locations -> 404 not found
// server class has a vector of locations
// first most specific prefix location
// /html is more specific than /
// location is prefix of URI; if URI starts with location -> this location is selected
const Config::Location& Router::find_location(const std::string& path, const Config::Server& server) {
	int			i = 0;
	int			location_index = 0;
	std::string last_path = "";

	location_index = 0;
	for (std::vector<Config::Location>::const_iterator it = server.location.begin(); it != server.location.end(); it++) {
		size_t found = path.find(it->_path);
		if (found != std::string::npos && found == 0) {
			if (last_path.size() < it->_path.size()) {
				location_index = i;
				last_path = it->_path;
			}
		}
		i++;
	}
	return server.location[location_index];
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
