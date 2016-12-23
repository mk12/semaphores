// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "problems.h"

#include <stddef.h>

const char *get_problem_name(int n) {
	switch (n) {
	case 1: return problem_1_name;
	case 2: return problem_2_name;
	case 3: return problem_3_name;
	case 4: return problem_4_name;
	case 5: return problem_5_name;
	case 6: return problem_6_name;
	case 7: return problem_7_name;
	default: return NULL;
	}
}

ProblemFn get_problem_function(int n) {
	switch (n) {
	case 1: return problem_1;
	case 2: return problem_2;
	case 3: return problem_3;
	case 4: return problem_4;
	case 5: return problem_5;
	case 6: return problem_6;
	case 7: return problem_7;
	default: return NULL;
	}
}
