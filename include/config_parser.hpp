#pragma once

#include "main.hpp"
#include <fstream>
#include <iostream>
#include <map>


// enum functions {
// 	ParseServerName,
// 	ParseListen,
// 	ParseErrorPage,
// 	ParseClientMaxBodySize,
// 	ParseAllowedMethods,
// 	ParseRoot
// };

// void (*functionPointers[6])() = {parseServerName, parseListen, parseErrorPage, parseClientMaxBodySize, parseAllowedMethods, parseRoot};

class config
{
public:
	config(/* args */);
	~config();
	void			set_port(unsigned int set);
	unsigned int	get_port();
	void			set_ip(std::string ip_adres);
	std::string		get_ip();
	void			set_serverName(std::string name);
	std::string		get_serverName();
	void			set_serverUrl(std::string url);
	std::string		get_serverUrl();
	void			set_root(std::string set);
	std::string		get_root();

private:
	unsigned int	port;
	std::string		ip;
	std::string		serverName;
	std::string		serverUrl;
	std::string		root;
	// std::vector<std::string> methods;

};

void parseServerName(std::string option, std::map<const std::string, std::string>& config_info, std::string line, class config& config);
void parseListen(std::string option, std::map<const std::string, std::string>& config_info, std::string line, class config& config);
void parseErrorPage(std::string option, std::map<const std::string, std::string>& config_info, std::string line, class config& config);
void parseClientMaxBodySize(std::string option, std::map<const std::string, std::string>& config_info, std::string line, class config& config);
void parseAllowedMethods(std::string option, std::map<const std::string, std::string>& config_info, std::string line, class config& config);
void parseRoot(std::string option, std::map<const std::string, std::string>& config_info, std::string line, class config& config);
std::string	get_html_request(config& config);


int config_parser(config& config, char **argv);
bool parse_int(unsigned int& output, const std::string& str);



// typedef struct config {
// 	std::map<int, std::string> config;
// };
