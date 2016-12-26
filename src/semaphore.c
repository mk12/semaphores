// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "semaphore.h"

#include <unistd.h>

Semaphore sema_create(long value, bool real_semaphore) {
	if (real_semaphore) {
		return dispatch_semaphore_create(value);
	}
	return 0;
}

void sema_destroy(Semaphore s) {
	if (s != 0) {
		dispatch_release(s);
	}
}

void sema_signal(Semaphore s) {
	if (s != 0) {
		dispatch_semaphore_signal(s);
	}
}

void sema_wait(Semaphore s) {
	if (s != 0) {
		dispatch_semaphore_wait(s, DISPATCH_TIME_FOREVER);
	}
}

void delay(void) {
	usleep(200);
}

void increment(int *ptr) {
	int val = *ptr;
	delay();
	*ptr = val + 1;
}

void decrement(int *ptr) {
	int val = *ptr;
	delay();
	*ptr = val - 1;
}
