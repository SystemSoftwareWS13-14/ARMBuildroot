#include <linux/kernel.h>
#include <linux/slab.h>
#include "fifo.h"


int buf_init(buffer *buf, const int size)
{
	buf->data = kmalloc(size, GFP_KERNEL);
	if (buf->data == NULL)
		return 0;

	buf->size = size;
	buf->index = 0;
	buf->byteCount = 0;

	return 1;
}

int buf_read(buffer *buf, int byte, char *out)
{
	int i;
	int toRead = min(byte, buf->byteCount);

	for (i = 0; i < toRead; ++i) {
		out[i] = buf->data[buf->index];
		buf->index = (buf->index + 1) % buf->size;
	}

	buf->byteCount -= toRead;

	return toRead;
}

int buf_write(buffer *buf, int byte, char *in)
{
	int i, index_w;
	int toWrite = min(byte, buf->size - buf->byteCount);

	for (i = 0; i < toWrite; ++i) {
		index_w = (buf->index + buf->byteCount) % buf->size;
		buf->data[index_w] = in[i];
		++buf->byteCount;
	}

	return toWrite;
}

int buf_destroy(buffer *buf)
{
	if (buf->data == NULL)
		return 0;

	kfree(buf->data);
	buf->size = -1;
	buf->index = -1;
	buf->byteCount = -1;

	return 1;
}
