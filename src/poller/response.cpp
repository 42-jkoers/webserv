#include "response.hpp"

Response::Response(fd_t fd, uint32_t ret) : _fd(fd), _response_code(ret) {
}

Response::~Response() {
}

// TODO: optimize
void Response::send_response(const std::string& message) {
	std::map<uint32_t, std::string> m; // TODO: make this static
	m[100] = "Continue";
	m[101] = "Switching Protocols";
	m[200] = "OK";
	m[201] = "Created";
	m[202] = "Accepted";
	m[203] = "Non-Authoritative Information";
	m[204] = "No Content";
	m[205] = "Reset Content";
	m[206] = "Partial Content";
	m[300] = "Multiple Choices";
	m[301] = "Moved Permanently";
	m[302] = "Found";
	m[303] = "See Other";
	m[304] = "Not Modified";
	m[305] = "Use Proxy";
	m[307] = "Temporary Redirect";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[402] = "Payment Required";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[406] = "Not Acceptable";
	m[407] = "Proxy Authentication Required";
	m[408] = "Request Timeout";
	m[409] = "Conflict";
	m[410] = "Gone";
	m[411] = "Length Required";
	m[412] = "Precondition Failed";
	m[413] = "Payload Too Large";
	m[414] = "URI Too Long";
	m[415] = "Unsupported Media Type";
	m[416] = "Range Not Satisfiable";
	m[417] = "Expectation Failed";
	m[426] = "Upgrade Required";
	m[500] = "Internal Server Error";
	m[501] = "Not Implemented";
	m[502] = "Bad Gateway";
	m[503] = "Service Unavailable";
	m[504] = "Gateway Timeout";
	m[505] = "HTTP Version Not Supported";

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
