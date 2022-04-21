#pragma once
#include "config_parser.hpp"
#include "main.hpp"
#include "poller.hpp"

class Router {
  public:
	Router();
	~Router();
	int route(Client& client);
	// read-only variables

  private:
	// disabled
	// Router(const Router& cp); // TODO
	// Router& operator=(const Router& cp);
	bool _has_server_name(std::vector<Config::Server>::iterator server, std::string server_name);
	void _link_client_server(Request& request);
};

std::ostream& operator<<(std::ostream& output, Router const& rhs);
