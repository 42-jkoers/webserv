#include "main.hpp"
#include <map>
#include <netinet/in.h>

// TODO: optimize
void response(fd_t fd, uint32_t response_code, const std::string& message) {
	std::map<uint32_t, std::string> m;
	m[200] = "OK";
	m[201] = "Created";
	m[202] = "Accepted";
	m[204] = "No Content";
	m[301] = "Moved Permanently";
	m[302] = "Moved Temporarily";
	m[304] = "Not Modified";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[500] = "Internal Server Error";
	m[501] = "Not Implemented";
	m[502] = "Bad Gateway";
	m[503] = "Service Unavailable";

	std::string response = "HTTP/1.1 ";
	response += cpp11::to_string(response_code);
	response += " ";
	response += m[response_code];
	response += "\r\n\r\n";
	response += message;
	write(fd, response.c_str(), response.length());
}
