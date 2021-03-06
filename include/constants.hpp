#pragma once
#include "main.hpp"

class Constants {
  public:
	Constants();
	const std::string& to_response_string(uint32_t code) const;
	bool			   is_valid_method(const std::string& method) const;
	std::string		   webserver_name() const;

  private:
	std::map<uint32_t, std::string> _response_str;
	std::vector<std::string>		_methods;

	// disabled
	Constants(const Constants& cp);
	Constants& operator=(const Constants& cp);
};

extern const Constants g_constants;
