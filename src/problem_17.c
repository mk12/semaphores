// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stddef.h>

#define N_COOKS 1
#define N_SAVAGES 9
#define N_THREADS (N_COOKS + N_SAVAGES)

#define N_SERVINGS_IN_POT 12
#define N_POT_REFILLS 5
#define N_TOTAL_SERVINGS (N_SERVINGS_IN_POT * N_POT_REFILLS)

const char *const problem_17_name = "Dining savages";

struct Data {
	Semaphore mutex;
	Semaphore empty_pot;
	Semaphore full_pot;
	int servings;
	bool finished;
	struct Buffer log;
};

static void *run_cook(void *ptr) {
	struct Data *d = ptr;

	for (int i = 0; i < N_POT_REFILLS; i++) {
		sema_wait(d->empty_pot);
		d->servings = N_SERVINGS_IN_POT;
		buf_push(&d->log, 'C');
		sema_signal(d->full_pot);
	}

	sema_wait(d->mutex);
	d->finished = true;
	sema_signal(d->mutex);

	return NULL;
}

static void *run_savage(void *ptr) {
	struct Data *d = ptr;

	for (;;) {
		sema_wait(d->mutex);
		if (d->servings == 0) {
			if (d->finished) {
				sema_signal(d->mutex);
				break;
			}
			sema_signal(d->empty_pot);
			sema_wait(d->full_pot);
		}
		d->servings--;
		buf_push(&d->log, 'S');
		sema_signal(d->mutex);
	}

	return NULL;
}

bool problem_17(bool positive) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1, positive),
		.empty_pot = sema_create(0, positive),
		.full_pot = sema_create(0, positive),
		.servings = 0,
		.finished = false
	};
	buf_init(&data.log, N_POT_REFILLS + N_TOTAL_SERVINGS);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_COOKS; i++) {
		pthread_create(&threads[i], NULL, run_cook, &data);
	}
	for (size_t i = N_COOKS; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run_savage, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = true;
	int servings = 0;
	for (size_t i = 0; i < N_POT_REFILLS + N_TOTAL_SERVINGS; i++) {
		unsigned char c = buf_read(&data.log, i);
		if (c == 'C') {
			success &= servings == 0;
			servings = N_SERVINGS_IN_POT;
		} else if (c == 'S') {
			success &= servings > 0;
			servings--;
		} else {
			success = false;
			break;
		}
	}
	success &= servings == 0;
	success &= data.servings == 0;
	success &= data.finished == true;

	// Clean up.
	sema_destroy(data.mutex);
	sema_destroy(data.empty_pot);
	sema_destroy(data.full_pot);
	buf_free(&data.log);

	return success;
}

