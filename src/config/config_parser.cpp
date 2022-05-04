#include "config_parser.hpp"
#include "main.hpp"
#include <sstream>

Config::Config(const std::string& config_file_path) {
	_config_parser(config_file_path);
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
		&Config::_parse_root,
		&Config::_parse_location,
		&Config::_parse_auto_index,
		&Config::_parse_index,
		&Config::_parse_cgi,
		&Config::_parse_return};
	for (size_t i = 0; i < options.size(); i++) {
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos) {
			servers.push_back(Server());
			_inside_server = true;
			return;
		}
		if (line.find(options[i]) != std::string::npos) {
			tokenizer(options[i], config_info, line);
			(this->*jump_table[i])(config_info);
			return;
		} else if (line.find_first_not_of("\t ") == std::string::npos)
			return;
		else if (line.find_first_of("}") != std::string::npos) {
			if (_inside_location > 0) {
				_inside_location--;
				_safe_new_path_location = true;
			} else if (_inside_server)
				_inside_server = false;
			else
				exit_with::message("config: syntx");
			return;
		}
	}
	std::cout << line << std::endl;
	exit_with::message("config error: invalid line");
}

// TODO: work with comments
void Config::_config_parser(const std::string& config_file_path) {
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
	options.push_back("return");
	_inside_location = 0;
	_safe_new_path_location = false;
	config_file.open(config_file_path);
	if (!config_file.is_open())
		exit_with::e_perror("Cannot open config file: \"" + config_file_path + "\"");
	while (getline(config_file, buffer)) {
		if (buffer.find_first_of("#") != std::string::npos)
			buffer = buffer.substr(0, buffer.find_first_of("#"));
		if (buffer.length() == 0)
			continue;
		_safe_info(buffer, config_info, options);
	}
	config_file.close();
}

std::ostream& operator<<(std::ostream& stream, Config const& config) {
	for (size_t server = 0; server < config.servers.size(); server++) {
		stream << "SERVER INFORMATION FROM SERVER " << server << std::endl;
		for (size_t j = 0; j < config.servers[server].ports.size(); j++) {
			stream << "PORTS = " << config.servers[server].ports[j] << std::endl;
		}
		for (size_t k = 0; k < config.servers[server].ports.size(); k++) {
			stream << "IP = " << config.servers[server].ips[k] << std::endl;
		}
		for (size_t j = 0; j < config.servers[server].server_names.size(); j++) {
			std::cout << "SERVER NAME = " << config.servers[server].server_names[j] << std::endl;
		}

		stream << "ROOT = " << config.servers[server].root << std::endl;
		stream << "CLIENT_MAX_BODY_SIZE = " << config.servers[server].client_max_body_size << std::endl;
		for (std::map<size_t, std::string>::const_iterator it = config.servers[server].error_pages.begin(); it != config.servers[server].error_pages.end(); it++) {
			stream << "ERROR_PAGES = " << it->first << " | " << it->second << std::endl;
		}

		for (size_t location = 0; location < config.servers[server].locations.size(); location++) {
			stream << "\nALL LOCATION INFO " << location << std::endl;
			stream << "PATH = " << config.servers[server].locations[location].path << std::endl;
			for (size_t m = 0; m < config.servers[server].locations[location].allowed_methods.size(); m++) {
				stream << "METHODS = " << config.servers[server].locations[location].allowed_methods[m] << std::endl;
			}
			stream << "AUTOINDEX = " << config.servers[server].locations[location].auto_index << std::endl;
			stream << "CGI = " << config.servers[server].locations[location].cgi_path.first << " | " << config.servers[server].locations[location].cgi_path.second << std::endl;
			for (size_t j = 0; j < config.servers[server].locations[location].ports.size(); j++) {
				stream << "PORTS = " << config.servers[server].locations[location].ports[j] << std::endl;
			}
			for (std::map<size_t, std::string>::const_iterator it = config.servers[server].locations[location].error_pages.begin(); it != config.servers[server].locations[location].error_pages.end(); it++) {
				stream << "ERROR_PAGES = " << it->first << " | " << it->second << std::endl;
			}
			stream << "METHODS = ";
			for (size_t i = 0; i < config.servers[server].locations[location].allowed_methods.size(); i++)
				stream << config.servers[server].locations[location].allowed_methods[i] << " | ";
			stream << std::ends;
		}
	}
	return stream;
}
