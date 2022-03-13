#include "config_parser.hpp"

unsigned int Config::get_port() const {
	return _server[_current_server]._port[0];
}

// void Server::set_ip(std::string ip_adres) {
// 	_ip = ip_adres;
// }

// std::string Server::get_ip() const {
// 	return _ip;
// }

// void Server::set_serverName(std::string name) {
// 	_serverName = name;
// }

// std::string Server::get_serverName() const {
// 	return _serverName;
// }

// void Server::set_serverUrl(std::string url) {
// 	_serverUrl = url;
// }

// std::string Server::get_serverUrl() const {
// 	return _serverUrl;
// }

// void Server::set_root(std::string set) {
// 	_root = set;
// }

// std::string Server::get_root() const {
// 	return _root;
// }

// void Server::set_errorPages(size_t error_code, std::string error) {
// 	_error_pages[error_code] = error;
// }

// std::string Server::get_errorPages(size_t error_code) const {
// 	return _error_pages[error_code];
// }

// void	Server::set_clientMaxBodySize(std::string size){
// 	_client_max_body_size = size;
// }

// std::string	Server::get_clientMaxBodySize() const {
// 	return _client_max_body_size;
// }
