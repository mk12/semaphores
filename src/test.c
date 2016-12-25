// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "test.h"

#include "problems.h"
#include "semaphore.h"
#include "util.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Converts a zero-based index to a one-based problem number.
#define INDEX_TO_PROBLEM(i) ((int)((i) + 1))

// There are three possible states for a test.
#define N_STATES 3
enum State {
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
// filling from 'results[start]' to 'results[end-1]'.
struct Task {
	unsigned short start;      // start index in 'result' (zero-based)
	unsigned short end;        // end index in 'result' (exclusive)
	unsigned short pos_iters;  // iterations for the positive case
	unsigned short neg_iters;  // iterations for the negative case
	struct Result *results;    // array of all results
};

// A string of dots used for padding.
static const char *const padding_dots = "......................";

// Returns a status string for the given state.
static const char* state_str(enum State state) {
	switch (state) {
	case PASS:
		return "ok";
	case FAIL:
		return "FAIL";
	case SKIP:
		return "skip";
	}
}

// Returns the combination of two states into one state.
static enum State state_combine(enum State s1, enum State s2) {
	static const enum State table[N_STATES][N_STATES] = {
		[PASS] = { [PASS] = PASS, [FAIL] = FAIL, [SKIP] = PASS },
		[FAIL] = { [PASS] = FAIL, [FAIL] = FAIL, [SKIP] = FAIL },
		[SKIP] = { [PASS] = PASS, [FAIL] = FAIL, [SKIP] = SKIP }
	};
	return table[s1][s2];
}

// Tests the given exercise problem, with 'pos_iters' iterations for the
// positive case and 'neg_iters' iterations for the negative case.
static struct Result test_one(int problem, int pos_iters, int neg_iters) {
	struct Result result = { .pos_state = SKIP, .neg_state = SKIP };
	ProblemFn function = get_problem_function(problem);

	for (int i = 0; i < pos_iters; i++) {
		result.pos_state = function(true) ? PASS : FAIL;
	}
	for (int i = 0; i < neg_iters; i++) {
		result.neg_state = function(false) ? FAIL : PASS;
	}
	return result;
}

// Prints the test result for the given problem on one line.
static void print_result(int problem, struct Result result) {
	const char *name = get_problem_name(problem);
	const char *pad = padding_dots + strlen(name);
	const char *pos_msg = state_str(result.pos_state);
	const char *neg_msg = state_str(result.neg_state);
	printf("%02d. %s %s %s/%s\n", problem, name, pad, pos_msg, neg_msg);
}

// Prints the test result summary line given the number of passes and failures.
static void print_summary(int passes, int fails, int skips) {
	if (fails == 0 && skips == 0) {
		puts("100%% of tests passed.");
	}
	printf("%d passed; %d failed; %d skipped.\n", passes, fails, skips);
}

// Performs the Task 'arg'. Always returns NULL.
static void *perform_task(void *arg) {
	const struct Task *task = (struct Task *)arg;
	for (unsigned short i = task->start; i < task->end; i++) {
		task->results[i] = test_one(
				INDEX_TO_PROBLEM(i), task->pos_iters, task->neg_iters);
	}
	return NULL;
}

// Breaks 'task' into 'n' subtasks and performs them in parallel. Returns true
// on success; prints an error message and returns false on failure. Waits for
// all subtask threads to terminate before returning.
static bool perform_in_parallel(const struct Task *task, size_t n) {
	pthread_t threads[n];
	struct Task subtasks[n];
	const unsigned short length = N_PROBLEMS / n;

	for (size_t i = 0; i < n; i++) {
		subtasks[i] = *task;
		subtasks[i].start = length * (unsigned short)i;
		subtasks[i].end = i == n - 1 ? N_PROBLEMS : subtasks[i].start + length;
		int err = pthread_create(&threads[i], NULL, perform_task, &subtasks[i]);
		if (err != 0) {
			printf_error("error creating thread #%d: %s", i, strerror(err));
			return false;
		}
	}
	for (size_t i = 0; i < n; i++) {
		int err = pthread_join(threads[i], NULL);
		if (err != 0) {
			printf_error("error joining thread #%d: %s", i, strerror(err));
			return false;
		}
		for (unsigned short j = subtasks[i].start; j < subtasks[i].end; j++) {
			print_result(INDEX_TO_PROBLEM(j), subtasks[0].results[j]);
		}
	}
	return true;
}

bool run_tests(const struct Parameters *params) {
	assert(params->problem >= 0);
	assert(params->pos_iters >= 0);
	assert(params->neg_iters >= 0);

	// Run tests synchronously if there is only one test to run.
	if (params->problem != ALL_PROBLEMS) {
		struct Result result = test_one(
				params->problem, params->pos_iters, params->neg_iters);
		print_result(params->problem, result);
		return state_combine(result.pos_state, result.neg_state) != FAIL;
	}

	// Run tests synchronously if parallelization is not enabled.
	if (params->jobs == 1) {
		int counts[N_STATES];
		for (int i = 1; i <= N_PROBLEMS; i++) {
			struct Result result =
					test_one(i, params->pos_iters, params->neg_iters);
			print_result(i, result);
			counts[result.pos_state]++;
			counts[result.neg_state]++;
		}
		print_summary(counts[PASS], counts[FAIL], counts[SKIP]);
		return counts[FAIL] == 0;
	}

	// Run tests in parallel according to the number of jobs specified.
	struct Task task = {
		.start = 0,
		.end = N_PROBLEMS,
		.pos_iters = (unsigned short)params->pos_iters,
		.neg_iters = (unsigned short)params->neg_iters,
		.results = malloc(N_PROBLEMS * sizeof *task.results)
	};
	size_t n = (size_t)MIN(params->jobs, N_PROBLEMS);
	bool success = perform_in_parallel(&task, n);
	free(task.results);
	return success;
}
