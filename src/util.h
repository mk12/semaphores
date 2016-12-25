// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

// Macros for min and max.
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

// Performs necessary setup. Must be called once when the program starts.
void setup_util(const char *program_name);

// Prints an error message to stderr with printf syntax.
void printf_error(const char *format, ...);

// Parses a string as an int. Stores the result in 'out' and returns true on
// success; prints an error message and returns false on failure. If 'str'
// begins with an equals sign, it is ignored.
bool parse_int(int *out, const char *str);

#endif
