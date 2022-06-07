#pragma once
#include "config_parser.hpp"
#include "main.hpp"
#include "poller.hpp"

class Route {
  public:
	Route(const std::string& header)
		: header(header),
		  file_fd(-1) {}

	Route(const std::string& header,
		  fd_t				 file_fd)
		: header(header),
		  file_fd(file_fd) {}
	Route() {}

	std::string header;
	fd_t		file_fd;
	// bool		close_connection;
};

class Router;
extern Router g_router;

class Router {
  public:
	Router();
	~Router();
	Route					route(Client& client) __attribute__((warn_unused_result));
	const Config::Server&	find_server(const uint16_t port, const std::string& hostname);
	const Config::Location& find_location(const std::string& path, const Config::Server& server);

  private:
	// disabled
	// Router(const Router& cp); // TODO
	// Router& operator=(const Router& cp);
	Route		_respond_with_error_code(const Request& request, const std::string& path, uint16_t error_code);
	std::string _find_index(const Config::Location& location, std::string& path);
	Route		_dir_list(Request& request, const std::string& path);
	Route		_route_cgi(Request& request, std::string& path);
	Route		_route_get(Request& request, std::string& path);
	Route		_route_post(Request& request);
	Route		_route_delete(Request& request);
	Route		_respond_redirect(const Request& request);
};
