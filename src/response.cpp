#include "config_parser.hpp"
#include "main.hpp"

std::string	get_index(Config& config) {
	std::ifstream html_file;
	std::string line;
	std::string html;
	html_file.open(config.get_root().c_str());
	while(getline(html_file, line)) {
		html+= line;
	}
	html_file.close();
	return html;
}
