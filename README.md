# Webserv

Brought to you by the amazing Iris, Joppe and Liz.

Currently in the process of creating a slimmed down version of nginx from scratch in C++98

## Structure
- Class::Config (global)
	- Reads and validates configuration file
    - Class::Location
    - Class::Server

- Class::Poller
	- Handles all incoming requests on `poll()`
	- Manages all open file descriptors
	- Attempts to read as much bytes as possible from client
	- Class::Client
		- Stores in progress raw read data
		- TODO: validate max body size
		- Class::Request
			- Attempts to parse raw incoming header bytes
			- Validates request line: HTTP version and method (GET/POST/DELETE)
			- Validates Host present
            - Validates Content-length
            - Stores request line attributes and header fields
            - Class::Header_field
		        - Stores name and value

- Class::Router
    - Validates valid request
    - Routes requests to the right server
    - TODO: make class
    - TODO: match request with server in config

- Response::text() | Response::cgi() | ...
	- Generates response in accordance with the HTTP standard
    - TODO: class -> namespace

- Class::Constants
	- Stores key value pairs
		- HTTP codes
		- Content type

## Incoming request flow

`Poller::start()`\
`Poller::read_pollfd()`\
`Poller::_parse()`\
Raw bytes read\
`on_request()`\
TODO: `Router::route()`\
`Response::text() | Response::file() | ...`\
Done
