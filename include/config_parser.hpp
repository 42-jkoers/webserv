#pragma once

#include "main.hpp"
#include <fstream>
#include <iostream>
#include <map>

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
	void		 _parseServerName(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void		 _parseListen(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void		 _parseErrorPage(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void		 _parseClientMaxBodySize(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void		 _parseAllowedMethods(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void		 _parseRoot(std::string option, std::map<const std::string, std::string>& config_info, std::string line);
	void		 _config_parser(char** argv);
	void		 safe_info(std::string line, std::map<const std::string, std::string>& config_info, std::vector<std::string>& options);
	unsigned int _port;
	std::string	 _ip;
	std::string	 _serverName;
	std::string	 _serverUrl;
	std::string	 _root;
};
