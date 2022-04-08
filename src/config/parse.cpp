
#include "config_parser.hpp"
#include "constants.hpp"
#include "file_system.hpp"
#include "main.hpp"

void cut_till_collon(std::string& line) {
	size_t find_collon;
	size_t end;

	find_collon = line.find_last_not_of("\t ");
	if (line[find_collon] != ';')
		exit_with::e_perror("config error: line");
	end = line.find_last_not_of("\t ;");
	line = line.substr(0, end + 1); // getting rid of the ';' and whitespace
}

void cut_till_bracket(std::string& line) {
	size_t find_bracket;
	size_t end;

	find_bracket = line.find_last_not_of("\t ");
	if (line[find_bracket] != '{')
		exit_with::e_perror("config error: line");
	end = line.find_last_not_of("\t {");
	line = line.substr(0, end + 1); // getting rid of the ';' and whitespace
}

Config::Location& Config::last_location() {
	if (!_servers.size())
		_servers.push_back(Server());
	if (!_servers[_servers.size() - 1].location.size())
		_servers[_servers.size() - 1].location.push_back(Location());
	return _servers[_servers.size() - 1].location[_servers[_servers.size() - 1].location.size() - 1];
}

/* How to check which server to connect to through server name
1. exact name
2. longest wildcard name starting with an asterisk, e.g. “*.example.org”
3. longest wildcard name ending with an asterisk, e.g. “mail.*”
*/

void Config::_parse_server_name(std::map<const std::string, std::string>& config_info) {
	std::string serverName = config_info["server_name"];
	size_t		i = 0;
	cut_till_collon(serverName);
	while (i < serverName.length() && i != std::string::npos) {
		if (serverName[i] != ' ' && serverName[i] != '\t') {
			_servers[_servers.size() - 1].server_name.push_back(serverName.substr(i, serverName.find_first_of(" \t", i) - i));
			i = serverName.find_first_of(" \t", i);
			if (i == std::string::npos)
				break;
		}
		i++;
	}
	// for (size_t j = 0; j < _server[_server.size() - 1]._serverName.size(); j++) {
	// 	std::cout << _server[_server.size() - 1]._serverName[j] << std::endl;
	// }
}

/*
 Function _parse_listen:
 searches if there are 2 parts on the listen line with ':' and if there is an ip adress present.
 then it will count all the '.' to see if the ip adress is valid. And check if the char is actually a digit or not.
 errors if something isn't right.
 then sets the right position to actually safe the ip adress and the port
 checks if it should be safed in location or in _server
 if there is no port the port will be set to 8080
 if there is no ip the ip will be set to 0.0.0.0
 */
void Config::_parse_listen(std::map<const std::string, std::string>& config_info) {
	std::string listen = config_info["listen"];
	size_t		i = 0;
	size_t		check_dots = 0;
	size_t		check_ip = 0;

	// std::cout << "hello I am here" << _servers.size() - 1 << std::endl;
	cut_till_collon(listen);
	if (strchr(listen.c_str(), '.') || strchr(listen.c_str(), ':')) {
		while (i < listen.size()) {
			if (listen[i] != '.' && !isdigit(listen[i]) && listen[i] != ':' && listen[i] != ';')
				exit_with::e_perror("config error: listen");
			if (listen[i] == '.') {
				check_ip = 0;
				if (check_dots > 2) // check if its a real ip address
					exit_with::e_perror("config error: listen");
				check_dots++;
			}
			if (check_ip > 2 && check_dots != 3) // check if its only 3 numbers long
				exit_with::e_perror("config error: listen");
			if (isdigit(listen[i]))
				check_ip++;
			i++;
		}
	}
	uint32_t			port;
	size_t				pos;
	std::vector<size_t> it;
	if (strchr(listen.c_str(), ':'))
		pos = config_info["listen"].find_first_of(":");
	else
		pos = listen.size();
	if (check_ip != 0) {
		if (_inside_location)
			last_location().ip.push_back(listen.substr(0, pos));
		else
			_servers[_servers.size() - 1].ip.push_back(listen.substr(0, pos));
		if (!strchr(listen.c_str(), ':'))
			port = 80;
		else
			parse_int(port, &config_info["listen"][pos + 1]);
	} else {
		parse_int(port, &config_info["listen"][0]);
		if (_inside_location)
			last_location().ip.push_back("0.0.0.0");
		else
			_servers[_servers.size() - 1].ip.push_back("0.0.0.0");
	}
	// TODO: validate this
	_servers[_servers.size() - 1].port.push_back(port);
}

void Config::_parse_error_page(std::map<const std::string, std::string>& config_info) {
	std::string error = config_info["error_page"];
	size_t		space;

	cut_till_collon(error);
	size_t error_code;
	space = error.find_first_of(" \t");
	if (space == std::string::npos)
		exit_with::e_perror("config error: error_page");
	std::stringstream sstream(error.substr(0, space).c_str());
	sstream >> error_code;
	if (_inside_location)
		last_location().error_pages[error_code] = error.substr(error.find_last_of(" \t"), error.size() - space);
	else
		_servers[_servers.size() - 1].error_pages[error_code] = error.substr(error.find_last_of(" \t"), error.size() - space);
	// std::cout << _server[_server.size() - 1].error_pages[error_code] << std::endl;
}

void Config::_parse_client_max_body_size(std::map<const std::string, std::string>& config_info) {
	std::string body_size = config_info["client_max_body_size"];
	cut_till_collon(body_size);
	if (body_size[body_size.size() - 1] != 'M' && body_size[body_size.size() - 1] != 'm')
		exit_with::e_perror("config error: client_max_body_size");
	_servers[_servers.size() - 1].client_max_body_size = body_size;
}

void Config::_add_methods(const std::string& methods_str, std::vector<std::string>& methods) {
	std::vector<std::string> methods_split = ft_split(methods_str, " \t");
	for (size_t i = 0; i < methods_split.size(); i++) {
		if (!g_constants.is_valid_method(methods_split[i]))
			exit_with::e_perror("config error: wrong method");
		else
			methods.push_back(methods_split[i]);
	}
}

// TODO: set methods in location aswell
void Config::_parse_allowed_methods(std::map<const std::string, std::string>& config_info) {
	std::string methods_str = config_info["allowed_methods"];

	cut_till_collon(methods_str);
	_servers[_servers.size() - 1].methods.push_back("");
	if (_inside_location)
		_add_methods(methods_str, last_location().methods);
	else
		_add_methods(methods_str, _servers[_servers.size() - 1].methods);
}

void Config::_parseRoot(std::map<const std::string, std::string>& config_info) {
	std::string root = config_info["root"];

	cut_till_collon(root);
	_servers[_servers.size() - 1].root = root;
}
/*
if there is a '=' a match if the requestif the request URI exactly matches the location given
you can see if this is the case with checking the equal variable. if it is 1 it should match if its 0 is doesn't
*/

void Config::_parse_location(std::map<const std::string, std::string>& config_info) {
	std::string location = config_info["location"];
	size_t		equal_sign;

	_inside_location = true;
	cut_till_bracket(location);
	if (strchr(location.c_str(), '=')) {
		_servers[_servers.size() - 1].equal = 1;
		equal_sign = location.find_first_of("=") + 1;
		equal_sign = location.find_first_not_of(" \t", equal_sign);
		location = location.substr(equal_sign, location.length() - equal_sign);
	} else
		_servers[_servers.size() - 1].equal = 0;
	if (location[0] == '/')
		location.insert(0, ".");
	else
		location.insert(0, "./");
	if (!fs::path_exists(location))
		exit_with::e_perror("config error: location");
	// std::cout << location << std::endl;
	_servers[_servers.size() - 1].location.push_back(Location());
	last_location() = (Location());
	last_location()._path = location;
}
// TODO: multiple indexes
void Config::_parse_index(std::map<const std::string, std::string>& config_info) {
	std::string	  index = config_info["index"];
	std::ifstream try_file;
	std::string	  path_to_file;

	cut_till_collon(index);
	path_to_file = last_location()._path + "/" + index;
	try_file.open(path_to_file);
	if (!try_file.is_open())
		exit_with::e_perror("config error: index");
	try_file.close();
}

void Config::_parse_auto_index(std::map<const std::string, std::string>& config_info) {
	std::string autoIndex = config_info["autoindex"];

	cut_till_collon(autoIndex);
	if (autoIndex.compare("on") != 0 && autoIndex.compare("off") != 0)
		exit_with::e_perror("config error: autoindex");
	if (_inside_location)
		last_location().auto_index = autoIndex;
	else
		_servers[_servers.size() - 1].auto_index = autoIndex;
}

void Config::_parse_cgi(std::map<const std::string, std::string>& config_info) {
	std::string cgi = config_info["cgi"];

	cut_till_collon(cgi);
	size_t		space = cgi.find_first_of(" \t");
	size_t		not_space = cgi.find_first_not_of(" \t", space);
	std::string path = cgi.substr(not_space, cgi.size());
	if (_inside_location) {
		last_location().cgi_path.first = cgi.substr(0, space);
		last_location().cgi_path.second = path;
	} else
		exit_with::e_perror("config error: cgi");
	// std::cout << _server[_server.size() - 1].location[_server[_server.size() - 1].location.size() - 1].cgi_path.first << " | " << _server[_server.size() - 1].location[_server[_server.size() - 1].location.size() - 1].cgi_path.second << std::endl;
}
