#pragma once

#include "main.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

class Location {
  public:
	Location(const std::string location);
	~Location();
	std::string getLocation() const;
	void		setLocation(const std::string location);

  private:
	std::string _location;
};

class Server {
  public:
	Server();
	~Server();

  private:
};

class Config {
  public:
	Config(int argc, char** argv);
	~Config();
	void		 set_port(unsigned int set);
	unsigned int get_port() const;
	void		 set_ip(std::string ip_adres);
	std::string	 get_ip() const;
	void		 set_serverName(std::string name);
	std::string	 get_serverName() const;
	void		 set_serverUrl(std::string url);
	std::string	 get_serverUrl() const;
	void		 set_root(std::string set);
	std::string	 get_root() const;

  private:
	void				_parseServerName(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseListen(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseErrorPage(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseClientMaxBodySize(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseAllowedMethods(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseRoot(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseIndex(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseAutoIndex(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseServer(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void				_parseLocation(std::string option, std::map<const std::string, std::string>& config_info, std::string line);

	void				_config_parser(int argc, char** argv);
	void				safe_info(std::string line, std::map<const std::string, std::string>& config_info, std::vector<std::string>& options);
	unsigned int		_port;
	std::vector<Server> _server;

	std::string			_ip;
	std::string			_serverName;
	std::string			_serverUrl;
	std::string			_root;
	// std::map<size_t, std::string> _location;
	std::string						_autoIndex;
	std::string						_client_max_body_size;
	std::map<size_t, std::string>	_error_pages;
	bool							_location_check;
	size_t							_current_location;
	std::map<std::string, Location> _location;
	std::vector<std::string>		_methods;
	size_t							_number_methods;
};

bool parse_int(unsigned int& output, const std::string& str);
