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
	void					route(Client& client);
	const Config::Server&	find_server(const uint16_t port, const std::string& hostname);
	const Config::Location& find_location(const std::string& path, const Config::Server& server);

  private:
	// disabled
	// Router(const Router& cp); // TODO
	// Router& operator=(const Router& cp);
	void		_respond_with_error_code(const Request& request, const std::string& path, uint16_t error_code);
	std::string _find_index(const Config::Location& location, std::string& path);
	void		_dir_list(Request& request, const std::string& path);
	void		_route_cgi(Request& request, std::string& path);
	void		_route_get(Request& request, std::string& path);
	void		_respond_redirect(const Request& request);
};
