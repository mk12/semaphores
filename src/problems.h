// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef PROBLEMS_H
#define PROBLEMS_H

#include <stdbool.h>

// Number of exercise problems done so far.
#define N_PROBLEMS 8

// A function that executes an exercise problem, returning true on success.
typedef bool (*ProblemFn)(void);

// Declarations for exercise problem names.
extern const char *const problem_1_name;
extern const char *const problem_2_name;
extern const char *const problem_3_name;
extern const char *const problem_4_name;
extern const char *const problem_5_name;
extern const char *const problem_6_name;
extern const char *const problem_7_name;
extern const char *const problem_8_name;

// Prototypes for exercise problem functions.
bool problem_1(void);
bool problem_2(void);
bool problem_3(void);
bool problem_4(void);
bool problem_5(void);
bool problem_6(void);
bool problem_7(void);
bool problem_8(void);

// Returns the name of the exercise problem numbered 'n'.
const char *get_problem_name(int n);

// Returns the function pointer for the exercise problem numbered 'n'.
ProblemFn get_problem_function(int n);

#endif
