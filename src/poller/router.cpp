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
void Router::_link_client_server(Client& client) {
	int found = 0;
	int i = 0;
	// client.server_index = -1;
	(void)client;
	(void)found;
	(void)i;

	std::cout << "connected port is " << client.request.port << std::endl;
	for (std::vector<Config::Server>::iterator it = g_config._servers.begin(); it != g_config._servers.end(); ++it) { // loop over servers
		for (std::vector<uint32_t>::iterator it2 = it->port.begin(); it2 != it->port.end(); ++it2) {				  // loop over ports
			if (*it2 == client.request.port) {
				std::cout << *it2 << " found at server " << i << std::endl;
				if (_has_server_name(it, client.request.field_value("host", 0))) {
					found = 1;
					client.server_index = i;
				} else if (found == 0) {
					found = 1;
					client.server_index = i;
					if (_has_server_name(it, client.request.field_value("host", 0)))
						return;
				}
			}
		}
		i++;
	}
}

/*
Routes request to the right server
connected port -> server names
*/
int Router::route(Client& client) {
	_link_client_server(client);
	// std::cout << "server_index:" << client.server_index << std::endl;
	return 0;
}
