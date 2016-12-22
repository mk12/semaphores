// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "semaphore.h"

// Set to 1 to make all "signals" and "waits" no-ops. This is useful to make
// sure everything fails (most of the time) without semaphores.
#define DISABLE_SEMAPHORES 0

Semaphore sema_create(long value) {
	return dispatch_semaphore_create(value);
}

void sema_destroy(Semaphore s) {
	dispatch_release(s);
}

// Increments the semaphore, possibly waking up a thread.
void sema_signal(Semaphore s) {
#if DISABLE_SEMAPHORES
	(void)s;
#else
	dispatch_semaphore_signal(s);
#endif
}

// Decrements the semaphore, and blocks if it becomes negative.
void sema_wait(Semaphore s) {
#if DISABLE_SEMAPHORES
	(void)s;
#else
	dispatch_semaphore_wait(s, DISPATCH_TIME_FOREVER);
#endif
}
