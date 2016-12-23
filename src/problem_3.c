// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

#define N_THREADS 10

const char *const problem_3_name = "Mutex"; 

struct Data {
	Semaphore mutex;
	int count;
};

static void *run(void *ptr) {
	struct Data *d = ptr;
	sema_wait(d->mutex);
	increment(&d->count);
	sema_signal(d->mutex);
	return NULL;
}

bool problem_3(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
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
	bool success = data.count == N_THREADS;

	// Clean up.
	sema_destroy(data.mutex);

	return success;
}

