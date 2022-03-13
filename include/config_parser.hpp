#pragma once

#include "main.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

class Config {
  public:
	Config(int argc, char** argv);
	~Config();
	uint32_t get_port() const;
	class Location {
		friend class Config;
		friend class Server;

	  public:
		Location(){};
		~Location(){};
		std::string getLocation() const;
		void		setLocation(const std::string location);

	  private:
		std::string					  _path;
		std::string					  _root;
		std::vector<std::string>	  _methods;
		std::string					  _autoIndex;
		std::string					  _defaultfile;
		std::string					  _cgi;
		std::vector<uint32_t>		  _port;
		std::vector<std::string>	  _ip;
		std::map<size_t, std::string> _error_pages;
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
		void		set_root(std::string set);
		std::string get_root() const;
		void		set_errorPages(size_t error_code, std::string error);
		std::string get_errorPages(size_t error) const;
		void		set_clientMaxBodySize(std::string size);
		std::string get_clientMaxBodySize() const;

	  private:
		std::vector<uint32_t>	 _port;
		std::vector<std::string> _ip;
		std::string				 _serverName;
		std::string				 _serverUrl;
		std::string				 _root;
		// std::map<size_t, std::string> _location;
		std::string					  _autoIndex;
		std::string					  _client_max_body_size;
		std::map<size_t, std::string> _error_pages;
		std::vector<Location>		  _location;
		std::vector<std::string>	  _methods;
		size_t						  _number_methods;
	};

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
	std::vector<Server> _server;
	size_t				_current_server;
	bool				_location_check;
	size_t				_current_location;
	bool				_server_check;
};

bool parse_int(unsigned int& output, const std::string& str);
