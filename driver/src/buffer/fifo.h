#ifndef BUFFER_FIFO
#define BUFFER_FIFO

typedef struct {
	char * data;
	int index;
	int size;
	int byteCount;
} buffer;

int buf_init(buffer *buf, const int size);
int buf_read(buffer *buf, int byte, char *out);
int buf_write(buffer *buf, int byte, char *in);
int buf_destroy(buffer *buf);

#endif
