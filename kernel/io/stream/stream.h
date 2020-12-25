#ifndef STREAM_H
#define	STREAM_H

#include "../../../lib/std/stdtypes.h"

/**
 * Initializes the default stream and opens it
 * @return Returns 'true' on success, otherwise 'false'
 */
bool stream_init();

/**
 * Writes a buffer to the stream
 * @param data The buffer to write to the stream
 * @param size The size of the buffer
 * @return Returns the number of bytes written to the stream
 */
int stream_write_buf(const void* data, unsigned int size);

/**
 * Writes a string to the stream
 * @param str The string to write to the stream
 * @return Returns the number of chars written to the stream
 */
int stream_write_str(const char* str);

/**
 * Reads a buffer from the stream
 * @param data Read bytes from the stream to the buffer
 * @param size The size of the buffer
 * @return Returns the number of bytes read from the stream
 */
int stream_read_buf(void* data, unsigned int size);

/**
 * Reads a string from the stream
 * @param data Read chars from the stream to the bufffer
 * @param size The size of the buffer
 * @return Returns the number of bytes read from the stream
 */
int stream_read_str(char* data, unsigned int size);

/**
 * Closes the current stream
 */
void stream_close();

#endif	/* STREAM_H */

