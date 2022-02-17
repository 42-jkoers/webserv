
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
				exit_with::e_perror("config error: listen not right");
			if (listen[i] == '.') {
				check_ip = 0;
				if (check_dots > 2) // check if its a real ip adress
					exit_with::e_perror("config error: listen dots");
				check_dots++;
			}
			if (check_ip > 2 && check_dots != 3) // check if its only 3 numbers long
				exit_with::e_perror("config error: listen ip");
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
	(void)option;
	(void)config_info;
	(void)line;
}

void Config::_parseClientMaxBodySize(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	(void)option;
	(void)config_info;
	(void)line;
}

void Config::_parseAllowedMethods(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	(void)option;
	(void)config_info;
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
