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
	Config(const std::string& config_file_path);
	Config(){};
	~Config();

	struct t_redirect {
		size_t		code;
		std::string text;
	} s_redirect;

	class Location {
		friend class Config;
		friend class Server;

	  public:
		Location();
		~Location();

		// READONLY
		std::string							path;
		std::vector<std::string>			indexes;		 // If empty is set to: index.html
		std::vector<std::string>			allowed_methods; // If empty is set to: GET, POST, DELETE
		std::string							auto_index;		 // If empty is set to: off
		std::pair<std::string, std::string> cgi_path;
		t_redirect							redirect; // If empty is set to: 0
		std::string							upload_pass;
		std::map<size_t, std::string>		error_pages;
		std::size_t							redirect_code;
		std::string							root; // If empty is set to: www/html
	};

	class Server {
		friend class Config;
		friend class Location;

	  public:
		Server() {
			client_max_body_size = ~0;
		};
		~Server(){};

		// READONLY
		std::vector<uint16_t>		  ports;
		std::vector<std::string>	  ips; // If empty is set to:	127.0.0.1
		std::vector<std::string>	  server_names;
		size_t						  client_max_body_size; // default is max int value
		std::map<size_t, std::string> error_pages;
		std::vector<Location>		  locations;
	};

	std::vector<Server> servers;

  private:
	Location&					  _last_location();
	void						  _parse_server_name(std::map<const std::string, std::string>& config_info);
	void						  _parse_listen(std::map<const std::string, std::string>& config_info);
	void						  _parse_error_page(std::map<const std::string, std::string>& config_info);
	void						  _parse_client_max_body_size(std::map<const std::string, std::string>& config_info);
	void						  _parse_allowed_methods(std::map<const std::string, std::string>& config_info);
	void						  _parse_root(std::map<const std::string, std::string>& config_info);
	void						  _parse_index(std::map<const std::string, std::string>& config_info);
	void						  _parse_auto_index(std::map<const std::string, std::string>& config_info);
	void						  _parse_location(std::map<const std::string, std::string>& config_info);
	void						  _parse_cgi(std::map<const std::string, std::string>& config_info);
	void						  _parse_return(std::map<const std::string, std::string>& config_info);
	void						  _parse_upload_pass(std::map<const std::string, std::string>& config_info);
	void						  _print_class();
	void						  _config_parser(const std::string& config_file_path);
	void						  _safe_info(std::string& line, std::vector<std::string>& options);
	void						  _cut_till_collon(std::string& line);
	void						  _add_methods(const std::string& methods_str, std::vector<std::string>& methods);
	const std::string			  _error_return();
	void						  _check_for_numeric_argument(std::vector<std::string>& listen_splitted);
	bool						  _inside_server;
	int							  _inside_location;
	bool						  _safe_new_path_location;
	std::map<size_t, std::string> _what_location;
	size_t						  _line_count;
	std::string					  _line;

	typedef void (Config::*Jump_table)(std::map<const std::string, std::string>&);
	std::vector<Jump_table> _jump_table;
};

std::ostream& operator<<(std::ostream& stream, Config const& config);
