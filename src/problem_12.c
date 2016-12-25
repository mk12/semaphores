// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "buffer.h"
#include "problems.h"
#include "semaphore.h"

#include <pthread.h>
#include <stddef.h>

#define N_READERS 6
#define N_WRITERS 4
#define N_THREADS (N_READERS + N_WRITERS)

const char *const problem_12_name = "Writer-priority R-W";

struct Data {
	Semaphore reader_mutex;
	Semaphore writer_mutex;
	Semaphore no_readers;
	Semaphore no_writers;
	int readers;
	int writers;
	int value;
	struct Buffer log;
};

static void *run_reader(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->no_readers);
	sema_wait(d->reader_mutex);
	if (d->readers == 0) {
		sema_wait(d->no_writers);
	}
	increment(&d->readers);
	sema_signal(d->reader_mutex);
	sema_signal(d->no_readers);

	buf_push(&d->log, 'R');
	buf_push(&d->log, (unsigned char)d->value);

	sema_wait(d->reader_mutex);
	decrement(&d->readers);
	if (d->readers == 0) {
		sema_signal(d->no_writers);
	}
	sema_signal(d->reader_mutex);

	return NULL;
}

static void *run_writer(void *ptr) {
	struct Data *d = ptr;

	sema_wait(d->writer_mutex);
	if (d->writers == 0) {
		sema_wait(d->no_readers);
	}
	increment(&d->writers);
	sema_signal(d->writer_mutex);

	sema_wait(d->no_writers);
	increment(&d->value);
	buf_push(&d->log, 'W');
	buf_push(&d->log, (unsigned char)d->value);
	sema_signal(d->no_writers);

	sema_wait(d->writer_mutex);
	decrement(&d->writers);
	if (d->writers == 0) {
		sema_signal(d->no_readers);
	}
	sema_signal(d->writer_mutex);

	return NULL;
}

bool problem_12(void) {
	// Initialize the shared data.
	struct Data data = {
		.reader_mutex = sema_create(1),
		.writer_mutex = sema_create(1),
		.no_readers = sema_create(1),
		.no_writers = sema_create(1),
		.readers = 0,
		.writers = 0,
		.value = 0
	};
	buf_init(&data.log, N_THREADS * 2);

	// Create and run threads.
	pthread_t threads[N_THREADS];
	for (size_t i = 0; i < N_READERS; i++) {
		pthread_create(&threads[i], NULL, run_reader, &data);
	}
	for (size_t i = N_READERS; i < N_THREADS; i++) {
		pthread_create(&threads[i], NULL, run_writer, &data);
	}
	for (size_t i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// Check for success.
	bool success = true;
	unsigned char value = 0;
	for (size_t i = 0; i < N_THREADS * 2; i += 2) {
		unsigned char c1 = buf_read(&data.log, i);
		unsigned char c2 = buf_read(&data.log, i + 1);

		if (c1 == 'R') {
			success &= c2 == value;
		} else if (c1 == 'W') {
			value++;
			success &= c2 == value;
		} else {
			success = false;
			break;
		}
	}
	success &= value == N_WRITERS;

	// Clean up.
	sema_destroy(data.reader_mutex);
	sema_destroy(data.writer_mutex);
	sema_destroy(data.no_readers);
	sema_destroy(data.no_writers);
	buf_free(&data.log);

	return success;
}

