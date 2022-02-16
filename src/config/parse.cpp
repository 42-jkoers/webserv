
#include "main.hpp"
#include "config_parser.hpp"
void parseServerName(std::string option, std::map<const std::string, std::string>& config_info, std::string line, config& config){
	(void)option;
	(void)config_info;
	(void)line;
	(void)config;
}
void parseListen(std::string option, std::map<const std::string, std::string>& config_info, std::string line, config& config){
	std::string listen = config_info["listen"];
	size_t i = 0;
	size_t check_dots = 0;
	size_t check_ip = 0;
	size_t find_collon;
	size_t end;

	find_collon = listen.find_last_not_of("\t ");
	if (listen[find_collon] != ';')
		exit_with::e_perror("config error: listen");
	end = listen.find_last_not_of("\t ;");
	listen = listen.substr(0, end + 1); // getting rid of the ';' and whitespace
	if (strchr(listen.c_str(), ':')) {
		while (i < listen.size()) {
			if (listen[i] != '.' && !isdigit(listen[i]) && listen[i] != ':' && listen[i] != ';')
				exit_with::e_perror("config error: listen not right");
			if (listen[i] == '.') {
				check_ip = 0;
				if (check_dots > 2)
					exit_with::e_perror("config error: listen dots");
				check_dots++;
			}
			if (check_ip > 2 && check_dots != 3)
				exit_with::e_perror("config error: listen ip");
			if (isdigit(listen[i]))
				check_ip++;
			i++;
		}
		unsigned int	port;
		size_t pos = config_info["listen"].find_first_of(":");
		config.set_ip(listen.substr(0, pos));
		parse_int(port, &config_info["listen"][pos + 1]);
		config.set_port(port);
	}
	else{
		unsigned int port;
		if (parse_int(port, listen) == false)
			exit_with::e_perror("config error: listen");
		config.set_port(port);
	}
	(void)line;
	(void)option;
}
void parseErrorPage(std::string option, std::map<const std::string, std::string>& config_info, std::string line, config& config){
	(void)option;
	(void)config_info;
	(void)line;
	(void)config;
}
void parseClientMaxBodySize(std::string option, std::map<const std::string, std::string>& config_info, std::string line, config& config){
	(void)option;
	(void)config_info;
	(void)line;
	(void)config;
}
void parseAllowedMethods(std::string option, std::map<const std::string, std::string>& config_info, std::string line, config& config){
	(void)option;
	(void)config_info;
	(void)line;
	(void)config;
}
void parseRoot(std::string option, std::map<const std::string, std::string>& config_info, std::string line, config& config){
	
	(void)option;
	(void)config_info;
	(void)line;
	(void)config;
}
