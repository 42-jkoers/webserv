#pragma once
#include "main.hpp"
#include "request.hpp"

namespace Response {

void text(const Request& request, uint32_t code, const std::string& message);
void cgi(const Request& request, const std::string& path, const std::string& path_info, const std::string& query_string);
void file(const Request& request, const std::string& path);
void error(const Request& request, const std::string& path, uint32_t code);

} // namespace Response
