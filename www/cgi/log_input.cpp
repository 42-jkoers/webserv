#include <iostream>

int main(int argc, char* argv[], char* envp[]) {
	std::cout << "Content-type: text/plain\r\n\r\n"
			  << std::endl;

	std::cout << "argv" << std::endl;
	char** v = argv;
	while (*v) {
		std::cout << *v << std::endl;
		v++;
	}
	std::cout << "envp" << std::endl;
	char** p = envp;
	while (*p) {
		std::cout << *p << std::endl;
		p++;
	}
	return 0;
}
