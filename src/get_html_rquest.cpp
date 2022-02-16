#include "config_parser.hpp"
#include "main.hpp"

std::string	get_html_request(config& config) {
	std::ifstream html_file;
	std::string line;
	std::string html;
	std::cout << config.get_root() << std::endl;
	html_file.open(config.get_root().c_str());
	while(getline(html_file, line)) {
		html+= line;
	}
	html_file.close();
	std::cout << html << std::endl;
	return html;
}
