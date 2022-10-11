#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/uio.h>

int main(int argc, char *argv[])
{
	const char buf1[] = "Hello, ";
	const char buf2[] = "Wikipedia ";
	const char buf3[] = "Community!\n";

	struct iovec bufs[] = {
		{ .iov_base = (void *)buf1, .iov_len = strlen(buf1) },
		{ .iov_base = (void *)buf2, .iov_len = strlen(buf2) },
		{ .iov_base = (void *)buf3, .iov_len = strlen(buf3) },
	};

	if (-1 == writev(STDOUT_FILENO, bufs, sizeof(bufs) / sizeof(bufs[0])))
	{
		perror("writev()");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}