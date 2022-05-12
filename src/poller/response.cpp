#include "response.hpp"
#include "constants.hpp"
#include "file_system.hpp"

// returns minimum starting paramters of response header
static std::string header_template(uint32_t code) {
	std::string header = "HTTP/1.1 ";
	header += std_ft::to_string(code);
	header += " ";
	header += g_constants.to_response_string(code);
	header += "\n";
	return header;
}

namespace Response {

void error(const Request& request, const std::string& path, uint32_t code) {
	std::string response = header_template(code);
	response += "\r\n";

	response += "<html>\n";
	response += "<head><title>";
	response += std_ft::to_string(code);
	response += " ";
	response += g_constants.to_response_string(code);
	response += "</title></head>\n";
	response += "<body>\n";

	response += "<center><h1>";
	response += std_ft::to_string(code);
	response += " ";
	response += g_constants.to_response_string(code);
	response += "</h1></center>\n";

	response += "<hr><center>webbysetservvvvvv/1.0\n";

	response += "<center>path: ";
	response += path;
	response += "\n";

	response += "</center>";
	response += "</body>";
	response += "</html>";
	write(request.fd, response.c_str(), response.length());
}

// TODO: optimize
void text(const Request& request, uint32_t code, const std::string& message) {
	std::string response = header_template(code);
	response += "\r\n";
	response += message;
	write(request.fd, response.c_str(), response.length()); // TODO: error handling
}

std::vector<std::string> get_cgi_env(const Request& request, const std::string& path, const std::string& path_info, const std::string& query_string) {
	std::vector<std::string> env;
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REMOTE_ADDR=" + request.ip);
	env.push_back("REQUEST_METHOD=" + request.method);
	env.push_back("SCRIPT_NAME=" + path);
	env.push_back("SERVER_NAME=" + request.associated_server_name(request.associated_server().server_names));
	env.push_back("SERVER_PORT=" + std_ft::to_string(request.port));
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=webserv/42");
	env.push_back("PATH_INFO=" + path_info);
	// if (request.method != "POST") // TODO: should this be here?
	env.push_back("QUERY_STRING=" + query_string);
	return env;
}

void cgi(const Request& request, const std::string& path, const std::string& path_info, const std::string& query_string) {
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

		const std::vector<std::string> envp = get_cgi_env(request, path, path_info, query_string);
		std::cerr << " MAKING MY WAY DOWNTOWN" << std::endl;
		if (cpp::execve(path, std::vector<std::string>(), envp))
			Response::text(request, 500, "Could not start cgi script \"" + request.path + "\"");

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
		close(request.fd);
	}
}

void file(const Request& request, const std::string& path) {
	std::string header = header_template(200);
	std::string file = fs::read_file(path);
	header += "Content-length: " + std_ft::to_string(file.size());
	header += "\r\n\r\n";
	write(request.fd, header.data(), header.size());
	// TODO wait for 100-Continue?
	write(request.fd, file.data(), file.size());
}

} // namespace Response
