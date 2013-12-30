#include "fifo_tsafe.h"
#include <linux/slab.h>

buffer buf_get(void)
{
	buffer buf;
	
	buf.data = NULL;
	buf.size = -1;
	buf.index = -1;
	buf.byteCount = -1;
	mutex_init(&buf.buffer_mutex);
	
	return buf;
}

int buf_init(buffer *buf, const int size)
{
	mutex_lock(&buf->buffer_mutex);
	
	// Buffer already initialized?
	if (buf->size != -1) {
		mutex_unlock(&buf->buffer_mutex);
		return 0;
	}
	
	buf->data = kmalloc(size, GFP_KERNEL);
	if (buf->data == NULL) {
		mutex_unlock(&buf->buffer_mutex);
		return 0;
	}

	buf->size = size;
	buf->index = 0;
	buf->byteCount = 0;

	mutex_unlock(&buf->buffer_mutex);

	return 1;
}

int buf_read(buffer *buf, char *out, int byte)
{
	int i;
	int toRead;
	
	mutex_lock(&buf->buffer_mutex);

	toRead = min(byte, buf->byteCount);

	for (i = 0; i < toRead; ++i) {
		out[i] = buf->data[buf->index];
		buf->index = (buf->index + 1) % buf->size;
	}

	buf->byteCount -= toRead;
	
	mutex_unlock(&buf->buffer_mutex);

	return toRead;
}

int buf_write(buffer *buf, char *in, int byte)
{
	int i, index_w;
	int toWrite;
	
	mutex_lock(&buf->buffer_mutex);

	toWrite = min(byte, buf->size - buf->byteCount);

	for (i = 0; i < toWrite; ++i) {
		index_w = (buf->index + buf->byteCount) % buf->size;
		buf->data[index_w] = in[i];
		++buf->byteCount;
	}
	
	mutex_unlock(&buf->buffer_mutex);

	return toWrite;
}

int buf_destroy(buffer *buf)
{
	mutex_lock(&buf->buffer_mutex);
	
	if (buf->data == NULL) {
		mutex_unlock(&buf->buffer_mutex);
		return 0;
	}

	kfree(buf->data);
	buf->size = -1;
	buf->index = -1;
	buf->byteCount = -1;
	
	mutex_unlock(&buf->buffer_mutex);

	return 1;
}

int buf_isempty(buffer *buf)
{
	int ret;
	mutex_lock(&buf->buffer_mutex);
	ret = (buf->byteCount <= 0);
	mutex_unlock(&buf->buffer_mutex);
	
	return ret;
}

int buf_isfull(buffer *buf)
{
	int ret;
	mutex_lock(&buf->buffer_mutex);
	ret = (buf->byteCount >= buf->size);
	mutex_unlock(&buf->buffer_mutex);
	
	return ret;
}
