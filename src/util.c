// Copyright 2016 Mitchell Kember. Subject to the MIT License.

#include "util.h"

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// The name of the program.
static const char *program_name = NULL;

// Function pointer for the default sigint handler.
static void (*default_sigint_handler)(int) = NULL;

void setup_util(const char *the_program_name) {
	program_name = the_program_name;
}

void printf_error(const char *format, ...)
	__attribute__((__format__ (__printf__, 1, 2)));
void printf_error(const char *format, ...) {
	va_list args;
	va_start(args, format);
	fprintf(stderr, "%s: ", program_name);
	vfprintf(stderr, format, args);
	putc('\n', stderr);
	va_end(args);
}

bool parse_int(int *out, const char *str) {
	// With the -a=b option syntax, 'str' will be "=b".
	if (str[0] == '=' && str[1]) {
		str++;
	}

	char *end;
	int n = (int)strtol(str, &end, 0);
	if (*end) {
		printf_error("%s: not an integer", str);
		return false;
	}
	*out = n;
	return true;
}

// Calls 'close_alt_screen' and then executes the default sigint handler.
static void sigint_handler(int sig) {
	close_alt_screen();
	default_sigint_handler(sig);
}

void open_alt_screen(void) {
	default_sigint_handler = signal(SIGINT, sigint_handler);
	fputs("\x1B[?1049h", stdout);
	fflush(stdout);
}

void close_alt_screen(void) {
	fputs("\x1B[?1049l", stdout);
	fflush(stdout);
}

void clear_screen(void) {
	fputs("\x1B[2J\x1B[H", stdout);
	fflush(stdout);
}
