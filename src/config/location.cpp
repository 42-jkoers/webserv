#include "main.hpp"

std::string Location::getLocation() const {
	return _location;
}

Location::Location(const std::string location): _location(location){

}

Location::~Location(){

}
