#pragma once
#include "config_parser.hpp"
#include "main.hpp"
#include "poller.hpp"

class Router;
extern Router g_router;

class Router {
  public:
	Router();
	~Router();
	int route(Client& client);
	// read-only variables
	const Config::Server&	find_server(uint16_t port, const std::string& hostname);
	const Config::Location& find_location(const std::string& path, const Config::Server& server);

  private:
	// disabled
	// Router(const Router& cp); // TODO
	// Router& operator=(const Router& cp);
	bool _has_server_name(std::vector<Config::Server>::iterator server, std::string server_name);
};

std::ostream& operator<<(std::ostream& output, Router const& rhs);
