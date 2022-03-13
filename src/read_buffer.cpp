#include "poller.hpp"

Read_buffer::Read_buffer() {
	reset();
}

ssize_t Read_buffer::append(fd_t fd) {
	ssize_t bytes_read = read(fd, _data + _size, sizeof(_data) - _size);
	if (bytes_read > 0) {
		_size += bytes_read;
		// _data[_size] = '\0';
	}
	return bytes_read;
}

void Read_buffer::free_n(size_t n) { // TODO: cursor
	if (n > _size)
		n = _size;
	memmove(_data, _data + n, _size - n);
	_size -= n;
}

void Read_buffer::copy_to_vector(std::vector<char>& v, size_t n) {
	v.reserve(v.size() + n);

	for (size_t i = 0; i < _size; i++)
		v.push_back(data()[i]);
}

void Read_buffer::print(size_t max) {
	std::cout << "==================================================" << std::endl;
	size_t n = cpp11::min(max, _size);
	write(1, _data, n);
	std::cout << "\n================================================= " << n << "\n " << std::endl;
}

void Read_buffer::reset() {
	_size = 0;
}
