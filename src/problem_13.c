// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stddef.h>

#define N_THREADS 10
#define N_ITERATIONS 2

const char *const problem_13_name = "No-starve mutex";

struct Data {
	Semaphore mutex;
	Semaphore turnstile1;
	Semaphore turnstile2;
	int room1;
	int room2;
	int count;
};

static void *run(void *ptr) {
	struct Data *d = ptr;

	for (int i = 0; i < N_ITERATIONS; i++) {
		sema_wait(d->mutex);
		increment(&d->room1);
		sema_signal(d->mutex);

		sema_wait(d->turnstile1);
		increment(&d->room2);
		sema_wait(d->mutex);
		decrement(&d->room1);
		if (d->room1 == 0) {
			sema_signal(d->mutex);
			sema_signal(d->turnstile2);
		} else {
			sema_signal(d->mutex);
			sema_signal(d->turnstile1);
		}

		sema_wait(d->turnstile2);
		decrement(&d->room2);

		// Critical section.
		increment(&d->count);

		if (d->room2 == 0) {
			sema_signal(d->turnstile1);
		} else {
			sema_signal(d->turnstile2);
		}
	}

	return NULL;
}

bool problem_13(bool positive) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1, positive),
		.turnstile1 = sema_create(1, positive),
		.turnstile2 = sema_create(0, positive),
		.room1 = 0,
		.room2 = 0,
		.count = 0
	};

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = data.count == N_THREADS * N_ITERATIONS;

	// Clean up.
	sema_destroy(data.mutex);
	sema_destroy(data.turnstile1);
	sema_destroy(data.turnstile2);

	return success;
}

