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

void Router::_link_client_server(Client& client) {
	int found = 0;
	int i = 0;
	// client.server_index = -1;
	(void)client;
	(void)found;
	(void)i;

	// fix server index
	// for (std::vector<Config::Server>::iterator it = g_config._server.begin(); it != g_config._server.end(); ++it) {
	// 	for (std::vector<uint32_t>::iterator it2 = it->_port.begin(); it2 != it->_port.end(); ++it2) {
	// 		if (*it2 == client.port) {
	// 			if (found == 0) { // first server with correct port is the default server
	// 				found = 1;
	// 				client.server_index = i;
	// 				if (_has_server_name(it, client.request.get_value("host")))
	// 					return;
	// 			} else if (_has_server_name(it, client.request.get_value("host")))
	// 				client.server_index = i;
	// 			return;
	// 		}
	// 	}
	// 	i++;
	// }
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
