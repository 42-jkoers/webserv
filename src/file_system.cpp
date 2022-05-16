#include "file_system.hpp"
#include <dirent.h>
#include <fstream>
#include <stdlib.h>
#include <sys/stat.h>

namespace fs {

std::string exception_str(const std::string& path) {
	std::string err;
	err += "\nCannot open \"";
	err += path;
	err += "\": ";
	err += std::strerror(errno);
	return err;
}

// make directory recursively
void mkdir(const std::string& path) {
	size_t slash_pos = 0;
	while (slash_pos < path.size()) {
		slash_pos = std_ft::min(path.find("/", slash_pos), path.size());
		const std::string dir = path.substr(0, slash_pos);
		slash_pos++;
		if (dir == "/" || dir == "." || dir == "..")
			continue;
		if (path_exists(dir))
			continue;
		if (::mkdir(dir.c_str(), 0777))
			exit_with::e_perror("mkdir");
	}
}

std::string read_file(const std::string& path) {
	std::ifstream	  file;
	std::stringstream ss;

	file.open(path.c_str());
	if (!file.is_open())
		throw std::runtime_error(exception_str(path).c_str());
	ss << file.rdbuf();
	file.close();
	return ss.str();
}

void write_file(const std::string& path, const std::string& data) {
	size_t slash = path.find_last_of("/");
	if (slash != std::string::npos)
		fs::mkdir(path.substr(0, slash));

	std::fstream file;
	file.open(path.c_str(), std::ios::out);
	if (!file.is_open())
		throw std::runtime_error(fs::exception_str(path).c_str());
	file.write(data.data(), data.size());
	file.close();
}

std::vector<std::string> list_dir(const std::string& path) {
	std::vector<std::string> files;
	DIR*					 dir = opendir(path.c_str());
	if (!dir)
		throw std::runtime_error(exception_str(path).c_str());

	while (true) {
		struct dirent* entry = readdir(dir);
		if (!entry)
			break;
		// if (!strcmp(entry->d_name, ".") && !strcmp(entry->d_name, ".."))
		files.push_back(entry->d_name);
	}
	closedir(dir);
	return files;
}

bool path_exists(const std::string& path) {
	struct stat buffer;
	return stat(path.c_str(), &buffer) == 0;
}

bool is_direcory(const std::string& path) {
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
		if (s.st_mode & S_IFDIR)
			return true;
	return false;
}

} // namespace fs

namespace path {

// "  ./aa//bb///  " -> "aa/bb"
// "//a" -> "/a"
// "/" -> "/"
// "" -> ""
std::string normalize(std::string path) {
	size_t start = path.find_first_not_of("\t ");
	path.erase(0, start);
	if (path.substr(0, 2) == "./")
		path.erase(0, 2);
	size_t end = path.size();
	while (end && std::iswspace(path[end - 1]))
		end--;
	path.erase(end, path.size());
	std::vector<std::string> blocks = ft_split(path, "/");
	std::string				 out;
	if (path[0] == '/')
		out += '/';
	for (size_t i = 0; i < blocks.size(); i++) {
		if (blocks[i] == ".")
			continue;
		if (i != 0)
			out += '/';
		out += blocks[i];
	}
	return out;
}

bool is_same(const std::string& path1, const std::string& path2) {
	return path::normalize(path1) == path::normalize(path2);
}

// this is easier than recursive template matching
std::string join(const std::string& path1, const std::string& path2) {
	return path::normalize(path1 + "/" + path2);
}

std::string join(const std::string& path1, const std::string& path2, const std::string& path3) {
	return path::normalize(path1 + "/" + path2 + "/" + path3);
}

std::string join(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
	return path::normalize(path1 + "/" + path2 + "/" + path3 + "/" + path4);
}

// returns empty string on fail
std::string absolute(const std::string& path) {
	char* absolute_c = realpath(("./" + path).c_str(), NULL);
	if (!absolute_c)
		return "";
	std::string absolute(absolute_c);
	free(absolute_c);
	return absolute;
}

} // namespace path
