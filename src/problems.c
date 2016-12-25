// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "problems.h"

#include <stddef.h>

bool problem_in_range(int n) {
	return n >= 1 && n <= N_PROBLEMS;
}

const char *get_problem_name(int n) {
	switch (n) {
	case  1: return problem_01_name;
	case  2: return problem_02_name;
	case  3: return problem_03_name;
	case  4: return problem_04_name;
	case  5: return problem_05_name;
	case  6: return problem_06_name;
	case  7: return problem_07_name;
	case  8: return problem_08_name;
	case  9: return problem_09_name;
	case 10: return problem_10_name;
	case 11: return problem_11_name;
	case 12: return problem_12_name;
	case 13: return problem_13_name;
	case 14: return problem_14_name;
	case 15: return problem_15_name;
	default: return NULL;
	}
}

ProblemFn get_problem_function(int n) {
	switch (n) {
	case  1: return problem_01;
	case  2: return problem_02;
	case  3: return problem_03;
	case  4: return problem_04;
	case  5: return problem_05;
	case  6: return problem_06;
	case  7: return problem_07;
	case  8: return problem_08;
	case  9: return problem_09;
	case 10: return problem_10;
	case 11: return problem_11;
	case 12: return problem_12;
	case 13: return problem_13;
	case 14: return problem_14;
	case 15: return problem_15;
	default: return NULL;
	}
}
