#ifndef BUFFER_FIFO
#define BUFFER_FIFO

typedef struct {
	char * data;
	int index;
	int size;
	int byteCount;
} buffer;

int buf_init(buffer *buf, const int size);
int buf_read(buffer *buf, char *out, int byte);
int buf_write(buffer *buf, char *in, int byte);
int buf_destroy(buffer *buf);
int buf_isempty(buffer *buf);
int buf_isfull(buffer *buf);

#endif
