// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "prototypes.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

#define N_THREADS 10
#define MAX_IN_CRITICAL 2

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

// 3.5: Multiplex
bool solution_4(void) {
	// Initialize the shared data.
	struct Data data = {
		.multiplex = sema_create(MAX_IN_CRITICAL),
		.count = 0
	};

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (int i = 0; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run, &data);
	}
	for (int i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = data.count >= 1 && data.count <= MAX_IN_CRITICAL;

	// Clean up.
	sema_destroy(data.multiplex);

	return success;
}

