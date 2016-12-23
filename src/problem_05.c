// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

#define N_THREADS 10

const char *const problem_05_name = "Barrier";

struct Data {
	Semaphore mutex;
	Semaphore turnstile;
	int count;
	struct Buffer log;
};

static void *run(void *ptr) {
	struct Data *d = ptr;

	buf_push(&d->log, '0');

	sema_wait(d->mutex);
	d->count++;
	if (d->count == N_THREADS) {
		sema_signal(d->turnstile);
	}
	sema_signal(d->mutex);
	sema_wait(d->turnstile);
	sema_signal(d->turnstile);

	buf_push(&d->log, '1');

	return NULL;
}

bool problem_05(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
		.turnstile = sema_create(0),
		.count = 0
	};
	buf_init(&data.log, N_THREADS * 2);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = true;
	for (size_t i = 0; i < N_THREADS; i++) {
		success &= buf_read(&data.log, i) == '0';
	}
	for (size_t i = N_THREADS; i < N_THREADS * 2; i++) {
		success &= buf_read(&data.log, i) == '1';
	}

	// Clean up.
	sema_destroy(data.mutex);
	sema_destroy(data.turnstile);
	buf_free(&data.log);

	return success;
}

