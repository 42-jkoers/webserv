#include <config_parser.hpp>
#include <main.hpp>

template <typename T>
T get_config_info(std::string info) {
	std::vector<std::string> options;

	options.push_back("server_name");
	options.push_back("listen");
	options.push_back("error_page");
	options.push_back("client_max_body_size");
	options.push_back("allowed_methods");
	options.push_back("root");
	options.push_back("location");
	options.push_back("autoindex");
	options.push_back("index");
	for (size_t i = 0; i < options.size(); i++)
	{
		if (options[i] == info){
			
		}
	}
}
