// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "semaphore.h"

Semaphore sema_create(long value) {
	return dispatch_semaphore_create(value);
}

void sema_destroy(Semaphore s) {
	dispatch_release(s);
}

// Increments the semaphore, possibly waking up a thread.
void sema_signal(Semaphore s) {
	dispatch_semaphore_signal(s);
}

// Decrements the semaphore, and blocks if it becomes negative.
void sema_wait(Semaphore s) {
	dispatch_semaphore_wait(s, DISPATCH_TIME_FOREVER);
}
