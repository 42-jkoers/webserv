#include "constants.hpp"
#include "main.hpp"

Constants::Constants() {
	// clang-format off
	_content_type[".aac"   ] = "audio/aac";
	_content_type[".abw"   ] = "application/x-abiword";
	_content_type[".arc"   ] = "application/x-freearc";
	_content_type[".avif"  ] = "image/avif";
	_content_type[".avi"   ] = "video/x-msvideo";
	_content_type[".azw"   ] = "application/vnd.amazon.ebook";
	_content_type[".bin"   ] = "application/octet-stream";
	_content_type[".bmp"   ] = "image/bmp";
	_content_type[".bz"    ] = "application/x-bzip";
	_content_type[".bz2"   ] = "application/x-bzip2";
	_content_type[".cda"   ] = "application/x-cdf";
	_content_type[".csh"   ] = "application/x-csh";
	_content_type[".css"   ] = "text/css";
	_content_type[".csv"   ] = "text/csv";
	_content_type[".doc"   ] = "application/msword";
	_content_type[".docx"  ] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_content_type[".eot"   ] = "application/vnd.ms-fontobject";
	_content_type[".epub"  ] = "application/epub+zip";
	_content_type[".gz"    ] = "application/gzip";
	_content_type[".gif"   ] = "image/gif";
	_content_type[".htm"   ] = "text/html";
	_content_type[".html"  ] = "text/html";
	_content_type[".ico"   ] = "image/vnd.microsoft.icon";
	_content_type[".ics"   ] = "text/calendar";
	_content_type[".jar"   ] = "application/java-archive";
	_content_type[".jpg"   ] = "image/jpeg";
	_content_type[".jpeg"  ] = "image/jpeg";
	_content_type[".js"    ] = "text/javascript";
	_content_type[".json"  ] = "application/json";
	_content_type[".jsonld"] = "application/ld+json";
	_content_type[".mid"   ] = "audio/midi" ;
	_content_type[".midi"  ] = "audio/midi" ;
	_content_type[".mjs"   ] = "text/javascript";
	_content_type[".mp3"   ] = "audio/mpeg";
	_content_type[".mp4"   ] = "video/mp4";
	_content_type[".mpeg"  ] = "video/mpeg";
	_content_type[".mpkg"  ] = "application/vnd.apple.installer+xml";
	_content_type[".odp"   ] = "application/vnd.oasis.opendocument.presentation";
	_content_type[".ods"   ] = "application/vnd.oasis.opendocument.spreadsheet";
	_content_type[".odt"   ] = "application/vnd.oasis.opendocument.text";
	_content_type[".oga"   ] = "audio/ogg";
	_content_type[".ogv"   ] = "video/ogg";
	_content_type[".ogx"   ] = "application/ogg";
	_content_type[".opus"  ] = "audio/opus";
	_content_type[".otf"   ] = "font/otf";
	_content_type[".png"   ] = "image/png";
	_content_type[".pdf"   ] = "application/pdf";
	_content_type[".php"   ] = "application/x-httpd-php";
	_content_type[".ppt"   ] = "application/vnd.ms-powerpoint";
	_content_type[".pptx"  ] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_content_type[".rar"   ] = "application/vnd.rar";
	_content_type[".rtf"   ] = "application/rtf";
	_content_type[".sh"    ] = "application/x-sh";
	_content_type[".svg"   ] = "image/svg+xml";
	_content_type[".swf"   ] = "application/x-shockwave-flash";
	_content_type[".tar"   ] = "application/x-tar";
	_content_type[".tif"   ] = "image/tiff";
	_content_type[".tiff"  ] = "image/tiff";
	_content_type[".ts"    ] = "video/mp2t";
	_content_type[".ttf"   ] = "font/ttf";
	_content_type[".txt"   ] = "text/plain";
	_content_type[".vsd"   ] = "application/vnd.visio";
	_content_type[".wav"   ] = "audio/wav";
	_content_type[".weba"  ] = "audio/webm";
	_content_type[".webm"  ] = "video/webm";
	_content_type[".webp"  ] = "image/webp";
	_content_type[".woff"  ] = "font/woff";
	_content_type[".woff2" ] = "font/woff2";
	_content_type[".xhtml" ] = "application/xhtml+xml";
	_content_type[".xls"   ] = "application/vnd.ms-excel";
	_content_type[".xlsx"  ] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_content_type[".xml"   ] = "application/xml";
	_content_type[".xul"   ] = "application/vnd.mozilla.xul+xml";
	_content_type[".zip"   ] = "application/zip";
	_content_type[".3gp"   ] = "video/3gpp";
	_content_type[".3g2"   ] = "video/3gpp2";
	_content_type[".7z"    ] = "application/x-7z-compressed";

	_response_str[100] = "Continue";
	_response_str[101] = "Switching Protocols";
	_response_str[200] = "OK";
	_response_str[201] = "Created";
	_response_str[202] = "Accepted";
	_response_str[203] = "Non-Authoritative Information";
	_response_str[204] = "No Content";
	_response_str[205] = "Reset Content";
	_response_str[206] = "Partial Content";
	_response_str[300] = "Multiple Choices";
	_response_str[301] = "Moved Permanently";
	_response_str[302] = "Found";
	_response_str[303] = "See Other";
	_response_str[304] = "Not Modified";
	_response_str[305] = "Use Proxy";
	_response_str[307] = "Temporary Redirect";
	_response_str[400] = "Bad Request";
	_response_str[401] = "Unauthorized";
	_response_str[402] = "Payment Required";
	_response_str[403] = "Forbidden";
	_response_str[404] = "Not Found";
	_response_str[405] = "Method Not Allowed";
	_response_str[406] = "Not Acceptable";
	_response_str[407] = "Proxy Authentication Required";
	_response_str[408] = "Request Timeout";
	_response_str[409] = "Conflict";
	_response_str[410] = "Gone";
	_response_str[411] = "Length Required";
	_response_str[412] = "Precondition Failed";
	_response_str[413] = "Payload Too Large";
	_response_str[414] = "URI Too Long";
	_response_str[415] = "Unsupported Media Type";
	_response_str[416] = "Range Not Satisfiable";
	_response_str[417] = "Expectation Failed";
	_response_str[426] = "Upgrade Required";
	_response_str[500] = "Internal Server Error";
	_response_str[501] = "Not Implemented";
	_response_str[502] = "Bad Gateway";
	_response_str[503] = "Service Unavailable";
	_response_str[504] = "Gateway Timeout";
	_response_str[505] = "HTTP Version Not Supported";

	_methods.push_back("GET");
	_methods.push_back("POST");
	_methods.push_back("DELETE");
	// clang-format on
}

const std::string& Constants::to_content_type(const std::string& filename) const {
	std::map<std::string, std::string>::const_iterator it = _content_type.find(filename);
	assert(it != _content_type.end());
	return it->second;
}

const std::string& Constants::to_response_string(uint32_t code) const {
	std::map<uint32_t, std::string>::const_iterator it = _response_str.find(code);
	assert(it != _response_str.end());
	return it->second;
}

bool Constants::is_valid_method(const std::string& method) const {
	for (size_t i = 0; i < _methods.size(); i++)
		if (_methods[i] == method)
			return true;
	return false;
}
