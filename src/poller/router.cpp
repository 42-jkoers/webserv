#include "router.hpp"
#include "file_system.hpp"

Router::~Router() {
}

Router::Router() {
	return;
}

bool Router::_has_server_name(std::vector<Config::Server>::iterator server, std::string server_name) {
	for (std::vector<std::string>::iterator it = server->server_name.begin(); it != server->server_name.end(); ++it) {
		if (*it == server_name) {
			return 1;
		}
	}
	return 0;
}

/*
first server with correct port is the default server
if correct server name is found -> this server is returned
*/
const Config::Server& Router::find_server(uint16_t port, const std::string& hostname) {
	int	   found = 0;
	size_t i = 0;
	size_t server_index;

	// TODO: cpp11 iterators
	for (std::vector<Config::Server>::iterator it = g_config._servers.begin(); it != g_config._servers.end(); ++it) { // loop over servers
		for (std::vector<uint16_t>::iterator it2 = it->port.begin(); it2 != it->port.end(); ++it2) {				  // loop over ports
			if (*it2 == port) {
				if (_has_server_name(it, hostname)) {
					return *it;
				} else if (found == 0) {
					server_index = i;
				}
				found = 1;
			}
		}
		i++;
	}
	return g_config._servers[server_index];
}

/*
first location is the default location
location is prefix of URI
location with most specific prefix is selected
*/
const Config::Location& Router::find_location(const std::string& path, const Config::Server& server) {
	int			i = 0;
	int			location_index = 0;
	std::string last_path = "";

	location_index = 0;
	for (std::vector<Config::Location>::const_iterator it = server.location.begin(); it != server.location.end(); it++) {
		size_t found = path.find(it->path);
		if (found != std::string::npos && found == 0) {
			if (last_path.size() < it->path.size()) {
				location_index = i;
				last_path = it->path;
			}
		}
		i++;
	}
	return server.location[location_index];
}

// TODO: Check allowed methods -> method not allowed
bool Router::_method_allowed(const Request& request, const Config::Location& location) {
	if (location.methods.empty())
		return true;
	for (std::vector<std::string>::const_iterator it = location.methods.begin(); it != location.methods.end(); it++) {
		if (to_upper(*it) == request.method)
			return true;
	}
	return false;
}

/*
Routes request to the right server
How the server processes request:
1. find the right server
2. find the right location
3. check allowed methods -> error if not allowed
4. add root to path
5. find URI matches file-> if not 404
6. if it is a directory -> defaultfile?
*/
void Router::route(Client& client) { // t
	const Request&			request = client.request;
	const Config::Server&	server = request.associated_server();
	const Config::Location& location = request.associated_location();

	if (!_method_allowed(request, location)) {
		Response::text(request, 405, "");
		return;
	}
	if (request.path.compare("favicon.ico") == 0) { // our server does not provide a favicon
		Response::text(request, 404, "");
		return;
	}

	if (location.cgi_path.first.size()) {
		// parse http://example.com/cgi-bin/printenv.pl/with/additional/path?and=a&query=string to:
		// request.uri     : "/cgi-bin/printenv.pl/with/additional/path"
		// exectutable_path: "/cgi-bin/printenv.pl"
		// path_info	   : "/with/additional/path"
		// request.query   : "and=a&query=string"

		std::string				 executable_path = server.root + "/" + location.path;
		std::vector<std::string> blocks = ft_split(request.uri, "/");
		bool					 found = false;
		while (blocks.size() && fs::path_exists(executable_path + "/" + blocks[0])) {
			executable_path += "/" + blocks[0];
			blocks.erase(blocks.begin());
			found = true;
		}
		if (!found)
			return Response::text(request, 404, ""); // TODO: error page
		std::string path_info;
		for (size_t i = 0; i < blocks.size(); i++)
			path_info += "/" + blocks[i];
		Response::cgi(request, executable_path, path_info, request.query); // todo should we read the cgi executable from the config?
	}
	if (request.uri.find("/cgi/index.sh") != std::string::npos)
		Response::cgi(request, "./cgi/index.sh", "", request.query);
	else if (request.uri.find("/form") != std::string::npos)
		Response::file(request, "./html/form.html");
	else if (request.field_contains("user-agent", "curl"))
		Response::text(request, 200, "Hello curl\n");
	else
		Response::file(request, "./html/upload.html");
}
