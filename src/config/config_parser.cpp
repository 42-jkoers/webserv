#include "config_parser.hpp"
#include "main.hpp"
#include <sstream>

/*To do 
a way to safe more locations
saving multiple ports
*/

// true on success
bool parse_int(unsigned int& output, const std::string& str) {
	char			  c;
	std::stringstream ss(str);
	ss >> output;
	return !(ss.fail() || ss.get(c));
}

Config::Config(int argc, char** argv) {
	_config_parser(argc, argv);
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

// Don't allow invalid lines in configuration file
void Config::safe_info(std::string line, std::map<const std::string, std::string>& config_info, std::vector<std::string>& options) {
	typedef void (Config::*Jump_table)(std::string, std::map<const std::string, std::string>&, std::string);
	const static Jump_table jump_table[] = {
		&Config::_parseServerName,
		&Config::_parseListen,
		&Config::_parseErrorPage,
		&Config::_parseClientMaxBodySize,
		&Config::_parseAllowedMethods,
		&Config::_parseRoot,
		&Config::_parseLocation,
		&Config::_parseAutoIndex,
		&Config::_parseIndex,
		&Config::_parseServer};

	for (size_t i = 0; i < options.size(); i++) {
		if (line.find(options[i]) != std::string::npos) {
			// std::cout << options[i] << std::endl;
			// std::cout << line << i << std::endl;
			tokenizer(options[i], config_info, line);
			(this->*jump_table[i])(options[i], config_info, line);
			return;
		} else if (line.find_first_not_of("\t ") == std::string::npos)
			return;
		else if (line.find_first_of("}") != std::string::npos) {
			if (_location_check == true){
				_current_location++;
				_location_check = false;
			}
			return;
		}
	}
	exit_with::e_perror("config error: invalid line");
}

void Config::_config_parser(int argc, char** argv) {
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
	options.push_back("location");
	options.push_back("autoindex");
	options.push_back("index");
	options.push_back("server");
	_current_location = 0;
	_location_check = false;
	if (argc == 2)
		config_file.open(argv[1]);
	else
		config_file.open("default.conf");
	if (!config_file.is_open())
		exit_with::e_perror("Cannot open config file");
	while (getline(config_file, buffer)) {
		safe_info(buffer, config_info, options);
	}
	config_file.close();
}
