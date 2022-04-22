#pragma once
#include "header_field.hpp"
#include "main.hpp"
#include <netinet/in.h>

class Request {
  public:
	Request(){};
	Request(uint16_t port);
	~Request(){};

	bool				field_exists(const std::string& field) const;
	const Header_field& field(const std::string& field) const;

	const std::string&	field_value(const std::string& _field) const;
	const std::string&	field_value(const std::string& _field, size_t index) const;
	bool				field_is(const std::string& field, const std::string& value) const;
	bool				field_contains(const std::string& field, const std::string& part) const;
	ssize_t				field_content_length() const;
	std::string			field_multipart_boundary() const;
	std::string			field_filename() const;

	void				set_fd(fd_t f) { this->fd = f; }
	// read-only variables
	uint16_t	port;
	uint32_t	response_code;
	fd_t		fd;
	std::string method;
	std::string uri;
	std::string http_version;
	std::string path;
	std::string absolute_form;
	std::string query;
	int			server_index;
	std::string body;

  protected:
	void								parse_header(const std::string& raw);
	int									parse_line(const std::string& line);
	std::map<std::string, Header_field> header_fields;
	std::map<std::string, std::string>	_request_line;

  private:
	std::string _raw;
	std::string _whitespaces;
	std::string _crlf;
	int			_parse_request_line();
	int			_parse_header_fields();
	int			_parse_field_values();
	int			_parse_host();
	int			_set_response_code(int ret);
	int			_parse_URI();

	friend class Client;
	friend std::ostream& operator<<(std::ostream& output, Request const& rhs);

	// disabled
	// Request(const Request& cp); // TODO
	// Request& operator=(const Request& cp);
};

std::ostream& operator<<(std::ostream& output, Request const& rhs);
