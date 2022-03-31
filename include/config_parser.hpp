#pragma once

#include "main.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <vector>

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_BLACK "\x1b[0m"

class Config;

extern Config g_config;

/*
How to get to the variable you want you asked?
Here is a small explaination:
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
	uint32_t	get_port() const;
	std::string get_root() const;
	class Location {
		friend class Config;
		friend class Server;

	  public:
		Location(){};
		~Location(){};
		std::string getLocation() const;
		void		setLocation(const std::string location);
		// READONLY!!!!!
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
		void		set_port(unsigned int set);
		void		set_ip(std::string ip_adres);
		std::string get_ip() const;
		void		set_serverName(std::string name);
		std::string get_serverName() const;
		void		set_serverUrl(std::string url);
		std::string get_serverUrl() const;
		std::string get_root() const;
		void		set_errorPages(size_t error_code, std::string error);
		std::string get_errorPages(size_t error) const;
		void		set_clientMaxBodySize(std::string size);
		std::string get_clientMaxBodySize() const;
		// READ ONLY!!!!!!!
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
	void _parseServerName(std::map<const std::string, std::string>& config_info);
	void _parseListen(std::map<const std::string, std::string>& config_info);
	void _parseErrorPage(std::map<const std::string, std::string>& config_info);
	void _parseClientMaxBodySize(std::map<const std::string, std::string>& config_info);
	void _parseAllowedMethods(std::map<const std::string, std::string>& config_info);
	void _parseRoot(std::map<const std::string, std::string>& config_info);
	void _parseIndex(std::map<const std::string, std::string>& config_info);
	void _parseAutoIndex(std::map<const std::string, std::string>& config_info);
	void _parseLocation(std::map<const std::string, std::string>& config_info);
	void _parseCgi(std::map<const std::string, std::string>& config_info);
	void _print_class();
	void _config_parser(int argc, char** argv);
	void _safe_info(std::string line, std::map<const std::string, std::string>& config_info, std::vector<std::string>& options);
	bool _server_check;
	bool _location_check;
};

bool		  parse_int(unsigned int& output, const std::string& str);
std::ostream& operator<<(std::ostream& stream, Config const& config);
