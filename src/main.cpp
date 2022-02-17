#include "main.hpp"
#include "file_system.hpp"
#include "poller.hpp"

#define PORT 8081

void on_request(Request& request) {
	std::cout << request.raw << std::endl;
	request.send_response(200, "Hello World!");
}

int main() {
	Poller poller(mode_ipv6, PORT, 500000);

	poller.start(on_request);
	return 0;
}
