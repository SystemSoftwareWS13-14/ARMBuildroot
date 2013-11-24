#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	struct sysinfo info;

	while (1) {
		sysinfo(&info);

		printf("=====================\n");
		printf("Uptime = %ld\n", info.uptime);
		printf("=====================\n");
	
		sleep(1);
	}

	return 0;
}
