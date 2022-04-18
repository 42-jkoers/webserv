#pragma once
#include "client.hpp"
#include "main.hpp"
#include "request.hpp"
#include "response.hpp"

namespace constructors {

struct sockaddr_in6 sockaddr6(uint16_t port);
struct sockaddr_in	sockaddr(uint16_t port);
fd_t				server_socket(IP_mode ip_mode, uint16_t port);
struct pollfd		pollfd(int fd, short events);

} // namespace constructors

class Poller {
  public:
	Poller();
	void add_server(IP_mode ip_mode, uint16_t port);
	void start(void (*on_request)(Client& client));
	~Poller();

  private:
	void					   _accept_clients();

	void					   _on_new_pollfd(pollfd& pfd, void (*on_request)(Client& client));
	size_t					   _n_servers;
	std::vector<struct pollfd> _pollfds;
	std::vector<uint16_t>	   _server_ports;
	std::map<fd_t, Client>	   _clients;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
