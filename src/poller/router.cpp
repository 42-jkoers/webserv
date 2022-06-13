#include "router.hpp"
#include "constants.hpp"
#include "file_system.hpp"
#include "main.hpp"
#include <ctime>
#include <fcntl.h>

Router::~Router() {
}

Router::Router() {
	return;
}

static bool has_server_name(const Config::Server& server, const std::string& server_name) {
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
const Config::Server& Router::find_server(const uint16_t port, const std::string& hostname) {
	int	   found = 0;
	size_t i = 0;
	size_t server_index;

	for (Config::Server& server : g_config.servers) { // loop over servers
		for (uint16_t p : server.ports) {			  // loop over ports
			if (p == port) {
				if (has_server_name(server, hostname)) {
					return server;
				} else if (found == 0) { // the first server with correct port = default server
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

bool method_allowed(const Request& request) {
	const Config::Location location = request.associated_location();

	for (std::string method : location.allowed_methods) {
		if (to_upper(method) == request.method)
			return true;
	}
	return false;
}

Route Router::_respond_with_error_code(const Request& request, const std::string& path, uint16_t error_code) {
	const Config::Server& server = request.associated_server();

	for (const std::pair<const size_t, std::string>& error_page : server.error_pages) {
		if (error_page.first == error_code) {
			std::string error_path = "www/html" + error_page.second;
			if (!fs::path_exists(error_path))
				return Response::error(error_path, 500);
			return Response::file(error_path, error_code); // return file if file exists
		}
	}
	return Route(Response::error(path, error_code));
}

Route Router::_respond_redirect(const Request& request) {
	// For a code in the 3xx series, the urlparameter defines the new (rewritten) URL
	// return (301 | 302 | 303 | 307) url;
	// only 300 | 301 | 302 | 303 | 307 (and 308) provide Location: <path> in response header

	// other codes:
	// you optionally define a text string which appears in the body of the response (and not the header)
	// return (1xx | 2xx | 4xx | 5xx) ["text"];
	const Config::Location& location = request.associated_location();

	if (location.redirect.code == 304)
		return Route(Response::redirect(location.redirect.code, "", ""));
	else if ((location.redirect.code >= 301 && location.redirect.code <= 303) || location.redirect.code == 307)
		return Route(Response::redirect(location.redirect.code, "", location.redirect.text));
	return Route(Response::redirect(location.redirect.code, location.redirect.text, ""));
	// request.path = location.redirect;
	// g_router.route(client);
}

std::string Router::_find_index(const Config::Location& location, std::string& path) {
	for (std::string index : location.indexes) {
		if (fs::path_exists(path + index))
			return index;
	}
	return "";
}

// returns the path of the file on disk
std::string get_path_on_disk(const Request& request) {
	// The path where the server should start looking for files
	// eg:  request.path = "/cgi/test"
	//     location.path = "/cgi"
	//     location.root = "www/cgi"
	// Then mounted_path = "www/cgi/test"
	return request.associated_location().root + request.path;
}

Route Router::_dir_list(Request& request, const std::string& path) {
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

	return Route(Response::text(200, response));
}

Route Router::_route_cgi(Request& request, std::string& path) {
	if (!fs::path_exists(path))
		return _respond_with_error_code(request, path, 404);
	Response::cgi(request, path);
	return Route("");
}

Route Router::_route_get(Request& request, std::string& path) {
	const Config::Location& location = request.associated_location();

	if (path.at(path.size() - 1) == '/') { // directory -> find index or else dir listing if autoindex on
		std::string index = _find_index(location, path);
		if (index.size())
			return Response::file(path + index, 200);
		if (fs::is_direcory(path) && location.auto_index == "off")
			return _respond_with_error_code(request, path, 403);
		if (fs::is_direcory(path) && location.auto_index == "on")
			return _dir_list(request, path);
		return _respond_with_error_code(request, path, 404);
	}

	if (fs::path_exists(path) && !fs::is_direcory(path))
		return Response::file(path, 200);
	return _respond_with_error_code(request, path, 404);
}

Route Router::_route_post(Request& request) {
	std::string save_path = path::join(request.associated_location().root, request.path);
	if (!fs::path_exists(save_path))
		return Route(Response::text(500, "Could not open path \"" + request.path + "\""));

	std::string filename = request.field_filename();
	if (filename == "")
		filename = std::to_string(std::time(0));
	save_path = path::join(save_path, filename);
	fs::write_file(save_path, request.body);
	return Route(Response::text(200, "Saved upload to file \"" + save_path + "\" on disk"));
}

Route Router::_route_delete(Request& request) {
	std::string path = path::join(request.associated_location().root, request.path);

	if (fs::delete_file(path))
		return Route(Response::text(200, "Deleted file \"" + path + "\" on disk"));
	else
		return Route(Response::text(500, "Could not delete file \"" + path + "\" on disk"));
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
Route Router::route(Client& client) {
	if (client.request.response_code != 200) // do not route if error in request reading/parsing has happened
		return Route(Response::error("", client.request.response_code));

	Request&				request = client.request;
	const Config::Location& location = request.associated_location();
	std::string				path = get_path_on_disk(request);

	if (!method_allowed(request))
		return _respond_with_error_code(request, path, 405);
	if (location.redirect.code != 0)
		return _respond_redirect(request);
	if (location.cgi_path.first.size() && location.auto_index == "off" && !fs::is_direcory(path))
		return _route_cgi(request, path);
	if (request.method == "GET")
		return _route_get(request, path);
	if (request.method == "POST")
		return _route_post(request);
	if (request.method == "DELETE")
		return _route_delete(request);
	return Route(Response::text(500, "Critical internal error"));
}
