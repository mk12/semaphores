// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stddef.h>
#include <string.h>

#define N_THREADS 10
#define N_FORKS 5

#define LEFT(i) (((i) + 1) % N_FORKS)
#define RIGHT(i) (((i) - 1 + N_FORKS) % N_FORKS)

const char *const problem_14_name = "Dining philosophers";

struct Data {
	Semaphore mutex;
	Semaphore multiplex;
	Semaphore forks[N_FORKS];
	int seats[N_FORKS];
	struct Buffer log;
};

static void *run(void *ptr) {
	struct Data *d = ptr;
	int seats[N_FORKS];

	for (size_t i = 0; i < N_FORKS; i++) {
		sema_wait(d->multiplex);
		sema_wait(d->forks[LEFT(i)]);
		sema_wait(d->forks[RIGHT(i)]);

		sema_wait(d->mutex);
		d->seats[i]++;
		memcpy(seats, d->seats, N_FORKS * sizeof *seats);
		sema_signal(d->mutex);

		delay();
		int count = 0;
		unsigned char result = 'Y';
		for (size_t j = 0; j < N_FORKS; j++) {
			int n = seats[j];
			count += n;
			if (n > 1) {
				result = 'N';
				break;
			}
		}
		if (count > N_FORKS / 2) {
			result = 'N';
		}
		buf_push(&d->log, result);

		sema_wait(d->mutex);
		d->seats[i]--;
		sema_signal(d->mutex);

		sema_signal(d->forks[LEFT(i)]);
		sema_signal(d->forks[RIGHT(i)]);
		sema_signal(d->multiplex);
	}

	return NULL;
}

bool problem_14(void) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1),
		.multiplex = sema_create(N_FORKS - 1),
		.seats = { 0 }
	};
	for (size_t i = 0; i < N_FORKS; i++) {
		data.forks[i] = sema_create(1);
	}
	buf_init(&data.log, N_THREADS * N_FORKS);

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
	for (size_t i = 0; i < N_THREADS * N_FORKS; i++) {
		success &= buf_read(&data.log, i) == 'Y';
	}
	for (size_t i = 0; i < N_FORKS; i++) {
		success &= data.seats[i] == 0;
	}

	// Clean up.
	sema_destroy(data.mutex);
	sema_destroy(data.multiplex);
	for (int i = 0; i < N_FORKS; i++) {
		sema_destroy(data.forks[i]);
	}
	buf_free(&data.log);

	return success;
}

