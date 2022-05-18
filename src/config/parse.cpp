
#include "config_parser.hpp"
#include "constants.hpp"
#include "file_system.hpp"
#include "main.hpp"

// getting rid of the ';' and its precending whitespace
void cut_till_collon(std::string& line) {
	size_t end = line.find(";");
	if (end == std::string::npos)
		exit_with::message("config error: line \"" + line + "\" missing semicolon");
	while (end > 1 && std::iswspace(line[end - 1]))
		end--;
	if (end == 0)
		exit_with::message("config error: line \"" + line + "\" is empty");
	line = line.substr(0, end);
}

void cut_till_bracket(std::string& line) {
	size_t find_bracket;
	size_t end;

	find_bracket = line.find_last_not_of("\t ");
	if (line[find_bracket] != '{')
		exit_with::message("config error: line");
	end = line.find_last_not_of("\t {");
	line = line.substr(0, end + 1); // getting rid of the ';' and whitespace
}

Config::Location& Config::_last_location() {
	if (!servers.size())
		servers.push_back(Server());
	if (!servers[servers.size() - 1].locations.size())
		servers[servers.size() - 1].locations.push_back(Location());
	return servers[servers.size() - 1].locations[servers[servers.size() - 1].locations.size() - 1];
}

/* How to check which server to connect to through server name
1. exact name
2. longest wildcard name starting with an asterisk, e.g. “*.example.org”
3. longest wildcard name ending with an asterisk, e.g. “mail.*”
*/

void Config::_parse_server_name(std::map<const std::string, std::string>& config_info) {
	std::string serverName = config_info["server_name"];
	size_t		i = 0;

	if (_inside_location)
		exit_with::message("\"server_name\" directive only allowed in server scope");
	cut_till_collon(serverName);
	while (i < serverName.length() && i != std::string::npos) {
		if (serverName[i] != ' ' && serverName[i] != '\t') {
			servers[servers.size() - 1].server_names.push_back(serverName.substr(i, serverName.find_first_of(" \t", i) - i));
			i = serverName.find_first_of(" \t", i);
			if (i == std::string::npos)
				break;
		}
		i++;
	}
}

/*
 Function _parse_listen:
 searches if there are 2 parts on the listen line with ':' and if there is an ip adress present.
 then it will count all the '.' to see if the ip adress is valid. And check if the char is actually a digit or not.
 errors if something isn't right.
 then sets the right position to actually safe the ip adress and the port
 checks if it should be safed in location or in _server
 if there is no port the port will be set to 8080
 if there is no ip the ip will be set to 127.0.0.1
 */

void Config::_parse_listen(std::map<const std::string, std::string>& config_info) {
	std::string listen = config_info["listen"];
	size_t		i = 0;
	size_t		check_dots = 0;
	size_t		check_ip = 0;

	if (_inside_location)
		exit_with::message("\"listen\" directive only allowed in server scope");
	cut_till_collon(listen);
	if (strchr(listen.c_str(), '.') || strchr(listen.c_str(), ':')) {
		while (i < listen.size()) {
			if (listen[i] != '.' && !isdigit(listen[i]) && listen[i] != ':' && listen[i] != ';')
				exit_with::message("config error: listen");
			if (listen[i] == '.') {
				check_ip = 0;
				if (check_dots > 2) // check if its a real ip address
					exit_with::message("config error: listen");
				check_dots++;
			}
			if (check_ip > 2 && check_dots != 3) // check if its only 3 numbers long
				exit_with::message("config error: listen");
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
		servers[servers.size() - 1].ips.push_back(listen.substr(0, pos));
		if (!strchr(listen.c_str(), ':'))
			port = 8080;
		else
			parse_int(port, &config_info["listen"][pos + 1]);
	} else {
		parse_int(port, &config_info["listen"][0]);
		servers[servers.size() - 1].ips.push_back("127.0.0.1");
	}
	// TODO: validate this
	if (port < ntohs(32768) || port > ntohs(61000))
		exit_with::message("config error: invalid port");
	servers[servers.size() - 1].ports.push_back(port);
}

// error pages are safed in a map structure so you can see what html file to use for what error code
void Config::_parse_error_page(std::map<const std::string, std::string>& config_info) {
	std::string error = config_info["error_page"];
	size_t		error_code;

	if (!_inside_location)
		exit_with::message("\"error_page\" directive only allowed in location scope");
	cut_till_collon(error);
	std::vector<std::string> splitted_error_codes = ft_split(error, " \t");
	for (std::string i : splitted_error_codes){
		std::stringstream sstream(i);
		sstream >> error_code;
		_last_location().error_pages[error_code] = splitted_error_codes[splitted_error_codes.size() - 1]; // Should this be the last one always??
		// std::cout << _last_location().error_pages[error_code] << std::endl;
	}
}

void Config::_parse_client_max_body_size(std::map<const std::string, std::string>& config_info) {
	std::string body_size = config_info["client_max_body_size"];

	if (_inside_location)
		exit_with::message("\"client_max_body_size\" directive only allowed in server scope");
	cut_till_collon(body_size);
	if (body_size[body_size.size() - 1] != 'M' && body_size[body_size.size() - 1] != 'm' && body_size[body_size.size() - 1] != 'K' && body_size[body_size.size() - 1] != 'k')
		exit_with::message("config error: client_max_body_size");
	servers[servers.size() - 1].client_max_body_size = body_size;
}

void Config::_add_methods(const std::string& methods_str, std::vector<std::string>& methods) {
	std::vector<std::string> methods_split = ft_split(methods_str, " \t");
	for (size_t i = 0; i < methods_split.size(); i++) {
		if (!g_constants.is_valid_method(methods_split[i]))
			exit_with::message("config error: wrong method");
		else
			methods.push_back(methods_split[i]);
	}
}

void Config::_parse_allowed_methods(std::map<const std::string, std::string>& config_info) {
	std::string methods_str = config_info["allowed_methods"];

	if (!_inside_location)
		exit_with::message("\"allowed_methods\" directive only allowed in location scope");
	_last_location().allowed_methods.clear();
	cut_till_collon(methods_str);
	_add_methods(methods_str, _last_location().allowed_methods);
}

void Config::_parse_root(std::map<const std::string, std::string>& config_info) {
	std::string root = config_info["root"];

	cut_till_collon(root);
	if (!_last_location().root.empty())
		exit_with::message("config error: No duplicate \"root\" allowed");
	else if (_inside_location)
		_last_location().root = root;
	else
		exit_with::message("\"root\" directive only allowed in location scope");
}

void Config::_parse_location(std::map<const std::string, std::string>& config_info) {
	std::string location = config_info["location"];

	_inside_location++;
	if (_safe_new_path_location == false && !servers[servers.size() - 1].locations.empty())
		_what_location[_inside_location] = _last_location().path;
	cut_till_bracket(location);
	if (_inside_location > 1) {
		location = _what_location[_inside_location] + location;
	}
	servers[servers.size() - 1].locations.push_back(Location());
	_last_location() = (Location());
	_last_location().path = location;
	// std::cout << _inside_location << "  | " << _last_location().path << std::endl;
	_safe_new_path_location = false;
}

void Config::_parse_index(std::map<const std::string, std::string>& config_info) {
	std::string index = config_info["index"];

	if (!_inside_location)
		exit_with::message("\"index\" directive only allowed in location scope");
	cut_till_collon(index);
	_last_location().indexes = ft_split(index, " \t");
}

void Config::_parse_auto_index(std::map<const std::string, std::string>& config_info) {
	std::string autoIndex = config_info["autoindex"];

	if (!_inside_location)
		exit_with::message("\"auto index\" directive only allowed in location scope");
	cut_till_collon(autoIndex);
	if (autoIndex.compare("on") != 0 && autoIndex.compare("off") != 0)
		exit_with::message("config error: autoindex");
	_last_location().auto_index = autoIndex;
}

void Config::_parse_cgi(std::map<const std::string, std::string>& config_info) {
	std::string cgi = config_info["cgi"];

	if (!_inside_location)
		exit_with::message("\"cgi\" directive only allowed in location scope");
	cut_till_collon(cgi);
	size_t		space = cgi.find_first_of(" \t");
	size_t		not_space = cgi.find_first_not_of(" \t", space);
	std::string path = cgi.substr(not_space, cgi.size());
	if (_inside_location) {
		_last_location().cgi_path.first = cgi.substr(0, space);
		_last_location().cgi_path.second = path;
	}
}

void Config::_parse_return(std::map<const std::string, std::string>& config_info) {
	std::string ret = config_info["return"];

	if (!_inside_location)
		exit_with::message("\"redirect\" directive only allowed in location scope");
	cut_till_collon(ret);
	size_t found_redirect = ret.find("301");
	if (found_redirect == std::string::npos)
		exit_with::message("config error: redirect");
	_last_location().redirect = ret.substr(ret.find_first_not_of("301 \t", found_redirect, ret.length() - found_redirect));
}
