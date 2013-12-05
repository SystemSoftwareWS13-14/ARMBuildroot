#include <stdio.h>
#include <string.h>

#define TRIES 10

int main(int argc, char *argv[])
{
	int i, opened;
	char *device;
	FILE* opened_files[TRIES];

	if(argc < 1)
	{
		printf("Give an argument.\n");
		return -1;
	}

	device = strcat("/dev/", argv[1]);
	opened = 0;

	for(i = 0; i < TRIES; ++i)
	{
		printf("%d Opening \"%s\"... ", i, device);
		opened_files[i] = fopen(device, "r");
		if(opened_files[i] == NULL)
			printf("FAILED\n");
		else
		{
			printf("SUCCESS\n");
			opened++;
		}
	}

	for(i = 0; i < TRIES; ++i)
	{
		if(opened_files[i] == NULL)
			continue;
		printf("%d Closing \"%s\"... ",i, device);
		if(fclose(opened_files[i]) < 0)
			printf("FAILED\n");
		else
			printf("SUCCESS\n");
	}
	printf("%d times opened.", opened);
	return 0;
}
