#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main() {
	setvbuf(stdout, NULL, _IONBF, 0);

	char buf[10000];
	bzero(buf, sizeof(buf));
	ssize_t n = read(0, buf, sizeof(buf));
	if (n >= 0)
		buf[n] = '\0';
	std::cout << "Content-type: text/plain\r\n\r\n " << std::endl;
	std::cout << "Read " << n << " bytes from stdin:\n<" << std::string(buf) << ">\n " << std::endl;
	return 0;
}
