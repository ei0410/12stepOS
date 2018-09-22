#include "defines.h"
#include "kozos.h"
#include "lib.h"

int test08_1_main(int argc, char *argv[])
{
	char buf[32];

	puts("test08_1 started.\n");

	while (1) {
		puts("> ");
		gets(buf);

		if (!strcmp(buf, "echo", 4)) {
			puts(buf + 4);
			puts("\n");
		} else if (!strcmp(buf, "exit")) {
			break;
		} else {
			puts("unknown.\n");
		}
	}

	putrs("test08_1 exit.\n");

	return 0;
}
