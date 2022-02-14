#pragma once

#include "main.hpp"
#include <fstream>
#include <iostream>
#include <map>

class config
{
public:
	config(/* args */);
	~config();
	void			set_port(unsigned int set);
	unsigned int	get_port();

private:
	unsigned int port;

};

int config_parser(config &config);

// typedef struct config {
// 	std::map<int, std::string> config;
// };
