#pragma once

#include "main.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <vector>

class Config;

extern Config g_config;

/*
How to get to the variable you want you asked?
Here is a small explanation:
first: you need to know in which server block you want to search so you can enter the serverblock with:
config._server[x]
second: you want to know if you're dealing with a Location block within a server block or just a serverblock variable
so it will be either:
* config._server[x]._location[y]
* config._server[x]._variable_name
*/
class Config {
  public:
	Config(int argc, char** argv);
	Config(){};
	~Config();

	class Location {
		friend class Config;
		friend class Server;

	  public:
		Location(){};
		~Location(){};

		// READONLY
		std::string							_path;
		std::string							_root;
		std::vector<std::string*>			_methods;
		std::string							_autoIndex;
		std::string							_defaultfile;
		std::pair<std::string, std::string> _cgiPath;
		std::vector<uint32_t>				_port;
		std::vector<std::string>			_ip;
		std::map<size_t, std::string>		_error_pages;

	  private:
	};
	class Server {
		friend class Config;
		friend class Location;

	  public:
		Server(){};
		~Server(){};

		// READONLY
		std::vector<uint32_t>		  _port;
		std::vector<std::string>	  _ip;
		std::vector<std::string>	  _serverName;
		std::string					  _serverUrl;
		std::string					  _root;
		std::string					  _autoIndex;
		std::string					  _client_max_body_size;
		std::map<size_t, std::string> _error_pages;
		std::vector<Location>		  _location;
		std::vector<std::string>	  _methods;
		size_t						  _number_methods;
		std::string					  _cgiPath;

	  private:
	};
	std::vector<Server> _server;

  private:
	void _parse_server_name(std::map<const std::string, std::string>& config_info);
	void _parse_listen(std::map<const std::string, std::string>& config_info);
	void _parse_error_page(std::map<const std::string, std::string>& config_info);
	void _parse_client_max_body_size(std::map<const std::string, std::string>& config_info);
	void _parse_allowed_methods(std::map<const std::string, std::string>& config_info);
	void _parseRoot(std::map<const std::string, std::string>& config_info);
	void _parse_index(std::map<const std::string, std::string>& config_info);
	void _parse_auto_index(std::map<const std::string, std::string>& config_info);
	void _parse_location(std::map<const std::string, std::string>& config_info);
	void _parse_cgi(std::map<const std::string, std::string>& config_info);
	void _print_class();
	void _config_parser(int argc, char** argv);
	void _safe_info(std::string line, std::map<const std::string, std::string>& config_info, std::vector<std::string>& options);
	bool _server_check;
	bool _location_check;
};

std::ostream& operator<<(std::ostream& stream, Config const& config);
