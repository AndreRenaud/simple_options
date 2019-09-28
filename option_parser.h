#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

#include <stdint.h>
#include <stdbool.h>

enum {
	OPTION_FLAG_none = 0,
	OPTION_FLAG_bool = 1 << 0, // Takes boolean parameter
	OPTION_FLAG_int = 1 << 1, // Takes integer parameter
	OPTION_FLAG_string = 1 << 2, // Takes string parameter
	OPTION_FLAG_required = 1 << 3, // Must be present
};

struct option_entry {
	const char *long_opt;
	char short_opt;
	const char *description;
	uint32_t flags;
	union {
		int64_t *integer;
		bool *boolean;
		char **string;
	};
	bool present;
};

int option_parse(int nargs, const char *args[], struct option_entry *options);

#endif