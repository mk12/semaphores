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

// The result of testing a solution to an exercise problem.
struct Result {
	bool pos_good : 1;  // true if test succeeded with semaphores enabled
	bool neg_good : 1;  // true if test failed with semaphores disabled
};

// Structure to describe one or more exercise problems and how to test them.
// Performing a Task consists of executing the solution program(s) and storing
// the result(s) in the 'result' location.
struct Task {
	int problem;            // problem number or ALL_PROBLEMS
	int pos_iterations;     // number of iterations for the positive case
	int neg_iterations;     // number of iterations for the negative case
	struct Result *result;  // destination for one result, or array of results
};

// A string of dots used for padding.
static const char *const padding_dots = "......................";

// Returns a status string to indicate success (true), failure (false), or
// skipping (when 'iterations' is zero).
static const char* status_str(bool success, int iterations) {
	return iterations == 0 ? "skip" : success ? "ok" : "FAIL";
}

// Tests one exercise problem.
static void test_one(const struct Task *task) {
	assert(task->problem != ALL_PROBLEMS);
	struct Result result = { .pos_good = true, .neg_good = true };
	ProblemFn function = get_problem_function(task->problem);

	set_semaphores_enabled(true);
	for (int i = 0; i < task->pos_iterations; i++) {
		if (!function()) {
			result.pos_good = false;
			break;
		}
	}
	set_semaphores_enabled(false);
	for (int i = 0; i < task->neg_iterations; i++) {
		if (function()) {
			result.neg_good = false;
			break;
		}
	}
	*task->result = result;
}

// Tests all exercise problems.
static void test_all(const struct Task *task) {
	assert(task->problem == ALL_PROBLEMS);

	struct Task t = *task;
	t.problem = 1;
	while (t.problem <= N_PROBLEMS) {
		test_one(&t);
		t.problem++;
		t.result++;
	}
}

// Performs the Task 'arg'. Always returns NULL.
static void *perform_task(void *arg) {
	const struct Task *task = (struct Task *)arg;
	if (task->problem == ALL_PROBLEMS) {
		test_all(task);
	} else {
		test_one(task);
	}
	return NULL;
}

// Breaks 'task' into 'n' subtasks and performs them in parallel. Returns true
// on success; prints an error message and returns false on failure. Waits for
// all subtask threads to terminate before returning.
static bool perform_in_parallel(const struct Task *task, size_t n) {
	assert(task->problem == ALL_PROBLEMS);
	pthread_t threads[n];
	struct Task subtasks[n];

	for (size_t i = 0; i < n; i++) {
		subtasks[i] = *task;
		subtasks[i].problem = (int)(i + 1);
		subtasks[i].result = task->result + i;
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
	}
	return true;
}

bool run_tests(const struct Parameters *params) {
	assert(params->problem >= 0);
	assert(params->pos_iterations >= 0);
	assert(params->neg_iterations >= 0);

	struct Task task = {
		.problem = params->problem,
		.pos_iterations = params->pos_iterations,
		.neg_iterations = params->neg_iterations
	};
	size_t n = params->problem == ALL_PROBLEMS ? N_PROBLEMS : 1;
	task.result = malloc((size_t)n * sizeof *task.result);

	if (params->problem != ALL_PROBLEMS || params->jobs == 1) {
		perform_task(&task);
	} else {
		size_t jobs = (size_t)MIN(params->jobs, N_PROBLEMS);
		if (!perform_in_parallel(&task, jobs)) {
			free(task.result);
			return false;
		}
	}

	// Print the results.
	int passes = 0;
	for (size_t i = 0; i < n; i++) {
		struct Result result = task.result[i];
		const char *name = get_problem_name((int)(i + 1));
		const char *pad = padding_dots + strlen(name);
		const char *pos_msg = status_str(result.pos_good, task.pos_iterations);
		const char *neg_msg = status_str(result.neg_good, task.neg_iterations);
		printf("%02zu. %s %s %s/%s\n", i + 1, name, pad, pos_msg, neg_msg);
		passes += result.pos_good && result.neg_good;
	}
	int fails = (int)n - passes;
	bool success = fails == 0;
	const char* msg = status_str(success, 1);
	printf("%s. %d passed; %d failed\n", msg, passes, fails);

	// Clean up and return.
	free(task.result);
	return success;
}
