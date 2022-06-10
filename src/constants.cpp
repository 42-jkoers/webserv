#include "constants.hpp"
#include "main.hpp"

Constants::Constants() {
	// clang-format off
	_response_str[100] = "Continue";
	_response_str[101] = "Switching Protocols";
	_response_str[200] = "OK";
	_response_str[201] = "Created";
	_response_str[202] = "Accepted";
	_response_str[203] = "Non-Authoritative Information";
	_response_str[204] = "No Content";
	_response_str[205] = "Reset Content";
	_response_str[206] = "Partial Content";
	_response_str[300] = "Multiple Choices";
	_response_str[301] = "Moved Permanently";
	_response_str[302] = "Found";
	_response_str[303] = "See Other";
	_response_str[304] = "Not Modified";
	_response_str[305] = "Use Proxy";
	_response_str[307] = "Temporary Redirect";
	_response_str[400] = "Bad Request";
	_response_str[401] = "Unauthorized";
	_response_str[402] = "Payment Required";
	_response_str[403] = "Forbidden";
	_response_str[404] = "Not Found";
	_response_str[405] = "Method Not Allowed";
	_response_str[406] = "Not Acceptable";
	_response_str[407] = "Proxy Authentication Required";
	_response_str[408] = "Request Timeout";
	_response_str[409] = "Conflict";
	_response_str[410] = "Gone";
	_response_str[411] = "Length Required";
	_response_str[412] = "Precondition Failed";
	_response_str[413] = "Payload Too Large";
	_response_str[414] = "URI Too Long";
	_response_str[415] = "Unsupported Media Type";
	_response_str[416] = "Range Not Satisfiable";
	_response_str[417] = "Expectation Failed";
	_response_str[418] = "I'm a teapot";
	_response_str[426] = "Upgrade Required";
	_response_str[500] = "Internal Server Error";
	_response_str[501] = "Not Implemented";
	_response_str[502] = "Bad Gateway";
	_response_str[503] = "Service Unavailable";
	_response_str[504] = "Gateway Timeout";
	_response_str[505] = "HTTP Version Not Supported";

	_methods.push_back("GET");
	_methods.push_back("POST");
	_methods.push_back("DELETE");
	// clang-format on
}

const std::string& Constants::to_response_string(uint32_t code) const {
	std::map<uint32_t, std::string>::const_iterator it = _response_str.find(code);
	assert(it != _response_str.end());
	return it->second;
}

bool Constants::is_valid_method(const std::string& method) const {
	for (size_t i = 0; i < _methods.size(); i++)
		if (_methods[i] == method)
			return true;
	return false;
}

std::string Constants::webserver_name() const {
	return "webbysetserv/3.0";
}
