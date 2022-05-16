#include "router.hpp"
#include "file_system.hpp"

Router::~Router() {
}

Router::Router() {
	return;
}

bool Router::_has_server_name(std::vector<Config::Server>::iterator server, std::string server_name) {
	for (std::vector<std::string>::iterator it = server->server_names.begin(); it != server->server_names.end(); ++it) {
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
	for (std::vector<Config::Server>::iterator it = g_config.servers.begin(); it != g_config.servers.end(); ++it) { // loop over servers
		for (std::vector<uint16_t>::iterator it2 = it->ports.begin(); it2 != it->ports.end(); ++it2) {				// loop over ports
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
	return g_config.servers[server_index];
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
	for (std::vector<Config::Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++) {
		size_t found = path.find(it->path);
		if (found != std::string::npos && found == 0) {
			if (last_path.size() < it->path.size()) {
				location_index = i;
				last_path = it->path;
			}
		}
		i++;
	}
	return server.locations[location_index];
}

// TODO: Check allowed methods -> method not allowed
bool Router::_method_allowed(const Request& request, const Config::Location& location) {
	if (location.allowed_methods.empty())
		return true;
	for (std::vector<std::string>::const_iterator it = location.allowed_methods.begin(); it != location.allowed_methods.end(); it++) {
		if (to_upper(*it) == request.method)
			return true;
	}
	return false;
}

void respond_with_file_not_found(Request& request) {
	Response::text(request, 404, "Path \"" + request.path + "\" not found");
}

// If a request ends with a slash, NGINX treats it as a request for a directory and tries to find an index file in the directory
// search for index.html or index if specified
// if not found-> if autoindex on -> dir listing
// if not exist and not autoindex on -> 404

// returns the path of the file on disk
// returns "" when the path could not be found
std::string get_path_on_disk(const Request& request) {
	// The path where the server should start looking for files
	// eg:  request.path = "/cgi/test"
	//     location.path = "/cgi"
	//     location.root = "www/cgi"
	// Then mounted_path = "www/cgi/test"

	const Config::Location& location = request.associated_location();
	std::string				path = request.path.size() ? request.path : "/";

	if (path.at(path.size() - 1) == '/')
		path = path::join(path, location.indexes[0]); // TODO: should this be an array?
	path = path.substr(location.path.size());
	path = path::join(location.root, path);
	path = path::normalize(path);
	if (!fs::path_exists(path))
		return "";
	return path;
}

void rout_cgi(Request& request) {
	// parse http://example.com/cgi-bin/printenv.pl/with/additional/path?and=a&query=string to:
	// request.uri     : "/cgi-bin/printenv.pl/with/additional/path"
	// exectutable_path: "/cgi-bin/printenv.pl"
	// path_info	   : "/with/additional/path" // TODO: not implemented
	// request.query   : "and=a&query=string"

	const std::string path = get_path_on_disk(request);
	if (path == "")
		return respond_with_file_not_found(request);
	Response::cgi(request, path); // todo should we read the cgi executable from the config?
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
void Router::route(Client& client) {
	Request&				request = client.request;
	const Config::Location& location = request.associated_location();

	if (!_method_allowed(request, location)) {
		Response::text(request, 405, "");
		return;
	}
	if (request.path.compare("/favicon.ico") == 0) { // our server does not provide a favicon
		Response::text(request, 404, "");
		return;
	}

	if (location.auto_index == "on" &&
		request.path.at(request.path.size() - 1) == '/')
		return Response::text(request, 200, "Files: ..."); // TODO: read files in dir

	if (location.auto_index == "off") {
		std::string path = get_path_on_disk(request);
		if (fs::is_direcory(path))
			return Response::text(request, 403, "Cannot access directory"); // TODO: should this be a 404 not found?
	}

	if (location.cgi_path.first.size())
		return rout_cgi(request);

	if (request.method == "GET") {
		const std::string path = get_path_on_disk(request);
		if (path == "")
			return respond_with_file_not_found(request);
		return Response::file(request, path);
	}
	if (request.method == "POST") {
		return Response::text(request, 200, "POST not jet implemented"); // TODO
	}
}
