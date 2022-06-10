#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main(void) {
	setvbuf(stdout, NULL, _IONBF, 0);

	char buf[10000];
	bzero(buf, sizeof(buf));
	read(0, buf, sizeof(buf));
	printf("Hello from cgi\n");

	return 0;
}
