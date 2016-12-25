// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef TEST_H
#define TEST_H

#include <stdbool.h>

#define ALL_PROBLEMS 0

// Parameters for testing solutions to exercise problems.
struct Parameters {
	int problem;         // problem number or ALL_PROBLEMS
	int pos_iterations;  // number of iterations for the positive case
	int neg_iterations;  // number of iterations for the negative case
	int jobs;            // number of parallel jobs to run
};

// Runs tests according to the parameters. Returns true on success.
bool run_tests(const struct Parameters *params);

#endif
