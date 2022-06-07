#pragma once
#include "main.hpp"
#include "request.hpp"
#include "router.hpp"
class Route;

namespace Response {

std::string header_template(uint32_t code);

//
std::string text(uint32_t code, const std::string& message) __attribute__((warn_unused_result));
void		cgi(const Request& request, const std::string& path);
Route		file(const std::string& path, uint32_t code) __attribute__((warn_unused_result));
std::string error(const std::string& path, uint32_t code) __attribute__((warn_unused_result));
std::string redirect(uint32_t code, const std::string& message, const std::string& redir_location) __attribute__((warn_unused_result));

} // namespace Response
