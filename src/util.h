// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef UTIL_H
#define UTIL_H

// Sleeps for a short time. Used to expose concurrency issues and cause
// failures when semaphores are disabled.
void delay(void);

// Increments the given integer, with a delay between reading and writing.
void increment(int *ptr);

// Decrements the given integer, with a delay between reading and writing.
void decrement(int *ptr);

#endif
