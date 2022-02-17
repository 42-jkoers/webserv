#include "config_parser.hpp"

void	config::set_port(unsigned int set)	{
	this->port = set;
}

unsigned int	config::get_port() {
	return this->port;
}

void	config::set_ip(std::string ip_adres) {
	this->ip = ip_adres;
}

std::string config::get_ip() {
	return this->ip;
}

void	config::set_serverName(std::string name) {
	this->serverName = name;
}

std::string	config::get_serverName() {
	return this->serverName;
}

void	config::set_serverUrl(std::string url) {
	this->serverUrl = url;
}

std::string	config::get_serverUrl() {
	return this->serverUrl;
}

void	config::set_root(std::string set) {
	this->root = set;
}

std::string	config::get_root() {
	return this->root;
}
