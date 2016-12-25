// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <dispatch/dispatch.h>

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

// Use Apple's Grand Central Dispatch semaphore type.
typedef dispatch_semaphore_t Semaphore;

// Creates a semaphore with an initial value. If 'real_semaphore' is false, then
// it just returns a dummy semaphore, and 'signal' and 'wait' will do nothing.
Semaphore sema_create(long value, bool real_semaphore);

// Destroys the semaphore.
void sema_destroy(Semaphore s);

// Increments the semaphore, possibly waking up a thread.
void sema_signal(Semaphore s);

// Decrements the semaphore, and blocks if it becomes negative.
void sema_wait(Semaphore s);

// Sleeps for a short time. Used to expose concurrency issues and cause
// failures when semaphores are disabled.
void delay(void);

// Increments the given integer, with a delay between reading and writing.
void increment(int *ptr);

// Decrements the given integer, with a delay between reading and writing.
void decrement(int *ptr);

#endif
