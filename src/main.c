// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "problems.h"
#include "test.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Default values for some parameters.
#define DEFAULT_POS_ITERS 5
#define DEFAULT_NEG_ITERS 1
#define DEFAULT_JOBS 1

// Maximum values for some parameters.
#define MAX_ITERATIONS 10000
#define MAX_JOBS 64

// Helper macros for stringification.
#define S_(x) #x
#define S(x) S_(x)

// The usage message for the program.
static const char *const usage_message =
	"usage: semaphores [options]\n"
	"\n"
	"  Default\n"
	"    semaphores -t all -p " S(DEFAULT_POS_ITERS) " -n " S(DEFAULT_NEG_ITERS)
		" -j " S(DEFAULT_JOBS) "\n"
	"\n"
	"  Problem number\n"
	"    -t all  Test solutions to all problems\n"
	"    -t N    Test solution to problem N (between 1 and " S(N_PROBLEMS) ")\n"
	"\n"
	"  Test iterations\n"
	"    -p N  Test success with semaphores (positive case), N iterations\n"
	"    -n N  Test failure without semaphores (negative case), N iterations\n"
	"    Use -p0 or -n0 to disable positive/negative tests\n"
	"\n"
	"  Other settings\n"
	"    -j N  Run N jobs in parallel\n"
	"\n";

#undef S
#undef S_

int main(int argc, char **argv) {
	setup_util(argv[0]);
	if (argc == 2 && strcmp(argv[1], "--help") == 0) {
		fputs(usage_message, stdout);
		return 0;
	}

	// Initialize the default parameters.
	struct Parameters params = {
		.problem = 0,
		.pos_iterations = DEFAULT_POS_ITERS,
		.neg_iterations = DEFAULT_NEG_ITERS,
		.jobs = DEFAULT_JOBS
	};

	// Get command line options.
	int c;
	extern char *optarg;
	extern int optind, optopt;
	while ((c = getopt(argc, argv, "ht:p:n:j:")) != -1) {
		switch (c) {
		case 'h':
			fputs(usage_message, stdout);
			return 0;
		case 't':
			if (strcmp(optarg, "all") == 0 || strcmp(optarg, "=all") == 0) {
				params.problem = 0;
				break;
			}
			if (!parse_int(&params.problem, optarg)) {
				return 1;
			}
			if (!problem_in_range(params.problem)) {
				printf_error("%s: out of range (should be between %d and %d)",
						optarg, 1, N_PROBLEMS);
			}
			break;
		case 'p':
			if (!parse_int(&params.pos_iterations, optarg)) {
				return 1;
			}
			if (params.pos_iterations < 0) {
				printf_error("%s: iterations must be nonnegative", optarg);
				return 1;
			}
			if (params.pos_iterations > MAX_ITERATIONS) {
				printf_error("%s: iterations too large (maximum %d)",
						optarg, MAX_ITERATIONS);
				return 1;
			}
			break;
		case 'n':
			if (!parse_int(&params.neg_iterations, optarg)) {
				return 1;
			}
			if (params.neg_iterations < 0) {
				printf_error("%s: iterations must be nonnegative", optarg);
				return 1;
			}
			if (params.neg_iterations > MAX_ITERATIONS) {
				printf_error("%s: iterations too large (maximum %d)",
						optarg, MAX_ITERATIONS);
				return 1;
			}
			break;
		case 'j':
			if (!parse_int(&params.jobs, optarg)) {
				return 1;
			}
			if (params.jobs <= 0) {
				printf_error("%s: jobs must be positive", optarg);
				return 1;
			}
			if (params.jobs > MAX_JOBS) {
				printf_error("%s: too many jobs (maximum %d)",
						optarg, MAX_JOBS);
				return 1;
			}
			break;
		case '?':
			fputs(usage_message, stderr);
			return 1;
		}
	}
	// Make sure all arguments were processed.
	if (optind != argc) {
		fputs(usage_message, stderr);
		return 1;
	}

	return run_tests(&params) ? 0 : 1;
}
