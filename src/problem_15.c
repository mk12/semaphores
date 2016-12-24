// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <pthread.h>
#include <stddef.h>
#include <string.h>

enum Role {
	AGENT,
	PUSHER,
	SMOKER,
	N_ROLES
};

enum Ingredient {
	TOBACCO,
	PAPER,
	MATCH,
	N_INGREDIENTS
};

#define N_THREADS (N_ROLES * N_INGREDIENTS)

const char *const problem_15_name = "Cigarette smokers";

struct Data {
	Semaphore agent;
	Semaphore pusher_mutex;
	Semaphore ingredients[N_INGREDIENTS];
	Semaphore push_ingredients[N_INGREDIENTS];
	Semaphore next_mutex[N_ROLES];
	bool is_ingredient[N_INGREDIENTS];
	size_t next[N_ROLES];
	struct Buffer log;
};

static void *run_agent(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->next_mutex[AGENT]);
	size_t n = d->next[AGENT]++;
	sema_signal(d->next_mutex[AGENT]);

	sema_wait(d->agent);
	delay();
	buf_push(&d->log, 'A');
	buf_push(&d->log, (unsigned char)n);
	for (size_t i = 0; i < N_INGREDIENTS; i++) {
		if (i != n) {
			sema_signal(d->ingredients[i]);
		}
	}

	return NULL;
}

static void *run_pusher(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->next_mutex[PUSHER]);
	size_t n = d->next[PUSHER]++;
	sema_signal(d->next_mutex[PUSHER]);

	for (int count = 0; count < 2; count++) {
		sema_wait(d->ingredients[n]);
		sema_wait(d->pusher_mutex);
		bool pushed = false;
		for (size_t i = 0; i < N_INGREDIENTS; i++) {
			if (i != n && d->is_ingredient[i]) {
				pushed = true;
				d->is_ingredient[i] = false;
				sema_signal(d->push_ingredients[N_INGREDIENTS-n-i]);
			}
		}
		if (!pushed) {
			d->is_ingredient[n] = true;
		}
		sema_signal(d->pusher_mutex);
	}

	return NULL;
}

static void *run_smoker(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->next_mutex[SMOKER]);
	size_t n = d->next[SMOKER]++;
	sema_signal(d->next_mutex[SMOKER]);

	sema_wait(d->push_ingredients[n]);
	sema_signal(d->agent);
	buf_push(&d->log, 'S');
	buf_push(&d->log, (unsigned char)n);

	return NULL;
}

bool problem_15(void) {
	// Initialize the shared data.
	struct Data data = {
		.agent = sema_create(1),
		.pusher_mutex = sema_create(1),
		.is_ingredient = { false },
		.next = { 0 }
	};
	for (size_t i = 0; i < N_INGREDIENTS; i++) {
		data.ingredients[i] = sema_create(0);
		data.push_ingredients[i] = sema_create(0);
	}
	for (size_t i = 0; i < N_ROLES; i++) {
		data.next_mutex[i] = sema_create(1);
	}
	buf_init(&data.log, N_INGREDIENTS * 4);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_THREADS; i += N_ROLES) {
		pthread_create(&threads[i], NULL, run_agent, &data);
		pthread_create(&threads[i + 1], NULL, run_pusher, &data);
		pthread_create(&threads[i + 2], NULL, run_smoker, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = true;
	bool ready[N_INGREDIENTS] = { false };
	int smoked = 0;
	for (size_t i = 0; i < N_INGREDIENTS * 4; i += 2) {
		unsigned char c1 = buf_read(&data.log, i);
		unsigned char c2 = buf_read(&data.log, i + 1);
		if (c2 > N_INGREDIENTS) {
			success = false;
			break;
		}

		if (c1 == 'A') {
			success &= !ready[c2];
			ready[c2] = true;
		} else if (c1 == 'S') {
			success &= ready[c2];
			ready[c2] = false;
			smoked++;
		} else {
			success = false;
			break;
		}
	}
	success &= smoked == N_INGREDIENTS;

	// Clean up.
	sema_destroy(data.agent);
	for (size_t i = 0; i < N_INGREDIENTS; i++) {
		sema_destroy(data.ingredients[i]);
		sema_destroy(data.push_ingredients[i]);
	}
	for (size_t i = 0; i < N_ROLES; i++) {
		sema_destroy(data.next_mutex[i]);
	}
	buf_free(&data.log);

	return success;
}

