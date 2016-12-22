// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "prototypes.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Number of times to repeat tests (once is not enough, since concurrency issues
// will usually cause intermittent failure).
#define ITERATIONS 25

// The usage message for the program.
static const char *const usage_message = "usage: semaphores [TEST_NUMBER]\n";

// A function that executes an exercise solution and returns true on success.
typedef bool (*SolutionFn)(void);

// Function pointers to the solution functions.
#define N_SOLUTIONS 2
static const SolutionFn solution_fns[N_SOLUTIONS] = {
	solution_1, solution_2
};

// Returns true if there is a solution function numbered 'n'.
static bool has_solution(int n) {
	return n >= 1 && n <= N_SOLUTIONS;
}

// Returns a status string for success (true) or failure (false).
static const char* status_str(bool success) {
	return success ? "ok" : "FAIL";
}

// Tests the exercise solution numbered 'n', printing the outcome.
static bool test(int n) {
	int index = n - 1;
	SolutionFn function = solution_fns[index];
	bool success = true;
	for (int i = 0; i < ITERATIONS; i++) {
		if (!function()) {
			success = false;
			break;
		}
	}
	const char* msg = status_str(success);
	printf("%02d ... %4s\n", n, msg);
	return success;
}

// Tests all exercise solutions, printing the individual outcomes and a summary.
static bool test_all(void) {
	int passes = 0;
	for (int i = 1; i <= N_SOLUTIONS; i++) {
		passes += test(i);
	}
	int fails = N_SOLUTIONS - passes;
	bool success = fails == 0;
	const char* msg = status_str(success);
	printf("%s. %d passed; %d failed\n", msg, passes, fails);
	return success;
}

int main(int argc, char **argv) {
	switch (argc) {
	case 1:
		return test_all();
	case 2:
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			fputs(usage_message, stdout);
			return 0;
		}
		char *end;
		int n = (int)strtol(argv[1], &end, 0);
		if (*end != '\0') {
			fprintf(stderr, "semaphores: %s: not a number\n", argv[1]);
			return 1;
		}
		if (!has_solution(n)) {
			fprintf(stderr, "semaphores: %s: out of bounds\n", argv[1]);
			return 1;
		}
		return test(n);
	default:
		fputs(usage_message, stderr);
		return 1;
	};
}
