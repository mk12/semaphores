// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "prototypes.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

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

// 3.4: Mutex (mutual exclusion)
bool solution_3(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
		.count = 0
	};

	// Create and run threads.
	pthread_t thread_a, thread_b;
	pthread_create(&thread_a, NULL, run, &data);
	pthread_create(&thread_b, NULL, run, &data);
	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);

	// Check for success.
	bool success = data.count == 2;

	// Clean up.
	sema_destroy(data.mutex);

	return success;
}

