#pragma once
#include "main.hpp"

class Constants {
  public:
	Constants();
	std::string to_content_type(const std::string& filename);
	std::string to_response_string(uint32_t code);

  private:
	std::map<std::string, std::string> _content_type;
	std::map<uint32_t, std::string>	   _response_str;

	// disabled
	Constants(const Constants& cp);
	Constants& operator=(const Constants& cp);
};

extern Constants g_constants;
