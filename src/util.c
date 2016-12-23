// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "util.h"

#include <unistd.h>

void delay(void) {
	usleep(200);
}

void increment(int *ptr) {
	int val = *ptr;
	delay();
	*ptr = val + 1;
}
