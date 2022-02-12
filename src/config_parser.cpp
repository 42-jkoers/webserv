#include "../include/config_parser.hpp"
#include "main.hpp"

// config_parser::config_parser() {
// }

// config_parser::~config_parser() {
// }

// std::string find_right_port(std::string line) {
// }

void safe_info(std::string line, std::map<std::string, std::string>& config_info) {
	std::vector<std::string> options;
	options.push_back("port");
	options.push_back("server_name");
	options.push_back("listen");
	options.push_back("error_page");
	options.push_back("client_max_body_size");
	options.push_back("allow_methods");

	for (unsigned long i = 0; i < options.size(); i++)
	{
		if (line.find(options[i]) != std::string::npos)
			std::cout << line << std::endl;
	}
	(void)config_info;
}

int config_parser(void) {
	std::ifstream					   config_file;
	std::string						   buffer;
	std::map<std::string, std::string> config_info;

	config_file.open("config_file.conf");
	if (!config_file.is_open())
		return 1;
	while (getline(config_file, buffer)) {
		safe_info(buffer, config_info);
		// std::cout << buffer << "\n";
	}
	// while (config_file)
	// {
	// 	std::string line;
	// 	getline(config_file, line, ' ');
	// 	std::string left_over;
	// 	config_file >> left_over;
	// 	config_file.get();
	// 	std::cout << "'" << line << "'" << "--- '" << left_over << "'" << std::endl;
	// }
	config_file.close();
	return 0;
}
