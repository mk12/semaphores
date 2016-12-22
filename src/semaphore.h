// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef SEMA_H
#define SEMA_H

#include <dispatch/dispatch.h>

#include <pthread.h>
#include <stddef.h>

// Use Apple's Grand Central Dispatch semaphore type.
typedef dispatch_semaphore_t Semaphore;

// Creates a semaphore with an initial value.
Semaphore sema_create(long value);

// Destroys the semaphore.
void sema_destroy(Semaphore s);

// Increments the semaphore, possibly waking up a thread.
void sema_signal(Semaphore s);

// Decrements the semaphore, and blocks if it becomes negative.
void sema_wait(Semaphore s);

#endif
