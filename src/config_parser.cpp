#include "../include/config_parser.hpp"
#include "main.hpp"
#include <sstream>

// true on success
bool parse_int(unsigned int& output, const std::string& str) {
	char			  c;
	std::stringstream ss(str);
	ss >> output;
	return !(ss.fail() || ss.get(c));
}

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
	config_info[option] = &line[pos_not];
}

void safe_info(std::string line, std::map<const std::string, std::string>& config_info, config& config) {
	std::vector<std::string> options;
	options.push_back("server_name");
	options.push_back("listen");
	options.push_back("error_page");
	options.push_back("client_max_body_size");
	options.push_back("allow_methods");

	for (size_t i = 0; i < options.size(); i++) {
		if (line.find(options[i]) != std::string::npos) {
			tokenizer(options[i], config_info, line);
		}
	}
	unsigned int	port;
	size_t pos = config_info["listen"].find_first_of(":");
	parse_int(port, &config_info["listen"][pos + 1]);
	config.set_port(port);
}

int config_parser(config& config) {
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
	return 0;
}
