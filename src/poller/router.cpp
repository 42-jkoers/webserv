#include "router.hpp"
#include "constants.hpp"
#include "file_system.hpp"

Router::~Router() {
}

Router::Router() {
	return;
}

bool Router::_has_server_name(Config::Server server, std::string server_name) {
	for (std::string name : server.server_names) {
		if (name == server_name) {
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
	for (Config::Server& server : g_config.servers) { // loop over servers
		for (uint16_t p : server.ports) {			  // loop over ports
			if (p == port) {
				if (_has_server_name(server, hostname)) {
					return server;
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
	for (Config::Location location : server.locations) {
		size_t found = path.find(location.path);
		if (found != std::string::npos && found == 0) {
			if (last_path.size() < location.path.size()) {
				location_index = i;
				last_path = location.path;
			}
		}
		i++;
	}
	return server.locations[location_index];
}

bool Router::_method_allowed(const Request& request, const Config::Location& location) {
	if (location.allowed_methods.empty())
		return true;
	for (std::string method : location.allowed_methods) {
		if (to_upper(method) == request.method)
			return true;
	}
	return false;
}

void respond_with_error_code(const Request& request, const std::string& path, uint16_t error_code) {
	// TODO: check for custom error page in config file
	// if custom error page and this page exists return this file, else:
	Response::error(request, path, error_code);
}

std::string find_index(const Config::Location& location, std::string& path) {
	for (std::string index : location.indexes) {
		if (fs::path_exists(path + index))
			return index;
	}
	return "";
}

// returns the path of the file on disk
std::string get_path_on_disk(const Request& request, const Config::Location& location) {
	// The path where the server should start looking for files
	// eg:  request.path = "/cgi/test"
	//     location.path = "/cgi"
	//     location.root = "www/cgi"
	// Then mounted_path = "www/cgi/test"
	return location.root + request.path;
}

void dir_list(Request& request, const std::string& path) {
	std::string					   response;
	const std::vector<std::string> files = fs::list_dir(path, true);
	response += "<html><head><title>Index of " + request.path + "</title></head>\n";
	response += "<body>\n";
	response += "<h1>Index of " + request.path + "</h1>\n";
	response += "<hr><pre><a href=\"../\">../</a>\n";
	for (std::string file_name : files) {
		if (fs::is_direcory(path + file_name))
			file_name += "/";
		response += "<a href=\"" + file_name + "\">" + file_name + "</a>\n";
	}
	response += "</pre><hr></body>\n";
	response += "</html>";

	Response::text(request, 200, response);
}

void route_cgi(Request& request, std::string& path) {
	// parse http://example.com/cgi-bin/printenv.pl/with/additional/path?and=a&query=string to:
	// request.uri     : "/cgi-bin/printenv.pl/with/additional/path"
	// exectutable_path: "/cgi-bin/printenv.pl"
	// path_info	   : "/with/additional/path" // TODO: not implemented
	// request.query   : "and=a&query=string"
	if (!fs::path_exists(path))
		return respond_with_error_code(request, path, 404);
	Response::cgi(request, path, "", request.query); // todo should we read the cgi executable from the config?
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

If a request ends with a slash, NGINX treats it as a request for a directory and tries to find an index file in the directory
search for index.html or index if specified
if dir and autoindex on -> dir listing
if dir and autoindex off -> 403
if not exist -> 404
*/
void Router::route(Client& client) {
	Request&				request = client.request;
	const Config::Location& location = request.associated_location();
	std::string				path = get_path_on_disk(request, location);

	if (!_method_allowed(request, location)) {
		Response::text(request, 405, "");
		return;
	}
	if (!location.redirect.empty()) {
		respond_with_error_code(request, path, location.redirect_code);
		client.request.path = location.redirect;
		g_router.route(client);
	}
	if (request.method == "GET") {
		if (path.at(path.size() - 1) == '/') { // directory -> find index or else dir listing if autoindex on
			std::string index = find_index(location, path);
			if (index.size())
				return Response::file(request, path + index);
			if (fs::is_direcory(path) && location.auto_index == "off")
				return respond_with_error_code(request, path, 403);
			if (fs::is_direcory(path) && location.auto_index == "on")
				return dir_list(request, path);
			return respond_with_error_code(request, path, 404);
		}

		if (fs::path_exists(path) && !fs::is_direcory(path))
			return Response::file(request, path);
		return respond_with_error_code(request, path, 404);
	}

	if (request.method == "POST") {
		if (location.cgi_path.first.size())
			return route_cgi(request, path);
		return Response::text(request, 200, "POST not yet implemented"); // TODO
	}

	if (request.method == "DELETE") {
		return Response::text(request, 200, "DELETE not yet implemented"); // TODO
	}
}
