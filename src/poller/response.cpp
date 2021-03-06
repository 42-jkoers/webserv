#include "response.hpp"
#include "constants.hpp"
#include "file_system.hpp"
#include "router.hpp"
#include <fcntl.h>

namespace Response {

// returns minimum starting paramters of response header
std::string header_template(uint32_t code) {
	std::string header = "HTTP/1.1 ";
	header += std_ft::to_string(code);
	header += " ";
	header += g_constants.to_response_string(code);
	header += "\n";
	header += "Server: " + g_constants.webserver_name() + "\n";
	return header;
}

// default error pages generator
std::string default_error(const std::string& path, uint32_t code) {
	std::string body;

	body += "<html>\n";
	body += "<head><title>";
	body += std_ft::to_string(code);
	body += " ";
	body += g_constants.to_response_string(code);
	body += "</title></head>\n";
	body += "<body>\n";

	body += "<center><h1>";
	body += std_ft::to_string(code);
	body += " ";
	body += g_constants.to_response_string(code);
	body += "</h1></center>\n";

	body += "<hr><center>" + g_constants.webserver_name() + "</center>\n";

	if (!path.empty()) {
		body += "<center>path: ";
		body += path;
		body += "</center>";
		body += "\n";
	}

	body += "</body>";
	body += "\n";
	body += "</html>";
	return body;
}

std::string error(const std::string& path, uint32_t code) {
	std::string header = header_template(code);
	std::string body = default_error(path, code);

	header += "Content-length: " + std_ft::to_string(body.size());
	header += "\r\n\r\n";

	return header + body;
}

std::string redirect(uint32_t code, const std::string& message, const std::string& redir_location) {
	std::string header = header_template(code);
	std::string body;

	if (!message.empty())
		body += message;
	if (!redir_location.empty()) {
		header += "Location: " + redir_location;
		header += "\n";
		header += "Content-Type: text/html\n";
		body += default_error("", code);
		body += "\n";
	}
	if (message.empty() && redir_location.empty()) {
		header += "Content-Type: text/html\n";
		body += default_error("", code);
		body += "\n";
	}
	header += "Content-length: " + std_ft::to_string(body.size());
	header += "\r\n\r\n";

	return header + body;
}

std::string redirect(const Request& request, uint32_t code, const std::string& message) {
	std::string response = header_template(code);

	if (message.empty()) {
		response += "Location: ";
		response += request.associated_location().redirect.text;
		response += "\n";
	}
	response += "\r\n";
	if (!message.empty()) {
		response += message;
	}
	return response;
}

std::string text(uint32_t code, const std::string& message) {
	std::string response = header_template(code);
	response += "\r\n";
	response += message;
	return response;
}

std::vector<std::string> get_cgi_env(const Request& request, const std::string& path) {
	std::vector<std::string> env;
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REMOTE_ADDR=" + request.ip);
	env.push_back("REQUEST_METHOD=" + request.method);
	env.push_back("SCRIPT_NAME=" + path);
	env.push_back("SERVER_NAME=" + request.associated_server_name(request.associated_server().server_names));
	env.push_back("SERVER_PORT=" + std_ft::to_string(request.port));
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("PATH_INFO=" + path::join("/" + request.associated_location().root, request.path));
	env.push_back("SERVER_SOFTWARE=" + g_constants.webserver_name());
	env.push_back("QUERY_STRING=" + request.query);
	if (request.body.size())
		env.push_back("CONTENT_LENGTH=" + std::to_string(request.body.size()));

	return env;
}

void cgi(const Request& request, const std::string& path) {
	int pipe_in[2];
	if (pipe(pipe_in))
		exit_with::e_errno("pipe() failed");

	pid_t pid = fork();
	if (pid == 0) { // child
		static std::string start_header = header_template(200);
		write(request.fd, start_header.data(), start_header.size());

		dup2(pipe_in[0], STDIN_FILENO);
		dup2(request.fd, STDOUT_FILENO);
		dup2(request.fd, STDERR_FILENO);

		const std::vector<std::string> envp = get_cgi_env(request, path);
		const std::vector<std::string> argv = {
			path::absolute(path::join(request.associated_location().root, request.path)),
			path::absolute(path::join(request.associated_location().root, request.path))};
		if (cpp::execve(path, argv, envp)) {
		}

		close(pipe_in[0]);
		close(pipe_in[1]);
		close(request.fd);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		exit(0);
	} //
	else {
		if (request.body.size())
			write(pipe_in[1], request.body.data(), request.body.size());
		close(pipe_in[0]);
		close(pipe_in[1]);
	}
}

Route file(const std::string& path, uint32_t code) {
	std::string header = header_template(code);
	ssize_t		size = fs::file_size(path);
	assert(size >= 0);
	header += "Content-length: " + std_ft::to_string(size);
	header += "\r\n\r\n";

	fd_t fd = open(path.c_str(), O_RDONLY);
	assert(fd != -1);

	return Route(header, fd, size);
}

} // namespace Response
