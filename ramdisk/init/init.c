
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int i;
	char *hellomsg = "Hello from init!\r\n";
	char *crlf = "\r\n";
	write(STDERR_FILENO, hellomsg, strlen(hellomsg));
	for (i = 0; i < argc; ++i) {
		write(STDERR_FILENO, argv[i], strlen(argv[i]));
		write(STDERR_FILENO, crlf, strlen(crlf));
	}
	for (;;)
		/* nothing */;
	return 0;
}
