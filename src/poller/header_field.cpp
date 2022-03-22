#include "header_field.hpp"

Header_field::Header_field(std::string name, std::string raw_value) : name(name), raw_value(raw_value) {
	size_values = 0;
	if (name == "content-length")
		type = integer;
	else
		type = string;
	// std::cout << "Header_field constructor called" << std::endl;
}

Header_field::~Header_field() {
}

void Header_field::add_value(std::string value) {
	size_values++;
	values.push_back(value);
}

std::ostream&
operator<<(std::ostream& output, Header_field const& rhs) {
	output << "Header field [" << rhs.name << "]"
		   << " with size [" << rhs.size_values << "]";
	return output;
}
