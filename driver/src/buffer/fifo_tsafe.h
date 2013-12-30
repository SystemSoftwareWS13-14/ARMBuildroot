/*
* Thread-safe implementation of fifo.
*/

#ifndef BUFFER_FIFO
#define BUFFER_FIFO

#include <linux/mutex.h>

typedef struct {
	char * data;
	int index;
	int size;
	int byteCount;
	struct mutex buffer_mutex;
} buffer;

buffer buf_get(void);
int buf_init(buffer *buf, const int size);
int buf_read(buffer *buf, char *out, int byte);
int buf_write(buffer *buf, char *in, int byte);
int buf_destroy(buffer *buf);
int buf_isempty(buffer *buf);
int buf_isfull(buffer *buf);

#endif
