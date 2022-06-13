#include "config_parser.hpp"
#include "constants.hpp"
#include "file_system.hpp"
#include "main.hpp"
#include <math.h>

const std::string Config::_error_return() {
	return "Config error: line " + std_ft::to_string(_line_count) + ": \"" + _line + "\": ";
}

// getting rid of the ';' and its precending whitespace
void Config::_cut_till_collon(std::string& line) {
	size_t collon_count = 0;
	for (size_t i = 0; i < line.length(); ++i) {
		if (line[i] == ';')
			collon_count++;
	}
	if (collon_count > 1)
		exit_with::message(_error_return() + "too many semicolons");
	size_t end = line.find(";");
	if (end == std::string::npos)
		exit_with::message(_error_return() + "missing semicolon");
	while (end > 1 && std::iswspace(line[end - 1]))
		end--;
	if (end == 0)
		exit_with::message(_error_return() + "line is empty");
	line = line.substr(0, end);
}

static void cut_till_bracket(std::string& line) {
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

	if (_inside_location)
		exit_with::message(_error_return() + "directive only allowed in server scope");
	_cut_till_collon(serverName);
	std::vector<std::string> serverName_splitted = ft_split(serverName, "\t ");
	for (std::string serverName : serverName_splitted)
		servers[servers.size() - 1].server_names.push_back(serverName);
}

void Config::_check_for_numeric_argument(std::vector<std::string>& listen_splitted) {
	for (std::string str : listen_splitted) {
		for (size_t i = 0; i < str.length(); i++) {
			if (!isdigit(str[i]))
				exit_with::message(_error_return() + "argument should be numeric");
		}
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
	size_t		check_dots = 0;
	uint32_t	port;
	std::string ip;

	if (_inside_location)
		exit_with::message(_error_return() + "directive only allowed in server scope");
	_cut_till_collon(listen);
	for (size_t i = 0; i < listen.length(); i++) {
		if (listen[i] == '.')
			check_dots++;
	}
	if (check_dots > 3)
		exit_with::message(_error_return() + "too many dots");
	std::vector<std::string> listen_splitted = ft_split(listen, " \t");
	if (listen_splitted.size() > 1)
		exit_with::message(_error_return() + "invalid number of arguments");
	listen_splitted.clear();
	listen_splitted = ft_split(listen, ".:");
	_check_for_numeric_argument(listen_splitted);
	if (listen_splitted.size() == 4 || listen_splitted.size() == 5) {
		for (std::string str : listen_splitted) {
			if (str.compare(listen_splitted[listen_splitted.size() - 1]) && listen_splitted.size() == 5)
				break;
			if (str.find_first_not_of("[0123456789]") != std::string::npos || stoi(str) > 225 || stoi(str) < 0)
				exit_with::message(_error_return() + "not a right given value");
		}
	} else if (listen_splitted.size() == 1) {
		ip = "127.0.0.1";
		port = stoi(listen_splitted[listen_splitted.size() - 1]);
	}
	port = stoi(listen_splitted[listen_splitted.size() - 1]);
	if (listen_splitted.size() == 5)
		ip = listen.substr(0, listen.find_first_of(":"));
	else if (listen_splitted.size() != 1)
		ip = listen;
	if (listen_splitted.size() == 4)
		port = 8080;
	if (port < ntohs(32768) || port > ntohs(61000))
		exit_with::message(_error_return() + "invalid port");
	for (size_t cmp_port : servers[servers.size() - 1].ports) {
		if (cmp_port == port)
			exit_with::message(_error_return() + "duplicate port");
	}
	servers[servers.size() - 1].ports.push_back(port);
	servers[servers.size() - 1].ips.push_back(ip);
}

// error pages are saved in a map structure so you can see what html file to use for what error code
// last value of the directive is considered the path to the error page
// other values of the directive are considered error codes (size_t)
void Config::_parse_error_page(std::map<const std::string, std::string>& config_info) {
	std::string error_page = config_info["error_page"];
	size_t		error_code;

	if (_inside_location)
		exit_with::message(_error_return() + "directive only allowed in server scope");
	_cut_till_collon(error_page);
	std::vector<std::string> splitted_error_page = ft_split(error_page, " \t");
	if (splitted_error_page.size() < 2)
		exit_with::message(_error_return() + "invalid number of arguments");
	for (std::string str : splitted_error_page) {
		if (str == splitted_error_page[splitted_error_page.size() - 1])
			return;
		if (!parse_int(error_code, str))
			exit_with::message(_error_return() + "invalid value: \"" + str + "\"");
		if (error_code < 300 || error_code > 599)
			exit_with::message(_error_return() + "error code must be between 300 and 599");
		if (!servers[servers.size() - 1].error_pages[error_code].empty()) // if error pages[error_code] is not empty => keep the old path?
			continue;
		servers[servers.size() - 1].error_pages[error_code] = splitted_error_page[splitted_error_page.size() - 1]; // Should this be the last one always??
	}
}

void Config::_parse_client_max_body_size(std::map<const std::string, std::string>& config_info) {
	if (_inside_location)
		exit_with::message(_error_return() + "directive only allowed in server scope");

	static const std::string multipliers = "KMGTP";
	std::string				 size = config_info["client_max_body_size"];
	_cut_till_collon(size);

	if (size.size() == 0)
		exit_with::message(_error_return() + "is empty");

	char unit = !std::isdigit(size[size.size() - 1]) ? std::toupper(size[size.size() - 1]) : 0;
	if (unit && multipliers.find(unit) == std::string::npos)
		exit_with::message(_error_return() + "invalid unit");
	if (unit)
		size.resize(size.size() - 1);

	size_t num;
	if (!parse_int(num, size))
		exit_with::message(_error_return() + "non numeric argument");
	if (unit)
		num *= pow(1024, multipliers.find(unit) + 1);

	servers[servers.size() - 1].client_max_body_size = num;
}

void Config::_add_methods(const std::string& methods_str, std::vector<std::string>& methods) {
	std::vector<std::string> methods_split = ft_split(methods_str, " \t");
	for (size_t i = 0; i < methods_split.size(); i++) {
		if (!g_constants.is_valid_method(methods_split[i]))
			exit_with::message(_error_return() + "wrong method");
		else
			methods.push_back(methods_split[i]);
	}
}

void Config::_parse_allowed_methods(std::map<const std::string, std::string>& config_info) {
	std::string methods_str = config_info["allowed_methods"];

	if (!_inside_location)
		exit_with::message(_error_return() + "directive only allowed in location scope");
	_last_location().allowed_methods.clear();
	_cut_till_collon(methods_str);
	_add_methods(methods_str, _last_location().allowed_methods);
}

void Config::_parse_root(std::map<const std::string, std::string>& config_info) {
	std::string				 root = config_info["root"];
	std::vector<std::string> root_splitted = ft_split(root, "\t ");

	_cut_till_collon(root);
	if (root_splitted.size() > 1)
		exit_with::message(_error_return() + "invalid number of arguments");
	if (!_last_location().root.empty())
		exit_with::message(_error_return() + "allowed");
	else if (_inside_location)
		_last_location().root = root;
	else
		exit_with::message(_error_return() + "directive only allowed in location scope");
}

void Config::_parse_location(std::map<const std::string, std::string>& config_info) {
	std::string location = config_info["location"];

	_inside_location++;
	if (_safe_new_path_location == false && !servers[servers.size() - 1].locations.empty())
		_what_location[_inside_location] = _last_location().path;
	cut_till_bracket(location);
	std::vector<std::string> location_splitted = ft_split(location, "\t ");
	if (location_splitted.size() > 1)
		exit_with::message(_error_return() + "invalid number of arguments");
	if (_inside_location > 1) {
		location = _what_location[_inside_location] + location;
	}
	servers[servers.size() - 1].locations.push_back(Location());
	_last_location() = (Location());
	_last_location().path = location;
	_safe_new_path_location = false;
}

void Config::_parse_index(std::map<const std::string, std::string>& config_info) {
	std::string index = config_info["index"];

	if (!_inside_location)
		exit_with::message(_error_return() + "directive only allowed in location scope");
	_cut_till_collon(index);
	_last_location().indexes = ft_split(index, " \t");
}

void Config::_parse_auto_index(std::map<const std::string, std::string>& config_info) {
	std::string autoIndex = config_info["autoindex"];

	if (!_inside_location)
		exit_with::message(_error_return() + "directive only allowed in location scope");
	_cut_till_collon(autoIndex);
	if (autoIndex.compare("on") != 0 && autoIndex.compare("off") != 0)
		exit_with::message(_error_return() + "wrong argument");
	_last_location().auto_index = autoIndex;
}

void Config::_parse_cgi(std::map<const std::string, std::string>& config_info) {
	std::string				 cgi = config_info["cgi"];
	std::vector<std::string> cgi_splitted = ft_split(cgi, "\t ");

	if (!_inside_location)
		exit_with::message(_error_return() + "directive only allowed in location scope");
	_cut_till_collon(cgi);
	if (cgi_splitted.size() > 2)
		exit_with::message(_error_return() + "invalid number of arguments");
	_last_location().cgi_path.first = cgi_splitted[0];
	if (cgi_splitted.size() == 2)
		_last_location().cgi_path.second = cgi_splitted[1];
}

void Config::_parse_return(std::map<const std::string, std::string>& config_info) {
	std::string ret = config_info["return"];
	int			redirect_code;

	if (!_inside_location)
		exit_with::message(_error_return() + "directive only allowed in location scope");
	_cut_till_collon(ret);

	std::vector<std::string> redirects = ft_split(ret, " \t");
	if (redirects.size() > 2)
		exit_with::message(_error_return() + "invalid number of arguments");
	if (!parse_int(redirect_code, redirects[0]))
		exit_with::message(_error_return() + "invalid value: \"" + redirects[0] + "\"");
	if (redirect_code < 0 || redirect_code > 1000)
		exit_with::message(_error_return() + "invalid return code: " + redirects[0]);
	_last_location().redirect.code = redirect_code;
	if (redirects.size() == 2)
		_last_location().redirect.text = redirects[1];
}

void Config::_parse_upload_pass(std::map<const std::string, std::string>& config_info) {
	std::string upload_pass = config_info["upload_pass"];

	if (!_inside_location)
		exit_with::message(_error_return() + "directive only allowed in location scope");
	_cut_till_collon(upload_pass);
	std::vector<std::string> upload_pass_splitted = ft_split(upload_pass, "\t ");
	if (upload_pass_splitted.size() > 1)
		exit_with::message(_error_return() + "invalid number of arguments");
	_last_location().upload_pass = upload_pass;
}
