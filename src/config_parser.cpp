#include "../include/config_parser.hpp"
#include "main.hpp"
#include <sstream>
config::config() {
}

config::~config() {
}

void	config::set_port(unsigned int set)	{
	this->port = set;
}

unsigned int	config::get_port() {
	return (this->port);
}

void tokenizer(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string delimiters = "\t ";

	size_t		pos_not = line.find_first_not_of(delimiters);
	size_t		pos_first = line.find_first_of(delimiters, pos_not);
	pos_not = line.find_first_not_of(delimiters, pos_first);
	// std::cout << "FOUND LINE = " << &line[pos_not] << "/" << std::endl;
	config_info[option] = &line[pos_not];
	// std::cout << "config_info = " << config_info[option] << std::endl;
}

// true on success
bool parse_int(unsigned int& output, const std::string& str) {
	char			  c;
	std::stringstream ss(str);
	ss >> output;
	return !(ss.fail() || ss.get(c));
}

void safe_info(std::string line, std::map<const std::string, std::string>& config_info, config &config) {
	std::vector<std::string> options;
	options.push_back("server_name");
	options.push_back("listen");
	options.push_back("error_page");
	options.push_back("client_max_body_size");
	options.push_back("allow_methods");

	for (size_t i = 0; i < options.size(); i++) {
		if (line.find(options[i]) != std::string::npos) {
			tokenizer(options[i], config_info, line);
			// std::cout << line << std::endl;
		}
	}
	unsigned int	port;
	size_t pos = config_info["listen"].find_first_of(":");
	config.set_port(parse_int(port, &config_info["listen"][pos + 1]));
	// std::cout << "port = " << port << std::endl;
}

int config_parser(config &config) {
	std::ifstream					   config_file;
	std::string						   buffer;
	std::map<const std::string, std::string> config_info;

	config_file.open("config_file.conf");
	if (!config_file.is_open())
		exit_with::e_perror("Cannot open config file");
	while (getline(config_file, buffer)) {
		safe_info(buffer, config_info, config);
	}
	config_file.close();
	// std::cout << config.get_port() << std::endl;

	return 0;
}
