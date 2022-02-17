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

Config::Config(int argc, char** argv) {
	_config_parser(argv);
	(void)argc;
}

Config::~Config() {
}

void tokenizer(std::string option, std::map<const std::string, std::string>& config_info, std::string line) {
	std::string delimiters = "\t ";

	size_t		pos_not = line.find_first_not_of(delimiters);
	size_t		pos_first = line.find_first_of(delimiters, pos_not);
	pos_not = line.find_first_not_of(delimiters, pos_first);
	config_info[option] = &line[pos_not];
}

//Don't allow invalid lines in configuration file
void Config::safe_info(std::string line, std::map<const std::string, std::string>& config_info, std::vector<std::string>& options) {
	typedef void (Config::*Jump_table)(std::string, std::map<const std::string, std::string>&, std::string);
	const static Jump_table jump_table[] = {
		&Config::_parseServerName,
		&Config::_parseListen,
		&Config::_parseErrorPage,
		&Config::_parseClientMaxBodySize,
		&Config::_parseAllowedMethods,
		&Config::_parseRoot};

	for (size_t i = 0; i < options.size(); i++) {
		if (line.find(options[i]) != std::string::npos) {
			tokenizer(options[i], config_info, line);
			(this->*jump_table[i])(options[i], config_info, line);
		}
	}
}

void Config::_config_parser(char** argv) {
	std::ifstream							 config_file;
	std::string								 buffer;
	std::map<const std::string, std::string> config_info;
	std::vector<std::string>				 options;

	options.push_back("server_name");
	options.push_back("listen");
	options.push_back("error_page");
	options.push_back("client_max_body_size");
	options.push_back("allowed_methods");
	options.push_back("root");
	config_file.open(argv[1]);
	if (!config_file.is_open())
		exit_with::e_perror("Cannot open config file");
	while (getline(config_file, buffer)) {
		safe_info(buffer, config_info, options);
	}
	config_file.close();
}
