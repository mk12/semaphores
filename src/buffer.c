// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_BYTE 0xEE

void buf_init(struct Buffer *buf, size_t cap) {
	buf->arr = malloc(cap);
	buf->len = 0;
	buf->cap = cap;
	buf->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
}

void buf_free(struct Buffer *buf) {
	free(buf->arr);
	buf->arr = NULL;
}

unsigned char buf_read(struct Buffer *buf, size_t i) {
	unsigned char c;
	pthread_mutex_lock(&buf->mutex);
	assert(i < buf->cap);
	c = buf->arr[i];
	pthread_mutex_unlock(&buf->mutex);
	return c;
}

void buf_push(struct Buffer *buf, unsigned char c) {
	pthread_mutex_lock(&buf->mutex);
	assert(buf->len < buf->cap);
	buf->arr[buf->len++] = c;
	pthread_mutex_unlock(&buf->mutex);
}

unsigned char buf_pop(struct Buffer *buf) {
	pthread_mutex_lock(&buf->mutex);
	// Return ERROR_BYTE so that tests with semaphores disabled don't crash.
	unsigned char c = buf->len > 0 ? buf->arr[--buf->len] : ERROR_BYTE;
	pthread_mutex_unlock(&buf->mutex);
	return c;
}

bool buf_eq(struct Buffer *buf, const char* s) {
	return strlen(s) == buf->len &&
		strncmp((const char *)buf->arr, s, buf->len) == 0;
}

bool buf_range_eq(struct Buffer *buf, size_t i, size_t j, const char* s) {
	assert(i <= j);
	assert(j <= buf->cap);
	return strlen(s) == j - i &&
		strncmp((const char *)(buf->arr + i), s, j - i) == 0;
}
