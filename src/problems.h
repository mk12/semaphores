// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef PROBLEMS_H
#define PROBLEMS_H

#include <stdbool.h>

// Number of exercise problems done so far.
#define N_PROBLEMS 14

// A function that executes an exercise problem, returning true on success.
typedef bool (*ProblemFn)(void);

// Declarations for exercise problem names.
extern const char *const problem_01_name;
extern const char *const problem_02_name;
extern const char *const problem_03_name;
extern const char *const problem_04_name;
extern const char *const problem_05_name;
extern const char *const problem_06_name;
extern const char *const problem_07_name;
extern const char *const problem_08_name;
extern const char *const problem_09_name;
extern const char *const problem_10_name;
extern const char *const problem_11_name;
extern const char *const problem_12_name;
extern const char *const problem_13_name;
extern const char *const problem_14_name;

// Prototypes for exercise problem functions.
bool problem_01(void);
bool problem_02(void);
bool problem_03(void);
bool problem_04(void);
bool problem_05(void);
bool problem_06(void);
bool problem_07(void);
bool problem_08(void);
bool problem_09(void);
bool problem_10(void);
bool problem_11(void);
bool problem_12(void);
bool problem_13(void);
bool problem_14(void);

// Returns the name of the exercise problem numbered 'n'.
const char *get_problem_name(int n);

// Returns the function pointer for the exercise problem numbered 'n'.
ProblemFn get_problem_function(int n);

#endif
