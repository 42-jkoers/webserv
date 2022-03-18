#pragma once
#include "main.hpp"

class Header_field {
  public:
	// Header_field();
	Header_field(std::string name, std::string raw_value);
	~Header_field();
	void add_value(std::string value);
	// read-only variables
	size_t					 size_values;
	std::vector<std::string> values;
	std::string				 name;
	std::string				 raw_value; // TODO: make private after parsing values

  private:
	// disabled
	// Header_field(const Header_field& cp); // TODO
	// Header_field& operator=(const Header_field& cp);
};

std::ostream& operator<<(std::ostream& output, Header_field const& rhs);
