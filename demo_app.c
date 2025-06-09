#include <stdio.h>
#include <stdarg.h>
#include "simple_options.h"

static int eprintf(const char *str, ...) {
	va_list ap;
	va_start(ap, str);
	vfprintf(stderr, str, ap);
	va_end(ap);
	return 0;
}

int main(int argc, const char *argv[])
{
	const char *file = NULL;
	int64_t val = 5;
	struct option_entry entries[] = {
		{"file", 'f', "File to load", OPTION_FLAG_string, .string = &file},
		{"number", 'n', "Number", OPTION_FLAG_int, .integer = &val},
		{"desc", 0, NULL, OPTION_FLAG_required},
		{NULL, 0},
	};
	int extra_args = opt_parse(argc, argv, entries);
	if (extra_args < 0) {
		opt_parse_usage(eprintf, argv[0], entries);
		return -1;
	}
	printf("file: %s (%spresent)\n", file, opt_parse_present('f', entries) ? "" : "not ");
	printf("number: %lld (%spresent)\n", (long long int)val, opt_parse_present('n', entries) ? "" : "not ");
	for (int i = extra_args; i < argc; i++) {
		printf("Extra arg %d: %s\n", i, argv[i]);
	}
	return 0;
}
