#include "response.hpp"

Response::Response(fd_t fd, int ret) : _fd(fd), _response_code(ret) {
}

Response::~Response() {
}

// TODO: optimize
void Response::send_response(const std::string& message) {
	std::map<uint32_t, std::string> m; // TODO: make this static
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
	response += std_ft::to_string(_response_code);
	response += " ";
	response += m[_response_code];
	response += "\r\n\r\n";
	response += message;
	write(_fd, response.c_str(), response.length()); // TODO: error handling
}

std::string Response::get_index(Config& config) {
	std::ifstream html_file;
	std::string	  line;
	std::string	  html;
	html_file.open(config.get_root().c_str());
	while (getline(html_file, line)) {
		html += line;
	}
	html_file.close();
	return html;
}
