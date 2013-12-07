#include <stdio.h>
#include <stdlib.h>

#define DEF_TRIES 10

int main(int argc, const char *argv[])
{
	int i, opened, tries;
	const char *device;
	FILE **opened_files;

	if(argc < 1)
	{
		printf("Usage: access [filename] [trycount]\n");
		return -1;
	}

	if(argc > 2)
		tries = atoi(argv[2]);
	else
		tries = DEF_TRIES;
	
	printf("To open: %s.\n", argv[1]);
	
	opened_files = malloc(tries * sizeof (FILE*));
	if(opened_files == NULL)
		return -2;

	device = argv[1];
	opened = 0;

	for(i = 0; i < tries; i++)
	{
		printf("%d Opening %s... ", i, device);
		opened_files[i] = fopen(device, "r");
		if(opened_files[i] == NULL)
			printf("FAILED\n");
		else
		{
			printf("SUCCESS\n");
			opened++;
		}
	}

	for(i = 0; i < tries; i++)
	{
		if(opened_files[i] == NULL)
			continue;
		printf("%d Closing %s... ",i, device);
		if(fclose(opened_files[i]) < 0)
			printf("FAILED\n");
		else
			printf("SUCCESS\n");
	}
	free(opened_files);
	printf("%d times opened.", opened);
	return 0;
}
