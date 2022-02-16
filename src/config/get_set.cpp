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
