// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "problems.h"
#include "semaphore.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The usage message for the program.
static const char *const usage_message = "usage: semaphores [PROBLEM_NUMBER]\n";

// A string of dots used for padding.
static const char *const padding_dots = "....................";

// Returns true if there is an exercise problem numbered 'n'.
static bool in_range(int n) {
	return n >= 1 && n <= N_PROBLEMS;
}

// Returns a status string for success (true) or failure (false).
static const char* status_str(bool success) {
	return success ? "ok" : "FAIL";
}

// Tests the exercise problem numbered 'n', printing the outcome.
static bool test(int n) {
	ProblemFn function = get_problem_function(n);
	set_semaphores_enabled(false);
	bool success_off = function();
	set_semaphores_enabled(true);
	bool success_on = function();

	const char *name = get_problem_name(n);
	const char *pad = padding_dots + strlen(name);
	const char *msg = status_str(success_on);
	printf("%02d. %s %s %s", n, name, pad, msg);
	if (success_off) {
		printf(" [WARNING: ok without]");
	}
	putchar('\n');
	return success_on;
}

// Tests all exercise problems, printing the individual outcomes and a summary.
static bool test_all(void) {
	int passes = 0;
	for (int i = 1; i <= N_PROBLEMS; i++) {
		passes += test(i);
	}
	int fails = N_PROBLEMS - passes;
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
		if (!in_range(n)) {
			fprintf(stderr, "semaphores: %s: out of bounds\n", argv[1]);
			return 1;
		}
		return test(n);
	default:
		fputs(usage_message, stderr);
		return 1;
	};
}
