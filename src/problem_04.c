// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stddef.h>

#define N_THREADS 10
#define MAX_IN_CRITICAL 2

const char *const problem_04_name = "Multiplex";

struct Data {
	Semaphore multiplex;
	int count;
};

static void *run(void *ptr) {
	struct Data *d = ptr;
	sema_wait(d->multiplex);
	d->count = 0;
	delay();
	d->count++;
	sema_signal(d->multiplex);
	return NULL;
}

bool problem_04(bool positive) {
	// Initialize the shared data.
	struct Data data = {
		.multiplex = sema_create(MAX_IN_CRITICAL, positive),
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
	bool success = data.count >= 1 && data.count <= MAX_IN_CRITICAL;

	// Clean up.
	sema_destroy(data.multiplex);

	return success;
}

