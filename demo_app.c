#include <stdio.h>
#include <stdarg.h>
#include "option_parser.h"

static int eprintf(const char *str, ...) {
	va_list ap;
	va_start(ap, str);
	vfprintf(stderr, str, ap);
	va_end(ap);
	return 0;
}

int main(int argc, char *const argv[])
{
	const char *file = NULL;
	int64_t val = 5;
	struct option_entry entries[] = {
		{"file", 'f', "File to load", OPTION_FLAG_string, .string = &file},
		{"number", 'n', "Number", OPTION_FLAG_int, .integer = &val},
		{NULL, 0},
	};
	if (opt_parse(argc, argv, entries) < 0) {
		opt_parse_usage(eprintf, argv[0], entries);
		return -1;
	}
	printf("file: %s\n", file);
	printf("number: %lld\n", (long long int)val);
	return 0;
}
