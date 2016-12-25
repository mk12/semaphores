// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "semaphore.h"

#include <unistd.h>

static bool semaphores_enabled = true;

void set_semaphores_enabled(bool enabled) {
	semaphores_enabled = enabled;
}

bool are_sempahores_enabled(void) {
	return semaphores_enabled;
}

Semaphore sema_create(long value) {
	return dispatch_semaphore_create(value);
}

void sema_destroy(Semaphore s) {
	dispatch_release(s);
}

void sema_signal(Semaphore s) {
	if (semaphores_enabled) {
		dispatch_semaphore_signal(s);
	}
}

void sema_wait(Semaphore s) {
	if (semaphores_enabled) {
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
