#pragma once
#include "client.hpp"
#include "main.hpp"
#include "request.hpp"
#include "response.hpp"

namespace constructors {

struct sockaddr_in6 sockaddr6(uint16_t port);
struct sockaddr_in	sockaddr(const char* str_addr, uint16_t port);
fd_t				server_socket(IP_mode ip_mode, const char* str_addr, uint16_t port);
struct pollfd		pollfd(int fd, short events);

} // namespace constructors

enum class Type {
	SERVER,
	CLIENT,
	RESPONSE_READ,
	RESPONSE_WRITE,
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

	class Response {
	  public:
		Response() {}
		Response(fd_t fd) : fd(fd) {}
		fd_t fd;
	};

	Poller();
	void add_server(IP_mode ip_mode, const char* str_addr, uint16_t port);
	void start();
	~Poller();

  private:
	void accept_clients(const Server& server);

	void on_poll(pollfd& pfd);

	void add_fd(pollfd pfd, const Server& server) {
		_pollfds.push_back(pfd);
		_fd_types[pfd.fd] = Type::SERVER;
		_servers[pfd.fd] = server;
	}
	void add_fd(pollfd pfd, const Client& client) {
		_pollfds.push_back(pfd);
		_fd_types[pfd.fd] = Type::CLIENT;
		_clients[pfd.fd] = client;
	}
	void add_fd(pollfd pfd, const Response& response) {
		_pollfds.push_back(pfd);
		_fd_types[pfd.fd] = Type::RESPONSE_READ;
		_responses[pfd.fd] = response;
	}

	std::vector<struct pollfd> _pollfds;

	std::map<fd_t, Type>	   _fd_types;
	std::map<fd_t, Server>	   _servers;
	std::map<fd_t, Client>	   _clients;
	std::map<fd_t, Response>   _responses;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
