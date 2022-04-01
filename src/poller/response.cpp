#include "response.hpp"
#include "constants.hpp"
#include "file_system.hpp"

Response::Response(fd_t fd) : _fd(fd) {
}

Response::~Response() {
	close(_fd);
}

// returns minimum starting paramters of response header
static std::string header_template(uint32_t code) {
	std::string header = "HTTP/1.1 ";
	header += std_ft::to_string(code);
	header += " ";
	header += g_constants.to_response_string(code);
	return header;
}

// TODO: optimize
void Response::text(uint32_t code, const std::string& message) {
	std::string response = header_template(code);
	response += "\r\n\r\n";
	response += message;
	write(_fd, response.c_str(), response.length()); // TODO: error handling
}

void Response::cgi(const std::string& path, const std::string& path_info, const std::string& query_string) {
	pid_t pid = fork();
	if (pid == 0) { // child
		static std::string start_header = header_template(200);
		write(_fd, start_header.data(), start_header.size());

		dup2(_fd, STDERR_FILENO);
		dup2(_fd, STDOUT_FILENO);

		std::vector<const std::string> env;
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

		std::vector<const char*> envp = vector_to_c_array(env);
		if (execve(path.c_str(), NULL, (char* const*)envp.data()))
			exit_with::e_errno("Could not start cgi script");

		close(_fd);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		exit(0);
	}
}

void Response::file(const std::string& path) {
	std::string header = header_template(200);
	std::string file = fs::read_file(path);
	header += "Content-length: " + std_ft::to_string(file.size());
	header += "\r\n\r\n";
	write(_fd, header.data(), header.size());
	// TODO wait for 100-Continue?
	write(_fd, file.data(), file.size());
}
