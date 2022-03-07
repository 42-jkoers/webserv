
#include "config_parser.hpp"
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

void Config::_parseServerName(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string serverName = config_info["server_name"];
	size_t		split;

	cut_till_collon(serverName);
	split = serverName.find_first_of(" \t");
	_serverName = serverName.substr(0, split);
	split = serverName.find_last_of("\t ");
	_serverUrl = serverName.substr(split + 1, serverName.size());
	// std::cout << config.get_serverName() << "\\" << config.get_serverUrl() << std::endl;
	(void)option;
	(void)line;
}

void Config::_parseListen(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string listen = config_info["listen"];
	size_t		i = 0;
	size_t		check_dots = 0;
	size_t		check_ip = 0;

	cut_till_collon(listen);
	if (strchr(listen.c_str(), ':')) {
		while (i < listen.size()) {
			if (listen[i] != '.' && !isdigit(listen[i]) && listen[i] != ':' && listen[i] != ';')
				exit_with::e_perror("config error: listen");
			if (listen[i] == '.') {
				check_ip = 0;
				if (check_dots > 2) // check if its a real ip adress
					exit_with::e_perror("config error: listen");
				check_dots++;
			}
			if (check_ip > 2 && check_dots != 3) // check if its only 3 numbers long
				exit_with::e_perror("config error: listen");
			if (isdigit(listen[i]))
				check_ip++;
			i++;
		}
		unsigned int port;
		size_t		 pos = config_info["listen"].find_first_of(":");
		_ip = listen.substr(0, pos);
		parse_int(port, &config_info["listen"][pos + 1]);
		_port = port;
	} else {
		unsigned int port;
		if (parse_int(port, listen) == false)
			exit_with::e_perror("config error: listen");
		_port = port;
	}
	(void)line;
	(void)option;
}

void Config::_parseErrorPage(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string error = config_info["error_page"];
	size_t		space;

	cut_till_collon(error);
	size_t error_code;
	space = error.find_first_of(" \t");
	if (space == std::string::npos)
		exit_with::e_perror("config error: error_page");
	error_code = atoi(error.substr(0, space).c_str());
	_error_pages[error_code] = error.substr(error.find_last_of(" \t"), error.size() - space);

	(void)option;
	(void)config_info;
	(void)line;
}

void Config::_parseClientMaxBodySize(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string body_size = config_info["client_max_body_size"];
	cut_till_collon(body_size);
	if (body_size[body_size.size() - 1] != 'M' && body_size[body_size.size() - 1] != 'm')
		exit_with::e_perror("config error: client_max_body_size");
	_client_max_body_size = body_size;

	(void)option;
	(void)config_info;
	(void)line;
}

void Config::_parseAllowedMethods(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string methods = config_info["allowed_methods"];
	// size_t		space = 0;
	size_t		i = 0;
	size_t		j = 0;
	// std::cout << methods << std::endl;

	// Make it work for multiple allowed methods
	cut_till_collon(methods);
	// space = methods.find_first_of("\t ");
	// _methods.push_back(methods.substr(0, space));
	// space = methods.find_first_not_of("\t ", space);
	// std::cout << _methods[0] << std::endl;
	_methods.push_back("");
	while (i < methods.length()) {
		if (methods[i] == '\t' || methods[i] == ' ') {
			_methods.push_back("");
			if (strncmp(_methods[j].c_str(), "GET", _methods[j].length()) != 0&& strncmp(_methods[j].c_str(), "POST",
				_methods[j].length()) != 0&& strncmp(_methods[j].c_str(), "SET", _methods[j].length())!= 0)
				exit_with::e_perror("config error: methods");
			j++;
		}
		else
			_methods[j].push_back(methods[i]);
		i++;
	}
	if (strncmp(_methods[j].c_str(), "GET", _methods[j].length()) != 0&& strncmp(_methods[j].c_str(), "POST",
				_methods[j].length()) != 0&& strncmp(_methods[j].c_str(), "SET", _methods[j].length())!= 0)
				exit_with::e_perror("config error: methods");
	_number_methods = j + 1;
	(void)config_info;
	(void)option;
	(void)line;
}

void Config::_parseRoot(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string root = config_info["root"];

	cut_till_collon(root);
	_root = root;
	(void)option;
	(void)config_info;
	(void)line;
}

bool IsPathExist(const std::string& s) {
	struct stat buffer;
	return (stat(s.c_str(), &buffer) == 0);
}

void Config::_parseLocation(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string location = config_info["location"];

	_location_check = true;
	cut_till_bracket(location);
	if (location[0] == '/')
		location.insert(0, ".");
	if (!IsPathExist(location))
		exit_with::e_perror("config error: location");
	_location.push_back(Location(location));

	(void)option;
	(void)config_info;
	(void)line;
}

void Config::_parseIndex(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string	  index = config_info["index"];
	std::ifstream try_file;
	std::string	  path_to_file;

	cut_till_collon(index);
	path_to_file = _location[_location.size() - 1].getLocation() + "/" + index;
	try_file.open(path_to_file);
	if (!try_file.is_open())
		exit_with::e_perror("config error: index");
	try_file.close();
	(void)option;
	(void)config_info;
	(void)line;
}

void Config::_parseServer(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	(void)option;
	(void)config_info;
	(void)line;
}

void Config::_parseAutoIndex(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string autoIndex = config_info["autoindex"];

	cut_till_collon(autoIndex);
	if (autoIndex.compare("on") != 0 && autoIndex.compare("off") != 0)
		exit_with::e_perror("config error: autoindex");
	_autoIndex = autoIndex;
	(void)option;
	(void)line;
}
