/*
* Small test app for reading a device file.
* Parameter: Give the device file to read from.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 128

// Prototypes of functions for Fabis coding style so that he is happy.
static int openFile(char *devFile);
static int closeFile(int fd);
static int readFile(int fd);

int main(int argc, char **argv)
{
	int fd;

	if (argc != 2) {
		printf("Usage: %s <device file>\n", argv[0]);
		return -1;
	}

	fd = openFile(argv[1]);
	if (fd == -1)
		return -1;

	readFile(fd);

	return closeFile(fd);
}

static int openFile(char *devFileName)
{
	int fd = open(devFileName, O_RDONLY);

        if (fd == -1) {
                perror("Error opening file.\n");
		// Do not know if fd can be 0, so return -1 ...
                return -1;
        }
	return fd;
}

static int closeFile(int fd)
{
	if (close(fd) == -1) {
		perror("Error closing file.\n");
                return -1;
	}
	return 1;
}

static int readFile(int fd)
{
	char buf[BUFFER_SIZE];
	int ret;

	while ((ret = read(fd, buf, BUFFER_SIZE))) {
		printf("Read data: %s\n", buf);
	}

	if (!ret)
		printf("Reached EOF\n");
	else {
		perror("Error closing file.\n");
                return -1;
	}

	return 1;
}
