// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

const char *const problem_1_name = "Signaling";

struct Data {
	Semaphore sem;
	struct Buffer buf;
};

static void *run_a(void *ptr) {
	struct Data *d = ptr;
	delay();
	buf_push(&d->buf, 'A');
	sema_signal(d->sem);
	return NULL;
}

static void *run_b(void *ptr) {
	struct Data *d = ptr;
	sema_wait(d->sem);
	buf_push(&d->buf, 'B');
	return NULL;
}

bool problem_1(void) {
	// Initialize the shared data.
	struct Data data = { .sem = sema_create(0) };
	buf_init(&data.buf, 2);

	// Create and run threads.
	pthread_t thread_a, thread_b;
	pthread_create(&thread_a, NULL, run_a, &data);
	pthread_create(&thread_b, NULL, run_b, &data);
	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);

	// Check for success.
	bool success = buf_eq(&data.buf, "AB");

	// Clean up.
	buf_free(&data.buf);
	sema_destroy(data.sem);

	return success;
}
