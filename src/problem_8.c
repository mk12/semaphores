// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

#define N_PRODUCERS 6
#define N_CONSUMERS 4
#define N_THREADS (N_PRODUCERS + N_CONSUMERS)

const char *const problem_8_name = "Producer-consumer";

struct Data {
	Semaphore mutex;
	Semaphore items;
	unsigned char next;
	struct Buffer buf;
	struct Buffer log;
};

static void *run_producer(void *ptr) {
	struct Data *d = ptr;

	delay();
	sema_wait(d->mutex);
	unsigned char item = d->next++;
	buf_push(&d->buf, item);
	buf_push(&d->log, 'P');
	buf_push(&d->log, item);
	sema_signal(d->mutex);
	sema_signal(d->items);

	return NULL;
}

static void *run_consumer(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->items);
	sema_wait(d->mutex);
	unsigned char item = buf_pop(&d->buf);
	buf_push(&d->log, 'C');
	buf_push(&d->log, item);
	sema_signal(d->mutex);

	return NULL;
}

bool problem_8(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
		.items = sema_create(0),
		.next = 0
	};
	buf_init(&data.buf, N_PRODUCERS);
	buf_init(&data.log, N_THREADS * 2);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_PRODUCERS; i++) {
		pthread_create(&threads[i], NULL, run_producer, &data);
	}
	for (size_t i = N_PRODUCERS; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run_consumer, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = true;
	bool waiting[N_PRODUCERS] = { false };
	size_t items = 0;
	for (size_t i = 0; i < N_THREADS * 2; i += 2) {
		unsigned char c1 = buf_read(&data.log, i);
		unsigned char c2 = buf_read(&data.log, i + 1);
		if (c2 >= N_PRODUCERS) {
			success = false;
			break;
		}

		if (c1 == 'P') {
			success &= !waiting[c2];
			waiting[c2] = true;
			items++;
		} else if (c1 == 'C') {
			success &= waiting[c2];
			waiting[c2] = false;
			items--;
		} else {
			success = false;
			break;
		}
	}
	success &= items == N_PRODUCERS - N_CONSUMERS;
	success &= data.buf.len == items;

	// Clean up.
	sema_destroy(data.mutex);
	sema_destroy(data.items);
	buf_free(&data.log);
	buf_free(&data.buf);

	return success;
}

