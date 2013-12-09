#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define DEF_TRIES 10
#define DEF_THREADS 4

static char *device;
static int tries;

void *run_thread(void *data);

int main(int argc, char *argv[])
{
	int thread_count,i;
	void *retval;
	pthread_t *threads;

	if(argc < 1)
	{
		printf("Usage: access [filename] [trycount]\n");
		return -1;
	}

	if(argc > 2)
		tries = atoi(argv[2]);
	else
		tries = DEF_TRIES;

	if(argc > 3)
		thread_count = atoi(argv[3]);
	else
		thread_count = DEF_THREADS;
	
	device = argv[1];
	threads = malloc(thread_count * sizeof(pthread_t));
	if(threads == NULL)
		return -1;

	printf("To open: %s.\n", device);	

	for(i = 0; i < thread_count; i++)
		 pthread_create(&threads[i],NULL, run_thread, NULL); 	

	for(i = 0; i < thread_count; i++)
		pthread_join(threads[i], &retval);	
	
	free(threads); 
	return 0;
}

void *run_thread(void *data)
{
	int i, opened;
	int *opened_files;
	int retval;
	retval = 0;	

	opened_files = malloc(tries * sizeof(int));
	if(opened_files == NULL)
	{
		retval = -1;
		pthread_exit(&retval);
	}

	opened = 0;

	for(i = 0; i < tries; i++)
	{
		//open file
		opened_files[i] = open(device, O_RDONLY);
		if(opened_files[i] < 0)
		{
			//close previous file if was open
			if( i > 0 && opened_files[i - 1] >= 0)
				close(opened_files[i - 1]);
		}
		else
			opened++;
	}
	
	//close last file if open
	if(opened_files[i] >= 0)
		close(opened_files[i - 1]);
	
	free(opened_files);
	printf("%d times opened\n", opened);
	pthread_exit(&retval);
}
