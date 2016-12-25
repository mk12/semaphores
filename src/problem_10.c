// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stddef.h>

#define N_READERS 6
#define N_WRITERS 4
#define N_THREADS (N_READERS + N_WRITERS)

const char *const problem_10_name = "Reader-writer";

struct Data {
	Semaphore mutex;
	Semaphore room_empty;
	int readers;
	int value;
	struct Buffer log;
};

static void *run_reader(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->mutex);
	if (d->readers == 0) {
		sema_wait(d->room_empty);
	}
	increment(&d->readers);
	sema_signal(d->mutex);

	buf_push(&d->log, 'R');
	buf_push(&d->log, (unsigned char)d->value);

	sema_wait(d->mutex);
	decrement(&d->readers);
	if (d->readers == 0) {
		sema_signal(d->room_empty);
	}
	sema_signal(d->mutex);

	return NULL;
}

static void *run_writer(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->room_empty);
	increment(&d->value);
	buf_push(&d->log, 'W');
	buf_push(&d->log, (unsigned char)d->value);
	sema_signal(d->room_empty);

	return NULL;
}

bool problem_10(bool positive) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1, positive),
		.room_empty = sema_create(1, positive),
		.readers = 0,
		.value = 0
	};
	buf_init(&data.log, N_THREADS * 2);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_READERS; i++) {
		pthread_create(&threads[i], NULL, run_reader, &data);
	}
	for (size_t i = N_READERS; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run_writer, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = true;
	unsigned char value = 0;
	for (size_t i = 0; i < N_THREADS * 2; i += 2) {
		unsigned char c1 = buf_read(&data.log, i);
		unsigned char c2 = buf_read(&data.log, i + 1);

		if (c1 == 'R') {
			success &= c2 == value;
		} else if (c1 == 'W') {
			value++;
			success &= c2 == value;
		} else {
			success = false;
			break;
		}
	}
	success &= value == N_WRITERS;

	// Clean up.
	sema_destroy(data.mutex);
	sema_destroy(data.room_empty);
	buf_free(&data.log);

	return success;
}

