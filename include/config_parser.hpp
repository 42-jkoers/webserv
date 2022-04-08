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
* config._server[x].location[y]
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
		std::string							root;
		std::vector<std::string>			methods;
		size_t								number_methods;
		std::string							auto_index;
		std::string							_defaultfile;
		std::pair<std::string, std::string> cgi_path;
		std::vector<uint32_t>				port;
		std::vector<std::string>			ip;
		std::map<size_t, std::string>		error_pages;

	  private:
	};
	class Server {
		friend class Config;
		friend class Location;

	  public:
		Server(){};
		~Server(){};

		// READONLY
		std::vector<uint32_t>		  port;
		std::vector<std::string>	  ip;
		std::vector<std::string>	  server_name;
		std::string					  server_url;
		std::string					  root;
		std::string					  auto_index;
		std::string					  client_max_body_size;
		std::map<size_t, std::string> error_pages;
		std::vector<Location>		  location;
		std::size_t					  equal;
		std::vector<std::string>	  methods;
		size_t						  number_methods;
		std::string					  cgi_path;

	  private:
	};
	std::map<size_t, std::vector<size_t> /**/> _ports_servers;
	std::vector<Server>						   _servers;

  private:
	Location& last_location();
	void	  _parse_server_name(std::map<const std::string, std::string>& config_info);
	void	  _parse_listen(std::map<const std::string, std::string>& config_info);
	void	  _parse_error_page(std::map<const std::string, std::string>& config_info);
	void	  _parse_client_max_body_size(std::map<const std::string, std::string>& config_info);
	void	  _parse_allowed_methods(std::map<const std::string, std::string>& config_info);
	void	  _parseRoot(std::map<const std::string, std::string>& config_info);
	void	  _parse_index(std::map<const std::string, std::string>& config_info);
	void	  _parse_auto_index(std::map<const std::string, std::string>& config_info);
	void	  _parse_location(std::map<const std::string, std::string>& config_info);
	void	  _parse_cgi(std::map<const std::string, std::string>& config_info);
	void	  _print_class();
	void	  _config_parser(int argc, char** argv);
	void	  _safe_info(std::string line, std::map<const std::string, std::string>& config_info, std::vector<std::string>& options);
	void	  _add_methods(const std::string& methods_str, std::vector<std::string>& methods);
	bool	  _inside_server;
	bool	  _inside_location;
};

std::ostream& operator<<(std::ostream& stream, Config const& config);
