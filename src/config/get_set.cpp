#include "config_parser.hpp"

void Config::set_port(unsigned int set) {
	_port = set;
}

unsigned int Config::get_port() const {
	return _port;
}

void Config::set_ip(std::string ip_adres) {
	_ip = ip_adres;
}

std::string Config::get_ip() const {
	return _ip;
}

void Config::set_serverName(std::string name) {
	_serverName = name;
}

std::string Config::get_serverName() const {
	return _serverName;
}

void Config::set_serverUrl(std::string url) {
	_serverUrl = url;
}

std::string Config::get_serverUrl() const {
	return _serverUrl;
}

void Config::set_root(std::string set) {
	_root = set;
}

std::string Config::get_root() const {
	return _root;
}
