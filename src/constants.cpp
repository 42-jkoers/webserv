#include "constants.hpp"
#include "main.hpp"

Constants::Constants() {
}

std::string to_content_type(const std::string& filename) {

	std::map<std::string, std::string> cgi;
	// clang-format off
	cgi[ ".aac"   ] = "audio/aac";
	cgi[ ".abw"   ] = "application/x-abiword";
	cgi[ ".arc"   ] = "application/x-freearc";
	cgi[ ".avif"  ] = "image/avif";
	cgi[ ".avi"   ] = "video/x-msvideo";
	cgi[ ".azw"   ] = "application/vnd.amazon.ebook";
	cgi[ ".bin"   ] = "application/octet-stream";
	cgi[ ".bmp"   ] = "image/bmp";
	cgi[ ".bz"    ] = "application/x-bzip";
	cgi[ ".bz2"   ] = "application/x-bzip2";
	cgi[ ".cda"   ] = "application/x-cdf";
	cgi[ ".csh"   ] = "application/x-csh";
	cgi[ ".css"   ] = "text/css";
	cgi[ ".csv"   ] = "text/csv";
	cgi[ ".doc"   ] = "application/msword";
	cgi[ ".docx"  ] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	cgi[ ".eot"   ] = "application/vnd.ms-fontobject";
	cgi[ ".epub"  ] = "application/epub+zip";
	cgi[ ".gz"    ] = "application/gzip";
	cgi[ ".gif"   ] = "image/gif";
	cgi[ ".htm"   ] = "text/html";
	cgi[ " .html" ] = "text/html";
	cgi[ ".ico"   ] = "image/vnd.microsoft.icon";
	cgi[ ".ics"   ] = "text/calendar";
	cgi[ ".jar"   ] = "application/java-archive";
	cgi[ ".jpg"   ] = "image/jpeg";
	cgi[ ".jpeg"  ] = "image/jpeg";
	cgi[ ".js"    ] = "text/javascript";
	cgi[ ".json"  ] = "application/json";
	cgi[ ".jsonld"] = "application/ld+json";
	cgi[ ".mid"   ] = "audio/midi" ;
	cgi[  ".midi" ] = "audio/midi" ;
	cgi[ ".mjs"   ] = "text/javascript";
	cgi[ ".mp3"   ] = "audio/mpeg";
	cgi[ ".mp4"   ] = "video/mp4";
	cgi[ ".mpeg"  ] = "video/mpeg";
	cgi[ ".mpkg"  ] = "application/vnd.apple.installer+xml";
	cgi[ ".odp"   ] = "application/vnd.oasis.opendocument.presentation";
	cgi[ ".ods"   ] = "application/vnd.oasis.opendocument.spreadsheet";
	cgi[ ".odt"   ] = "application/vnd.oasis.opendocument.text";
	cgi[ ".oga"   ] = "audio/ogg";
	cgi[ ".ogv"   ] = "video/ogg";
	cgi[ ".ogx"   ] = "application/ogg";
	cgi[ ".opus"  ] = "audio/opus";
	cgi[ ".otf"   ] = "font/otf";
	cgi[ ".png"   ] = "image/png";
	cgi[ ".pdf"   ] = "application/pdf";
	cgi[ ".php"   ] = "application/x-httpd-php";
	cgi[ ".ppt"   ] = "application/vnd.ms-powerpoint";
	cgi[ ".pptx"  ] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	cgi[ ".rar"   ] = "application/vnd.rar";
	cgi[ ".rtf"   ] = "application/rtf";
	cgi[ ".sh"    ] = "application/x-sh";
	cgi[ ".svg"   ] = "image/svg+xml";
	cgi[ ".swf"   ] = "application/x-shockwave-flash";
	cgi[ ".tar"   ] = "application/x-tar";
	cgi[ ".tif"   ] = "image/tiff";
	cgi[ ".tiff"  ] = "image/tiff";
	cgi[ ".ts"    ] = "video/mp2t";
	cgi[ ".ttf"   ] = "font/ttf";
	cgi[ ".txt"   ] = "text/plain";
	cgi[ ".vsd"   ] = "application/vnd.visio";
	cgi[ ".wav"   ] = "audio/wav";
	cgi[ ".weba"  ] = "audio/webm";
	cgi[ ".webm"  ] = "video/webm";
	cgi[ ".webp"  ] = "image/webp";
	cgi[ ".woff"  ] = "font/woff";
	cgi[ ".woff2" ] = "font/woff2";
	cgi[ ".xhtml" ] = "application/xhtml+xml";
	cgi[ ".xls"   ] = "application/vnd.ms-excel";
	cgi[ ".xlsx"  ] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	cgi[ ".xml"   ] = "application/xml";
	cgi[ ".xul"   ] = "application/vnd.mozilla.xul+xml";
	cgi[ ".zip"   ] = "application/zip";
	cgi[ ".3gp"   ] = "video/3gpp";
	cgi[ ".3g2"   ] = "video/3gpp2";
	cgi[ ".7z"    ] = "application/x-7z-compressed";
	// clang-format on
}
