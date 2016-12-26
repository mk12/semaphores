// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef TEST_H
#define TEST_H

#include <stdbool.h>

// Integer constant that designates all problems rather than just one.
#define ALL_PROBLEMS 0

// Parameters for testing solutions to exercise problems.
struct Parameters {
	int problem;       // problem number or ALL_PROBLEMS
	int pos_iters;     // number of iterations for the positive case
	int neg_iters;     // number of iterations for the negative case
	int jobs;          // Number of parallel jobs to run
	bool interactive;  // use interactive mode (updates in alternate screen)
};

// Runs tests according to the parameters. Returns true on success.
bool run_tests(const struct Parameters *params);

#endif
