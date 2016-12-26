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
#define MAX_ITERS 10000
#define MAX_JOBS 64

// Helper macros for stringification.
#define S_(x) #x
#define S(x) S_(x)

// The usage message for the program.
static const char *const usage_message =
	"usage: semaphores [options]\n"
	"\n"
	"  Default\n"
	"    semaphores -p " S(DEFAULT_POS_ITERS) " -n " S(DEFAULT_NEG_ITERS) " -j "
		S(DEFAULT_JOBS) "\n"
	"\n"
	"  Test options\n"
	"    -t N  Test only problem N (1 to " S(N_PROBLEMS) "), not all problems\n"
	"    -p N  Test success with semaphores (positive case), N iterations\n"
	"    -n N  Test failure without semaphores (negative case), N iterations\n"
	"    Use -p0 to disable positive tests and -n0 to disable negative tests\n"
	"\n"
	"  Other options\n"
	"    -j N  Run N jobs in parallel\n"
	"    -i    Use interactive mode (display updates in alternate screen)\n"
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
		.problem = ALL_PROBLEMS,
		.pos_iters = DEFAULT_POS_ITERS,
		.neg_iters = DEFAULT_NEG_ITERS,
		.jobs = DEFAULT_JOBS,
		.interactive = false
	};

	// Get command line options.
	int c;
	extern char *optarg;
	extern int optind, optopt;
	while ((c = getopt(argc, argv, "t:p:n:fih")) != -1) {
		switch (c) {
		case 't':
			if (!parse_int(&params.problem, optarg)) {
				return 1;
			}
			if (!problem_in_range(params.problem)) {
				printf_error("%s: out of range (should be between %d and %d)",
						optarg, 1, N_PROBLEMS);
				return 1;
			}
			break;
		case 'p':
			if (!parse_int(&params.pos_iters, optarg)) {
				return 1;
			}
			if (params.pos_iters < 0) {
				printf_error("%s: iterations must be nonnegative", optarg);
				return 1;
			}
			if (params.pos_iters > MAX_ITERS) {
				printf_error("%s: iterations too large (maximum %d)",
						optarg, MAX_ITERS);
				return 1;
			}
			break;
		case 'n':
			if (!parse_int(&params.neg_iters, optarg)) {
				return 1;
			}
			if (params.neg_iters < 0) {
				printf_error("%s: iterations must be nonnegative", optarg);
				return 1;
			}
			if (params.neg_iters > MAX_ITERS) {
				printf_error("%s: iterations too large (maximum %d)",
						optarg, MAX_ITERS);
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
		case 'i':
			params.interactive = true;
			break;
		case 'h':
			fputs(usage_message, stdout);
			return 0;
		case '?':
			fputs(usage_message, stderr);
			return 1;
		}
	}
	// Check for invalid combiantions of options.
	if (params.problem != ALL_PROBLEMS && params.interactive) {
		printf_error("interactive mode cannot be used for single tests");
		return 1;
	}
	// Make sure all arguments were processed.
	if (optind != argc) {
		fputs(usage_message, stderr);
		return 1;
	}

	return run_tests(&params) ? 0 : 1;
}
