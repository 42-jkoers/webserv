#pragma once
#include "client.hpp"
#include "main.hpp"
#include "request.hpp"
#include "response.hpp"
#include "route.hpp"

namespace constructors {

struct sockaddr_in6 sockaddr6(uint16_t port);
struct sockaddr_in	sockaddr(const char* str_addr, uint16_t port);
fd_t				server_socket(IP_mode ip_mode, const char* str_addr, uint16_t port);
struct pollfd		pollfd(int fd, short events);

} // namespace constructors

enum class Fd_type {
	SERVER,
	CLIENT,
	ROUTE,
	CLOSED
};

class Poller {
  public:
	class Server {
	  public:
		Server() {}
		Server(fd_t fd, uint16_t port) : fd(fd), port(port) {}

		fd_t	 fd;
		uint16_t port;
	};

	Poller();
	void add_server(IP_mode ip_mode, const char* str_addr, uint16_t port);
	void start();
	~Poller();

  private:
	void accept_clients(const Server& server);

	void on_poll(pollfd pfd);
	void on_poll(pollfd pfd, Client& client);

	void add_fd(pollfd pfd, const Server& server) {
		_pollfds.push_back(pfd);
		_fd_types[pfd.fd] = Fd_type::SERVER;
		_servers[pfd.fd] = server;
	}
	void add_fd(pollfd pfd, const Client& client) {
		_pollfds.push_back(pfd);
		_fd_types[pfd.fd] = Fd_type::CLIENT;
		_clients[pfd.fd] = client;
	}
	void add_fd(pollfd pfd, const Route& route) {
		_pollfds.push_back(pfd);
		_fd_types[pfd.fd] = Fd_type::ROUTE;
		_routes[pfd.fd] = route;
	}

	void close_fd(pollfd pfd) {
		close(pfd.fd);
		_clients.erase(pfd.fd);
		_servers.erase(pfd.fd);
		_routes.erase(pfd.fd);
		_fd_types[pfd.fd] = Fd_type::CLOSED;
	}

	std::vector<struct pollfd> _pollfds;

	std::map<fd_t, Fd_type>	   _fd_types;
	std::map<fd_t, Server>	   _servers;
	std::map<fd_t, Client>	   _clients;
	std::map<fd_t, Route>	   _routes;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
