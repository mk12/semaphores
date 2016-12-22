// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

void buf_write(struct Buffer *buf, size_t i, unsigned char c) {
	pthread_mutex_lock(&buf->mutex);
	assert(i < buf->cap);
	buf->arr[i] = c;
	pthread_mutex_unlock(&buf->mutex);
}

void buf_push(struct Buffer *buf, unsigned char c) {
	pthread_mutex_lock(&buf->mutex);
	assert(buf->len < buf->cap);
	buf->arr[buf->len++] = c;
	pthread_mutex_unlock(&buf->mutex);
}

int buf_equals(struct Buffer *buf, const char* s) {
	return strlen(s) == buf->len &&
		strncmp((const char *)buf->arr, s, buf->len) == 0;
}
