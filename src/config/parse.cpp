
#include "main.hpp"
#include "config_parser.hpp"
void parseServerName(std::string option, std::map<const std::string, std::string>& config_info, std::string line, config& config){
	(void)option;
	(void)config_info;
	(void)line;
	(void)config;
}
void parseListen(std::string option, std::map<const std::string, std::string>& config_info, std::string line, config& config){
	unsigned int	port;
	size_t pos = config_info["listen"].find_first_of(":");
	parse_int(port, &config_info["listen"][pos + 1]);
	config.set_port(port);
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
