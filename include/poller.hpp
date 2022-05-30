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

class Fdinfo {
  public:
	enum class Type {
		SERVER,
		CLIENT,
		RESPONSE
	};

	class Server {
	  public:
		Server(uint16_t port) : port(port) {}
		uint16_t port;
	};

	class Response {
	  public:
		fd_t fd;
	};

	Fdinfo() {}
	Fdinfo(Type t) : type(t){};
	~Fdinfo(){};
	Fdinfo& operator=(const Fdinfo& cp) {
		type = cp.type;
		if (cp.type == Type::SERVER)
			server = cp.server;
		else if (cp.type == Type::CLIENT)
			client = cp.client;
		else if (cp.type == Type::RESPONSE)
			response = cp.response;
		return *this;
	}

	Fdinfo::Type type;
	union {
		Fdinfo::Server	 server;
		Client			 client;
		Fdinfo::Response response;
	};
};

class Poller {
  public:
	Poller();
	void add_server(IP_mode ip_mode, const char* str_addr, uint16_t port);
	void start(void (*on_request)(Client& client));
	~Poller();

  private:
	void					   _accept_clients();

	void					   _on_poll(pollfd& pfd, void (*on_request)(Client& client));
	void					   _on_server(const pollfd& pfd);
	void					   _on_client(pollfd& pfd, void (*on_request)(Client& client));
	void					   _on_response(const pollfd& pfd);

	std::vector<struct pollfd> _pollfds;
	std::map<fd_t, Fdinfo>	   _fdinfo;

	// disabled
	Poller(const Poller& cp);
	Poller& operator=(const Poller& cp);
};
