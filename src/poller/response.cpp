#include "response.hpp"
#include "constants.hpp"

Response::Response(fd_t fd) : _fd(fd) {
}

Response::~Response() {
}

// TODO: optimize
void Response::send_response(uint32_t code, const std::string& message) {

	std::string response = "HTTP/1.1 ";
	response += std_ft::to_string(code);
	response += " ";
	response += g_constants.to_response_string(code);
	response += "\r\n\r\n";
	response += message;
	write(_fd, response.c_str(), response.length()); // TODO: error handling
}

void Response::send_cgi(const std::string& path, const std::string& path_info, const std::string& query_string) {
	pid_t pid = fork();
	if (pid == 0) { // child
		dup2(_fd, STDERR_FILENO);
		dup2(_fd, STDOUT_FILENO);

		std::vector<const std::string> env;
		if (path_info.length())
			env.push_back("PATH_INFO=" + path_info);
		if (query_string.length())
			env.push_back("QUERY_STRING=" + query_string);

		std::vector<const char*> envp;
		for (size_t i = 0; i < env.size(); i++)
			envp.push_back(env[i].c_str());
		envp.push_back(NULL);

		if (execve(path.c_str(), NULL, NULL))
			exit_with::e_errno("Could not start cgi script");
		close(_fd);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		exit(0);
	} else {
		close(_fd);
	}
}
