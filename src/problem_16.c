// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stddef.h>

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

const char *const problem_16_name = "Generalized CS";

struct Data {
	Semaphore pusher_mutex;
	Semaphore ingredients[N_INGREDIENTS];
	Semaphore push_ingredients[N_INGREDIENTS];
	Semaphore next_mutex[N_ROLES];
	int ingredient_counts[N_INGREDIENTS];
	bool already_pushed[N_INGREDIENTS];
	size_t next[N_ROLES];
	struct Buffer log;
};

static void *run_agent(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->next_mutex[AGENT]);
	size_t n = d->next[AGENT]++;
	sema_signal(d->next_mutex[AGENT]);

	delay();
	buf_push2(&d->log, 'A', (unsigned char)n);
	for (size_t i = 0; i < N_INGREDIENTS; i++) {
		if (i != n) {
			delay();
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
			size_t other = N_INGREDIENTS - n - i;
			if (i != n && d->ingredient_counts[i] > 0
					&& !d->already_pushed[other]) {
				d->ingredient_counts[i]--;
				d->already_pushed[other] = true;
				sema_signal(d->push_ingredients[other]);
				pushed = true;
				break;
			}
		}
		if (!pushed) {
			d->ingredient_counts[n]++;
		}
		sema_signal(d->pusher_mutex);
	}

	return NULL;
}

static void *run_smoker(void *ptr) {
	struct Data *d = ptr;

	delay();
	sema_wait(d->next_mutex[SMOKER]);
	size_t n = d->next[SMOKER]++;
	sema_signal(d->next_mutex[SMOKER]);

	sema_wait(d->push_ingredients[n]);
	buf_push2(&d->log, 'S', (unsigned char)n);

	return NULL;
}

bool problem_16(bool positive) {
	// Initialize the shared data.
	struct Data data = {
		.pusher_mutex = sema_create(1, positive),
		.ingredient_counts = { 0 },
		.already_pushed = { false },
		.next = { 0 }
	};
	for (size_t i = 0; i < N_INGREDIENTS; i++) {
		data.ingredients[i] = sema_create(0, positive);
		data.push_ingredients[i] = sema_create(0, positive);
	}
	for (size_t i = 0; i < N_ROLES; i++) {
		data.next_mutex[i] = sema_create(1, positive);
	}
	buf_init(&data.log, N_INGREDIENTS * 4);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_INGREDIENTS; i++) {
		pthread_create(&threads[i], NULL, run_agent, &data);
		pthread_create(&threads[N_INGREDIENTS+i], NULL, run_smoker, &data);
		pthread_create(&threads[N_INGREDIENTS*2+i], NULL, run_pusher, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = true;
	int counts[N_INGREDIENTS] = { 0 };
	int smoked = 0;
	for (size_t i = 0; i < N_INGREDIENTS * 4; i += 2) {
		unsigned char c1 = buf_read(&data.log, i);
		unsigned char c2 = buf_read(&data.log, i + 1);
		if (c2 > N_INGREDIENTS) {
			success = false;
			break;
		}

		size_t other1 = (size_t)(c2 + 1) % N_INGREDIENTS;
		size_t other2 = (size_t)(c2 + 2) % N_INGREDIENTS;
		if (c1 == 'A') {
			counts[other1]++;
			counts[other2]++;
		} else if (c1 == 'S') {
			success &= counts[other1] > 0;
			success &= counts[other2] > 0;
			counts[other1]--;
			counts[other2]--;
			smoked++;
		} else {
			success = false;
			break;
		}
	}
	for (size_t i = 0; i < N_INGREDIENTS; i++) {
		success &= counts[i] == 0;
		success &= data.ingredient_counts[i] == 0;
		success &= data.already_pushed[i] == true;
		success &= data.next[i] == N_INGREDIENTS;
	}
	success &= smoked == N_INGREDIENTS;

	// Clean up.
	sema_destroy(data.pusher_mutex);
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
