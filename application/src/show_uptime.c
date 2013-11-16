#include <stdio.h>
#include <sys/sysinfo.h>

int main(int argc, char **argv)
{
	struct sysinfo info;
	sysinfo(&info);

	printf("=====================\n");
	printf("Uptime = %ld\n", info.uptime);
	printf("=====================\n");

	return 0;
}
