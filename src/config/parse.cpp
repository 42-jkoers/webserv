
#include "config_parser.hpp"
#include "main.hpp"

// char* ft_subStr(const char* cStr, int iStart, int iLength) {
// 	static char cRes[1024];
// 	char*		pRes = cRes;
// 	int			i = 0, iPos = 0;
// 	int			iByte;

// 	while (cStr[i] != '\0') {
// 		iByte = cntByte(cStr[i]);
// 		if (iStart <= iPos && iPos < iStart + iLength) {
// 			memcpy(pRes, (cStr + i), iByte);
// 			pRes += iByte;
// 		}
// 		i += iByte;
// 		iPos++;
// 	}
// 	*pRes = '\0';

// 	return cRes;
// }

// std::string serverName = config_info["server_name"];

// cut_till_collon(serverName);
// // size_t		i = 0;
// // while (i < serverName.length() && i != std::string::npos) {
// // 	if (serverName[i] != ' ' && serverName[i] != '\t') {
// // 		std::cout << "i = " << i << std::endl;
// // 		std::cout << serverName.substr(i, serverName.find_first_of(" \t", i)) << std::endl;
// // 		std::cout << "find first of = " << serverName.find_first_of(" \t", i) << std::endl;
// // 		_server[_server.size() - 1]._serverName.push_back(serverName.substr(i, serverName.find_first_of(" \t", i)));
// // 		i = serverName.find_first_of(" \t", i);
// // 		std::cout << "i2 = " << i << std::endl;
// // 		if (i == std::string::npos)
// // 			break;
// // 	}
// // 	i++;
// // }
// // for (size_t j = 0; j < _server[_server.size() - 1]._serverName.size(); j++) {
// // 	std::cout << _server[_server.size() - 1]._serverName[j] << std::endl;
// // }

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

void Config::_parseServerName(std::map<const std::string, std::string>& config_info) {
	std::string serverName = config_info["server_name"];
	size_t		i = 0;
	cut_till_collon(serverName);
	while (i < serverName.length() && i != std::string::npos) {
		if (serverName[i] != ' ' && serverName[i] != '\t') {
			_server[_server.size() - 1]._serverName.push_back(serverName.substr(i, serverName.find_first_of(" \t", i) - i));
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
 checks if it should be safed in _location or in _server
 if there is no port the port will be set to 8080
 if there is no ip the ip will be set to 0.0.0.0
 */
void Config::_parseListen(std::map<const std::string, std::string>& config_info) {
	std::string listen = config_info["listen"];
	size_t		i = 0;
	size_t		check_dots = 0;
	size_t		check_ip = 0;

	cut_till_collon(listen);
	if (strchr(listen.c_str(), '.') || strchr(listen.c_str(), ':')) {
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
	}
	uint32_t port;
	size_t	 pos;
	if (strchr(listen.c_str(), ':'))
		pos = config_info["listen"].find_first_of(":");
	else
		pos = listen.size();
	if (check_ip != 0) {
		if (_location_check == true)
			_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._ip.push_back(listen.substr(0, pos));
		else
			_server[_server.size() - 1]._ip.push_back(listen.substr(0, pos));
		if (!strchr(listen.c_str(), ':'))
			port = 8080;
		else
			parse_int(port, &config_info["listen"][pos + 1]);
	} else {
		parse_int(port, &config_info["listen"][0]);
		if (_location_check == true)
			_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._ip.push_back("0.0.0.0");
		else
			_server[_server.size() - 1]._ip.push_back("0.0.0.0");
	}
	if (_location_check == true)
		_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._port.push_back(port);
	else
		_server[_server.size() - 1]._port.push_back(port);
}

void Config::_parseErrorPage(std::map<const std::string, std::string>& config_info) {
	std::string error = config_info["error_page"];
	size_t		space;

	cut_till_collon(error);
	size_t error_code;
	space = error.find_first_of(" \t");
	if (space == std::string::npos)
		exit_with::e_perror("config error: error_page");
	std::stringstream sstream(error.substr(0, space).c_str());
	sstream >> error_code;
	if (_location_check == true)
		_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._error_pages[error_code] = error.substr(error.find_last_of(" \t"), error.size() - space);
	else
		_server[_server.size() - 1]._error_pages[error_code] = error.substr(error.find_last_of(" \t"), error.size() - space);
	// std::cout << _server[_server.size() - 1]._error_pages[error_code] << std::endl;
}

void Config::_parseClientMaxBodySize(std::map<const std::string, std::string>& config_info) {
	std::string body_size = config_info["client_max_body_size"];
	cut_till_collon(body_size);
	if (body_size[body_size.size() - 1] != 'M' && body_size[body_size.size() - 1] != 'm')
		exit_with::e_perror("config error: client_max_body_size");
	_server[_server.size() - 1]._client_max_body_size = body_size;
}

void Config::_parseAllowedMethods(std::map<const std::string, std::string>& config_info) {
	std::string methods = config_info["allowed_methods"];
	size_t		i = 0;
	size_t		j = 0;

	cut_till_collon(methods);
	_server[_server.size() - 1]._methods.push_back("");
	while (i < methods.length()) {
		if (methods[i] == '\t' || methods[i] == ' ') {
			_server[_server.size() - 1]._methods.push_back("");
			if (strncmp(_server[_server.size() - 1]._methods[j].c_str(), "GET", _server[_server.size() - 1]._methods[j].length()) != 0 && strncmp(_server[_server.size() - 1]._methods[j].c_str(), "POST", _server[_server.size() - 1]._methods[j].length()) != 0 && strncmp(_server[_server.size() - 1]._methods[j].c_str(), "SET", _server[_server.size() - 1]._methods[j].length()) != 0)
				exit_with::e_perror("config error: methods");
			j++;
		} else
			_server[_server.size() - 1]._methods[j].push_back(methods[i]);
		i++;
	}
	if (strncmp(_server[_server.size() - 1]._methods[j].c_str(), "GET", _server[_server.size() - 1]._methods[j].length()) != 0 && strncmp(_server[_server.size() - 1]._methods[j].c_str(), "POST", _server[_server.size() - 1]._methods[j].length()) != 0 && strncmp(_server[_server.size() - 1]._methods[j].c_str(), "SET", _server[_server.size() - 1]._methods[j].length()) != 0)
		exit_with::e_perror("config error: methods");
	_server[_server.size() - 1]._number_methods = j + 1;
}

void Config::_parseRoot(std::map<const std::string, std::string>& config_info) {
	std::string root = config_info["root"];

	cut_till_collon(root);
	_server[_server.size() - 1]._root = root;
}

bool IsPathExist(const std::string& s) {
	struct stat buffer;
	return (stat(s.c_str(), &buffer) == 0);
}

void Config::_parseLocation(std::map<const std::string, std::string>& config_info) {
	std::string location = config_info["location"];
	_location_check = true;
	cut_till_bracket(location);
	if (location[0] == '/')
		location.insert(0, ".");
	else
		location.insert(0, "./");
	if (!IsPathExist(location))
		exit_with::e_perror("config error: location");
	_server[_server.size() - 1]._location.push_back(Location());
	_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1] = (Location());
	_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._path = location;
}

void Config::_parseIndex(std::map<const std::string, std::string>& config_info) {
	std::string	  index = config_info["index"];
	std::ifstream try_file;
	std::string	  path_to_file;

	cut_till_collon(index);
	path_to_file = _server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._path + "/" + index;
	try_file.open(path_to_file);
	if (!try_file.is_open())
		exit_with::e_perror("config error: index");
	try_file.close();
}

void Config::_parseAutoIndex(std::map<const std::string, std::string>& config_info) {
	std::string autoIndex = config_info["autoindex"];

	cut_till_collon(autoIndex);
	if (autoIndex.compare("on") != 0 && autoIndex.compare("off") != 0)
		exit_with::e_perror("config error: autoindex");
	if (_location_check == true)
		_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._autoIndex = autoIndex;
	else
		_server[_server.size() - 1]._autoIndex = autoIndex;
}

void Config::_parseCgi(std::map<const std::string, std::string>& config_info) {
	std::string cgi = config_info["cgi"];

	cut_till_collon(cgi);
	size_t		space = cgi.find_first_of(" \t");
	size_t		not_space = cgi.find_first_not_of(" \t", space);
	std::string path = cgi.substr(not_space, cgi.size());
	if (_location_check == true) {
		_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._cgiPath.first = cgi.substr(0, space);
		_server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._cgiPath.second = path;
	} else
		exit_with::e_perror("config error: cgi");
	// std::cout << _server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._cgiPath.first << " | " << _server[_server.size() - 1]._location[_server[_server.size() - 1]._location.size() - 1]._cgiPath.second << std::endl;
}
