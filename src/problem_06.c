// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stddef.h>

#define N_THREADS 10
#define N_ITERATIONS 3

const char *const problem_06_name = "Reusable barrier";

struct Data {
	Semaphore mutex;
	Semaphore turnstile1;
	Semaphore turnstile2;
	int count;
	struct Buffer log;
};

static void *run(void *ptr) {
	struct Data *d = ptr;

	for (int i = 0; i < N_ITERATIONS; i++) {
		buf_push(&d->log, (unsigned char)i);

		sema_wait(d->mutex);
		d->count++;
		if (d->count == N_THREADS) {
			sema_wait(d->turnstile2);
			sema_signal(d->turnstile1);
		}
		sema_signal(d->mutex);

		sema_wait(d->turnstile1);
		sema_signal(d->turnstile1);

		sema_wait(d->mutex);
		d->count--;
		if (d->count == 0) {
			sema_wait(d->turnstile1);
			sema_signal(d->turnstile2);
		}
		sema_signal(d->mutex);

		sema_wait(d->turnstile2);
		sema_signal(d->turnstile2);
	}

	return NULL;
}

bool problem_06(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
		.turnstile1 = sema_create(0),
		.turnstile2 = sema_create(1),
		.count = 0
	};
	buf_init(&data.log, N_THREADS * N_ITERATIONS);

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
	for (size_t i = 0; i < N_ITERATIONS; i++) {
		for (size_t j = 0; j < N_THREADS; j++) {
			size_t index = i * N_THREADS + j;
			success &= buf_read(&data.log, index) == i;
		}
	}

	// Clean up.
	sema_destroy(data.mutex);
	sema_destroy(data.turnstile1);
	sema_destroy(data.turnstile2);
	buf_free(&data.log);

	return success;
}

