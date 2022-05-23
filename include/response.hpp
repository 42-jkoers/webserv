#pragma once
#include "main.hpp"
#include "request.hpp"

namespace Response {

void text(const Request& request, uint32_t code, const std::string& message);
void cgi(const Request& request, const std::string& path);
void file(const Request& request, const std::string& path, uint32_t code);
void error(const Request& request, const std::string& path, uint32_t code);
void redirect(const Request& request, uint32_t code, const std::string& message, const std::string& redir_location);

} // namespace Response
