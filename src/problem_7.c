// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>

#define N_THREADS 10

const char *const problem_7_name = "Exclusive queue";

struct Data {
	Semaphore mutex;
	Semaphore rendezvous;
	Semaphore leader_queue;
	Semaphore follower_queue;
	int leaders;
	int followers;
	struct Buffer buf;
};

static void *run_leader(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->mutex);
	if (d->followers > 0) {
		d->followers--;
		sema_signal(d->follower_queue);
	} else {
		d->leaders++;
		sema_signal(d->mutex);
		sema_wait(d->leader_queue);
	}
	buf_push(&d->buf, 'L');
	sema_wait(d->rendezvous);
	sema_signal(d->mutex);

	return NULL;
}

static void *run_follower(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->mutex);
	if (d->leaders > 0) {
		d->leaders--;
		sema_signal(d->leader_queue);
	} else {
		d->followers++;
		sema_signal(d->mutex);
		sema_wait(d->follower_queue);
	}
	buf_push(&d->buf, 'F');
	sema_signal(d->rendezvous);

	return NULL;
}

bool problem_7(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
		.rendezvous = sema_create(0),
		.leader_queue = sema_create(0),
		.follower_queue = sema_create(0),
		.leaders = 0,
		.followers = 0
	};
	buf_init(&data.buf, N_THREADS);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_THREADS / 2; i++) {
		pthread_create(&threads[i], NULL, run_leader, &data);
	}
	for (size_t i = N_THREADS / 2; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run_follower, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = true;
	for (size_t i = 0; i < N_THREADS; i += 2) {
		unsigned char c1 = buf_read(&data.buf, i);
		unsigned char c2 = buf_read(&data.buf, i + 1);
		success &= (c1 == 'L' && c2 == 'F') || (c1 == 'F' && c2 == 'L');
	}

	// Clean up.
	buf_free(&data.buf);
	sema_destroy(data.mutex);
	sema_destroy(data.rendezvous);
	sema_destroy(data.leader_queue);
	sema_destroy(data.follower_queue);

	return success;
}

