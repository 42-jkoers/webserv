#include "header_field.hpp"

// Header_field::Header_field() {
// 	_size = 0;
// }

Header_field::Header_field(std::string name, std::string raw_value) : _name(name), _raw_value(raw_value) {
	_size_values = 0;
	std::cout << "Header_field constructor called" << std::endl;
}

Header_field::~Header_field() {
}

void Header_field::add_value(std::string value) {
	_size_values++;
	_values.push_back(value);
}

std::ostream&
operator<<(std::ostream& output, Header_field const& rhs) {
	output << "Header field [" << rhs._name << "]"
		   << " with size [" << rhs._size_values << "]";
	return output;
}
