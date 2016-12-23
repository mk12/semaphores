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
	struct Buffer buf;
};

static void *run_producer(void *ptr) {
	struct Data *d = ptr;

	delay();
	sema_wait(d->mutex);
	buf_push(&d->buf, 'P');
	sema_signal(d->mutex);
	sema_signal(d->items);

	return NULL;
}

static void *run_consumer(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->items);
	sema_wait(d->mutex);
	buf_push(&d->buf, 'C');
	sema_signal(d->mutex);

	return NULL;
}

bool problem_8(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
		.items = sema_create(0),
	};
	buf_init(&data.buf, N_THREADS);

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
	int items = 0;
	for (size_t i = 0; i < N_THREADS; i++) {
		unsigned char c = buf_read(&data.buf, i);
		if (c == 'P') {
			items++;
		} else if (c == 'C') {
			success &= items > 0;
			items--;
		} else {
			success = false;
			break;
		}
	}
	success &= items == N_PRODUCERS - N_CONSUMERS;

	// Clean up.
	buf_free(&data.buf);
	sema_destroy(data.mutex);
	sema_destroy(data.items);

	return success;
}

