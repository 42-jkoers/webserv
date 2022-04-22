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

// TODO: optimize
void text(const Request& request, uint32_t code, const std::string& message) {
	std::string response = header_template(code);
	response += "\r\n\r\n";
	response += message;
	write(request.fd, response.c_str(), response.length()); // TODO: error handling
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

		std::vector<std::string> env;
		env.push_back("GATEWAY_INTERFACE=CGI/1.1");	 // TODO: what value
		env.push_back("REMOTE_ADDR=127.0.0.1");		 // TODO: IP of the client
		env.push_back("REQUEST_METHOD=GET");		 // TODO: allow POST
		env.push_back("SCRIPT_NAME=" + path);		 //
		env.push_back("SERVER_NAME=127.0.0.1");		 // TODO: read from config
		env.push_back("SERVER_PORT=8080");			 // TODO: read from request
		env.push_back("SERVER_PROTOCOL=HTTP/1.1");	 //
		env.push_back("SERVER_SOFTWARE=webserv/42"); //
		env.push_back("PATH_INFO=" + path_info);
		env.push_back("QUERY_STRING=" + query_string);

		char* const				 args[] = {NULL};
		std::vector<const char*> envp = vector_to_c_array(env);
		if (execve(path.c_str(), args, (char* const*)envp.data()))
			exit_with::e_errno("Could not start cgi script"); // TODO pipe to parent?

		close(pipe_in[0]);
		close(pipe_in[1]);
		close(request.fd);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		exit(0);
	} //
	else {
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
