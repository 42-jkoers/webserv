#include <iostream>

int main(int argc, char* argv[], char* envp[]) {
	(void)argc;
	std::cout << "Content-type: text/plain\r\n\r\n"
			  << std::ends;

	std::cout << "==== ARGV ====" << std::endl;
	char** v = argv;
	while (*v) {
		std::cout << *v << std::endl;
		v++;
	}
	std::cout << "\n==== ENVP ====" << std::endl;
	char** p = envp;
	while (*p) {
		std::cout << *p << std::endl;
		p++;
	}
	std::cout << "\n==== END =====" << std::endl;
	return 0;
}
