// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <dispatch/dispatch.h>

#include <pthread.h>
#include <stddef.h>

// Use Apple's Grand Central Dispatch semaphore type.
typedef dispatch_semaphore_t Semaphore;

// Enables or disables semaphores. If 'enabled' is true, then 'sema_signal' and
// 'sema_wait' will behave normally. If it is false, then they will be no-ops.
void set_semaphores_enabled(bool enabled);

// Returns true if semaphores are enabled.
bool are_sempahores_enabled(void);

// Creates a semaphore with an initial value.
Semaphore sema_create(long value);

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
