// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

// Fixed-capacity dynamic-length thread-safe buffer data type.
struct Buffer {
	unsigned char *arr;
	pthread_mutex_t mutex;
	size_t len;
	size_t cap;
};

// Initializes the buffer with capacity 'cap'. Call only once.
void buf_init(struct Buffer *buf, size_t cap);

// Frees the buffer's memory. Do not use after calling this.
void buf_free(struct Buffer *buf);

// Reads the byte in the buffer at index 'i'.
unsigned char buf_read(struct Buffer *buf, size_t i);

// Writes the character 'c' at index 'i' in the buffer.
void buf_write(struct Buffer *buf, size_t i, unsigned char c);

// Writes the character 'c' at the end of the buffer.
void buf_push(struct Buffer *buf, unsigned char c);

// Returns true if the buffer contents are the same as the string 's'.
bool buf_eq(struct Buffer *buf, const char* s);

// Returns true if the buffer contents from index 'i' (inclusive) to index 'j'
// (exclusive) are the same as the string 's'.
bool buf_range_eq(struct Buffer *buf, size_t i, size_t j, const char* s);

#endif
