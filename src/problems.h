// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef PROBLEMS_H
#define PROBLEMS_H

#include <stdbool.h>

// Number of exercise problems done so far.
#define N_PROBLEMS 18

// A function that executes an exercise problem, returning true on success. If
// 'positive' is true, then it uses real semaphores. Otherwise, it uses dummy
// semaphores (that way we can test for failure with semaphores disabled).
typedef bool (*ProblemFn)(bool positive);

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
extern const char *const problem_15_name;
extern const char *const problem_16_name;
extern const char *const problem_17_name;
extern const char *const problem_18_name;

// Prototypes for exercise problem functions.
bool problem_01(bool);
bool problem_02(bool);
bool problem_03(bool);
bool problem_04(bool);
bool problem_05(bool);
bool problem_06(bool);
bool problem_07(bool);
bool problem_08(bool);
bool problem_09(bool);
bool problem_10(bool);
bool problem_11(bool);
bool problem_12(bool);
bool problem_13(bool);
bool problem_14(bool);
bool problem_15(bool);
bool problem_16(bool);
bool problem_17(bool);
bool problem_18(bool);

// Returns true if there is an exercise problem numbered 'n'.
bool problem_in_range(int n);

// Returns the name of the exercise problem numbered 'n'.
const char *get_problem_name(int n);

// Returns the function pointer for the exercise problem numbered 'n'.
ProblemFn get_problem_function(int n);

#endif
