// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

#define N_THREADS 10

const char *const problem_5_name = "Barrier"; 

struct Data {
	Semaphore mutex;
	Semaphore barrier;
	int count;
	struct Buffer buf;
};

static void *run(void *ptr) {
	struct Data *d = ptr;

	buf_push(&d->buf, '1');

	sema_wait(d->mutex);
	d->count++;
	sema_signal(d->mutex);
	if (d->count == N_THREADS) {
		sema_signal(d->barrier);
	}
	sema_wait(d->barrier);
	sema_signal(d->barrier);

	buf_push(&d->buf, '2');

	return NULL;
}

bool problem_5(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
		.barrier = sema_create(0),
		.count = 0
	};
	buf_init(&data.buf, N_THREADS * 2);

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
		success &= buf_read(&data.buf, i) == '1';
	}
	for (size_t i = N_THREADS; i < N_THREADS * 2; i++) {
		success &= buf_read(&data.buf, i) == '2';
	}

	// Clean up.
	buf_free(&data.buf);
	sema_destroy(data.mutex);
	sema_destroy(data.barrier);

	return success;
}

