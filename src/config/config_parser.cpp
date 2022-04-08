#include "config_parser.hpp"
#include "main.hpp"
#include <sstream>

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

void Config::_safe_info(std::string line, std::map<const std::string, std::string>& config_info, std::vector<std::string>& options) {
	typedef void (Config::*Jump_table)(std::map<const std::string, std::string>&);
	const static Jump_table jump_table[] = {
		&Config::_parse_server_name,
		&Config::_parse_listen,
		&Config::_parse_error_page,
		&Config::_parse_client_max_body_size,
		&Config::_parse_allowed_methods,
		&Config::_parseRoot,
		&Config::_parse_location,
		&Config::_parse_auto_index,
		&Config::_parse_index,
		&Config::_parse_cgi};
	for (size_t i = 0; i < options.size(); i++) {
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos) {
			_servers.push_back(Server());
			_server_check = false;
			return;
		}
		if (line.find(options[i]) != std::string::npos) {
			tokenizer(options[i], config_info, line);
			(this->*jump_table[i])(config_info);
			return;
		} else if (line.find_first_not_of("\t ") == std::string::npos)
			return;
		else if (line.find_first_of("}") != std::string::npos) {
			if (_location_check == true)
				_location_check = false;
			else if (_server_check == true)
				_server_check = false;
			return;
		}
	}
	exit_with::e_perror("config error: invalid line");
}

//TODO: work with comments
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
	options.push_back("cgi");
	if (argc == 2)
		config_file.open(argv[1]);
	else
		config_file.open("default.conf");
	if (!config_file.is_open())
		exit_with::e_perror("Cannot open config file");
	while (getline(config_file, buffer)) {
		_safe_info(buffer, config_info, options);
	}
	config_file.close();
}

std::ostream& operator<<(std::ostream& stream, Config const& config) {
	for (size_t server = 0; server < config._servers.size(); server++) {
		stream << "SERVER INFORMATION FROM SERVER " << server << std::endl;
		for (size_t j = 0; j < config._servers[server].port.size(); j++) {
			stream << "PORTS = " << config._servers[server].port[j] << std::endl;
		}
		for (size_t k = 0; k < config._servers[server].port.size(); k++) {
			stream << "IP = " << config._servers[server].ip[k] << std::endl;
		}
		for (size_t j = 0; j < config._servers[server].server_name.size(); j++){
				std::cout << "SERVER NAME = " << config._servers[server].server_name[j] << std::endl;
		}
		stream << "SEVERURL = " << config._servers[server].server_url << std::endl;
		stream << "ROOT = " << config._servers[server].auto_index << std::endl;
		stream << "CLIENT_MAX_BODY_SIZE = " << config._servers[server].client_max_body_size << std::endl;
		for (std::map<size_t, std::string>::const_iterator it = config._servers[server].error_pages.begin(); it != config._servers[server].error_pages.end(); it++) {
			stream << "ERROR_PAGES = " << it->first << " | " << it->second << std::endl;
		}
		for (size_t m = 0; m < config._servers[server].port.size(); m++) {
			stream << "METHODS = " << config._servers[server].methods[m] << std::endl;
		}
		for (size_t location = 0; location < config._servers[server].location.size(); location++) {
			stream << "ALL LOCATION INFO FROM LOCATION " << location << std::endl;
			stream << "PATH = " << config._servers[server].location[location]._path << std::endl;
			for (size_t m = 0; m < config._servers[server].location[location].methods.size(); m++) {
				stream << "METHODS = " << config._servers[server].location[location].methods[m] << std::endl;
			}
			stream << "AUTOINDEX = " << config._servers[server].location[location].auto_index << std::endl;
			stream << "DEFAULT = " << config._servers[server].location[location]._defaultfile << std::endl;
			stream << "CGI = " << config._servers[server].location[location].cgi_path.first << config._servers[server].location[location].cgi_path.second << std::endl;
			for (size_t j = 0; j < config._servers[server].location[location].port.size(); j++) {
				stream << "PORTS = " << config._servers[server].location[location].port[j] << std::endl;
			}
			for (size_t k = 0; k < config._servers[server].location[location].port.size(); k++) {
				stream << "IP = " << config._servers[server].location[location].ip[k] << std::endl;
			}
			for (std::map<size_t, std::string>::const_iterator it = config._servers[server].location[location].error_pages.begin(); it != config._servers[server].location[location].error_pages.end(); it++) {
				stream << "ERROR_PAGES = " << it->first << " | " << it->second << std::endl;
			}
		}
	}
	for (std::map<size_t, std::vector<size_t> >::const_iterator it = config._ports_servers.begin(); it != config._ports_servers.end(); it++) {
		for (size_t index = 0; index < it->second.size(); index++) {
			std::cout << "PORTS_SERVER = " << it->first << " | " << it->second[index] << std::endl;
		}
	}
	return stream;
}
