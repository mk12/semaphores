// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "test.h"

#include "problems.h"
#include "util.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Delay between result updates for interactive mode, in milliseconds.
#define UPDATE_DELAY_MS 60

// Character to press to quit interactive mode.
#define QUIT_CHARACTER 'q'

// Converts a zero-based index to a one-based problem number.
#define INDEX_TO_PROBLEM(i) ((int)((i) + 1))

// There are four possible states for a test.
#define N_STATES 4
enum State {
	PENDING,
	PASS,
	FAIL,
	SKIP
};

// The result of testing a solution to an exercise problem.
struct Result {
	enum State pos_state : 4;  // state of test with semaphores enabled
	enum State neg_state : 4;  // state of test with semaphores disabled
};

// A Task specifies a range of exercise problems to test. Performing a Task
// consists of executing the solution programs for the problems beginning at
// 'start' (zero-based, not one-based), and storing the results in 'results',
// filling from 'results[start]' to 'results[end-1]'. Finally, the 'tasks_left'
// counter should be decremented.
struct Task {
	unsigned short start;       // start index in 'result' (zero-based)
	unsigned short end;         // end index in 'result' (exclusive)
	unsigned short pos_iters;   // iterations for the positive case
	unsigned short neg_iters;   // iterations for the negative case
	struct Result *results;     // array of all results
	atomic_ushort *tasks_left;  // pointer to task countdown
};

// A string of dots used for padding.
static const char *const padding_dots = "......................";

// Returns a status string for the given state.
static const char* state_str(enum State state) {
	switch (state) {
	case PENDING:
		return " -- ";
	case PASS:
		return " ok ";
	case FAIL:
		return "FAIL";
	case SKIP:
		return "skip";
	}
}

// Returns true if the result is good (meaning we can exit with exit status 0).
static bool result_good(struct Result result) {
	return result.pos_state != FAIL && result.neg_state != FAIL;
}

// Returns true if all results are good in the array of size N_PROBLEMS.
static bool all_results_good(struct Result *results) {
	for (size_t i = 0; i < N_PROBLEMS; i++) {
		if (!result_good(results[i])) {
			return false;
		}
	}
	return true;
}

// Prints the test result for the given problem on one line.
static void print_result(int problem, struct Result result) {
	const char *name = get_problem_name(problem);
	const char *pad = padding_dots + strlen(name);
	const char *pos_msg = state_str(result.pos_state);
	const char *neg_msg = state_str(result.neg_state);
	printf("%02d. %s %s %s %s\n", problem, name, pad, pos_msg, neg_msg);
}

// Prints a header for the test results.
static void print_header(void) {
	printf("No. Problem name            Pos. Neg.\n");
	printf("=== ======================= ==== ====\n");
}

// Prints a summary of the test results.
static void print_summary(struct Result *results) {
	int counts[N_STATES] = { 0 };
	for (size_t i = 0; i < N_PROBLEMS; i++) {
		counts[results[i].pos_state]++;
		counts[results[i].neg_state]++;
	}
	printf("Summary: %d passed, %d failed, %d skipped.\n",
			counts[PASS], counts[FAIL], counts[SKIP]);
}

// Prints all results from 'results', an array of length N_PROBLEMS, with a
// header at the top and a summary at the bottom.
static void print_all_results(struct Result *results) {
	print_header();
	for (size_t i = 0; i < N_PROBLEMS; i++) {
		print_result(INDEX_TO_PROBLEM(i), results[i]);
	}
	print_summary(results);
}

// Clears the screen and then calls 'print_all_results'.
static void update_all_results(struct Result *results) {
	clear_screen();
	print_all_results(results);
}

// Tests the given problem function 'iters' times using the positive case
// (success expected with semaphores enabled). Returns the resulting state.
static enum State test_positive(ProblemFn function, int iters) {
	if (iters == 0) {
		return SKIP;
	}
	for (int i = 0; i < iters; i++) {
		if (!function(true)) {
			return FAIL;
		}
	}
	return PASS;
}

// Tests the given problem function 'iters' times using the negative case
// (failure expected with semaphores disabled). Returns the resulting state.
static enum State test_negative(ProblemFn function, int iters) {
	if (iters == 0) {
		return SKIP;
	}
	for (int i = 0; i < iters; i++) {
		if (function(false)) {
			return FAIL;
		}
	}
	return PASS;
}

// Tests the given exercise problem, with 'pos_iters' iterations for the
// positive case and 'neg_iters' iterations for the negative case. Stores the
// result in 'out'.
static void test_problem(
		struct Result *out, int problem, int pos_iters, int neg_iters) {
	ProblemFn function = get_problem_function(problem);
	out->pos_state = test_positive(function, pos_iters);
	out->neg_state = test_negative(function, neg_iters);
}

// Runs the tests specified by 'params' sequentially, storing results in the
// 'results' array and printing them as tests complete.
static void run_sequential(
		const struct Parameters *params, struct Result *results) {
	assert(params->problem == ALL_PROBLEMS);
	assert(params->jobs == 1);

	if (params->interactive) {
		for (size_t i = 0; i < N_PROBLEMS; i++) {
			ProblemFn function = get_problem_function(INDEX_TO_PROBLEM(i));
			results[i].pos_state = test_positive(function, params->pos_iters);
			update_all_results(results);
			results[i].neg_state = test_negative(function, params->neg_iters);
			update_all_results(results);
		}
	} else {
		print_header();
		for (size_t i = 0; i < N_PROBLEMS; i++) {
			test_problem(results + i,
					INDEX_TO_PROBLEM(i), params->pos_iters, params->neg_iters);
			print_result(INDEX_TO_PROBLEM(i), results[i]);
		}
		print_summary(results);
	}
}

// Performs the Task 'arg'. Always returns NULL.
static void *perform_task(void *arg) {
	struct Task *task = (struct Task *)arg;
	for (unsigned short i = task->start; i < task->end; i++) {
		test_problem(task->results + i,
				INDEX_TO_PROBLEM(i), task->pos_iters, task->neg_iters);
	}
	--*task->tasks_left;
	return NULL;
}

// Runs the tests specified by 'params' in parallel, storing results in the
// 'results' array. Clears the screen and prints results periodically while jobs
// are progressing if 'params->interactive' is true. If there was an pthread
// error, prints an error message and returns false.
static bool run_parallel(
		const struct Parameters *params, struct Result *results) {
	assert(params->problem == ALL_PROBLEMS);
	assert(params->jobs > 1);

	const size_t jobs = (size_t)MIN(params->jobs, N_PROBLEMS);
	const unsigned short length = N_PROBLEMS / jobs;
	pthread_t threads[jobs];
	struct Task tasks[jobs];
	atomic_ushort tasks_left = jobs;

	struct Task base_task = {
		.start = 0,
		.end = 0,
		.pos_iters = (unsigned short)params->pos_iters,
		.neg_iters = (unsigned short)params->neg_iters,
		.results = results,
		.tasks_left = &tasks_left
	};

	// Create threads for the tasks.
	for (size_t i = 0; i < jobs; i++) {
		tasks[i] = base_task;
		tasks[i].start = length * (unsigned short)i;
		tasks[i].end = i == jobs - 1 ? N_PROBLEMS : tasks[i].start + length;
		int err = pthread_create(&threads[i], NULL, perform_task, &tasks[i]);
		if (err != 0) {
			printf_error("error creating thread #%d: %s", i, strerror(err));
			return false;
		}
	}

	// In interactive mode, update results until all tasks are finished.
	if (params->interactive) {
		while (tasks_left > 0) {
			update_all_results(results);
			usleep(UPDATE_DELAY_MS * 1000);
		}
		update_all_results(results);
	}

	// Join all the threads and return.
	for (size_t i = 0; i < jobs; i++) {
		int err = pthread_join(threads[i], NULL);
		if (err != 0) {
			printf_error("error joining thread #%d: %s", i, strerror(err));
			return false;
		}
	}
	return true;
}

bool run_tests(const struct Parameters *params) {
	assert(params->problem >= 0);
	assert(params->pos_iters >= 0);
	assert(params->neg_iters >= 0);
	assert(!(params->problem != ALL_PROBLEMS && params->interactive));

	// Run tests synchronously if there is only one test to run.
	if (params->problem != ALL_PROBLEMS) {
		struct Result result;
		test_problem(&result,
				params->problem, params->pos_iters, params->neg_iters);
		print_result(params->problem, result);
		return result_good(result);
	}

	// Allocate the results array (use 'calloc' because PENDING is 0).
	struct Result *results = calloc(N_PROBLEMS, sizeof *results);

	// If this is interactive mode, open the alternative terminal screen.
	if (params->interactive) {
		open_alt_screen();
		clear_screen();
	}

	// Run the tests, sequentially or in parallel.
	if (params->jobs == 1) {
		run_sequential(params, results);
	} else if (!run_parallel(params, results)) {
		free(results);
		return 1;
	}

	// Close interactive mode if necessary and print the results.
	if (params->interactive) {
		use_unbuffered_input();
		while (getchar() != QUIT_CHARACTER);
		close_alt_screen();
	}
	if (params->jobs != 1 || params->interactive) {
		print_all_results(results);
	}

	// Clean up and return.
	free(results);
	return all_results_good(results);
}
