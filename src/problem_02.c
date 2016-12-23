// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

const char *const problem_02_name = "Rendezvous";

struct Data {
	Semaphore a_arrived;
	Semaphore b_arrived;
	struct Buffer log;
};

static void *run_a(void *ptr) {
	struct Data *d = ptr;
	delay();
	buf_push(&d->log, 'a');
	sema_signal(d->a_arrived);
	sema_wait(d->b_arrived);
	buf_push(&d->log, 'A');
	return NULL;
}

static void *run_b(void *ptr) {
	struct Data *d = ptr;
	buf_push(&d->log, 'b');
	sema_signal(d->b_arrived);
	sema_wait(d->a_arrived);
	buf_push(&d->log, 'B');
	return NULL;
}

bool problem_02(void) {
	// Initialize the shared data.
	struct Data data = {
		.a_arrived = sema_create(0),
		.b_arrived = sema_create(0)
	};
	buf_init(&data.log, 4);

	// Create and run threads.
	pthread_t thread_a, thread_b;
	pthread_create(&thread_a, NULL, run_a, &data);
	pthread_create(&thread_b, NULL, run_b, &data);
	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);

	// Check for success.
	bool success =
		(buf_range_eq(&data.log, 0, 2, "ab")
			|| buf_range_eq(&data.log, 0, 2, "ba"))
		&& (buf_range_eq(&data.log, 2, 4, "AB")
			|| buf_range_eq(&data.log, 2, 4, "BA"));

	// Clean up.
	sema_destroy(data.a_arrived);
	sema_destroy(data.b_arrived);
	buf_free(&data.log);

	return success;
}
