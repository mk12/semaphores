// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>

#define N_CHAIRS 5
#define N_CUSTOMERS 10
#define N_THREADS (N_CUSTOMERS + 1)

const char *const problem_18_name = "Barbershop problem";

struct Data {
	Semaphore mutex;
	Semaphore barber_ready;
	Semaphore customer_ready;
	Semaphore barber_done;
	Semaphore customer_done;
	atomic_uchar next_number;
	atomic_int customers_left;
	int n_waiting;
	unsigned char current_customer;
	struct Buffer log;
};

static void *run_barber(void *ptr) {
	struct Data *d = ptr;

	for (;;) {
		sema_wait(d->mutex);
		if (d->customers_left == 0) {
			sema_signal(d->mutex);
			break;
		}
		sema_signal(d->mutex);

		sema_wait(d->customer_ready);
		sema_signal(d->barber_ready);
		buf_push2(&d->log, 'B', d->current_customer);
		sema_wait(d->customer_done);
		sema_signal(d->barber_done);
	}

	return NULL;
}

static void *run_customer(void *ptr) {
	struct Data *d = ptr;

	unsigned char n = d->next_number++;

	sema_wait(d->mutex);
	if (d->n_waiting < N_CHAIRS) {
		d->n_waiting++;
		sema_signal(d->mutex);

		sema_signal(d->customer_ready);
		d->current_customer = n;
		sema_wait(d->barber_ready);

		sema_wait(d->mutex);
		d->n_waiting--;
		sema_signal(d->mutex);

		buf_push2(&d->log, 'C', n);

		sema_signal(d->customer_done);
		sema_wait(d->barber_done);

	} else {
		buf_push2(&d->log, 'L', n);
		sema_signal(d->mutex);
	}

	d->customers_left--;
	return NULL;
}

bool problem_18(bool positive) {
	// Initialize the shared data.
	struct Data data = {
		.mutex = sema_create(1, positive),
		.barber_ready = sema_create(0, positive),
		.customer_ready = sema_create(0, positive),
		.barber_done = sema_create(0, positive),
		.customer_done = sema_create(0, positive),
		.next_number = 0,
		.customers_left = N_CUSTOMERS,
		.current_customer = 0
	};
	buf_init(&data.log, N_CUSTOMERS * 4);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	pthread_create(&threads[0], NULL, run_barber, &data);
	for (size_t i = 1; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run_customer, &data);
	}

	// Check for success.
	bool success = true;
	size_t haircuts = 0;
	enum { NONE, LEAVE, READY, CUT } state[N_CUSTOMERS];
	for (size_t i = 0; i < data.log.len; i += 2) {
		unsigned char c1 = buf_read(&data.log, i);
		unsigned char c2 = buf_read(&data.log, i + 1);
		if (c2 >= N_CUSTOMERS) {
			success = false;
			break;
		}

		if (c1 == 'L') {
			success &= state[c2] == NONE;
			state[c2] = LEAVE;
		} else if (c1 == 'C') {
			success &= state[c2] == NONE;
			state[c2] = READY;
		} else if (c1 == 'B') {
			success &= state[c2] == READY;
			state[c2] = CUT;
			haircuts++;
		} else {
			success = false;
			break;
		}
	}
	success &= haircuts <= N_CUSTOMERS;
	success &= data.n_waiting == 0;

	// Clean up.
	// sema_destroy(data.mutex);
	// sema_destroy(data.barber_ready);
	// sema_destroy(data.customer_ready);
	// sema_destroy(data.barber_done);
	// sema_destroy(data.customer_done);
	buf_free(&data.log);

	return success;
}
