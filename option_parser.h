/**
 * @description Command line option parsing library
 * Provides ability to create simple unix-style command line
 * flags parsing functions with minimal code
 * Only external requirements: strcmp, strcasecmp
 */

#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
	OPTION_FLAG_none = 0,
	OPTION_FLAG_bool = 1 << 0, // Takes boolean parameter
	OPTION_FLAG_int = 1 << 1, // Takes integer parameter
	OPTION_FLAG_string = 1 << 2, // Takes string parameter
	OPTION_FLAG_required = 1 << 3, // Must be present
};

struct option_entry {
	const char *long_opt; // Name of the option when used in the long format
	char short_opt; // Single character short option
	const char *description; // Description for the usage/help message
	uint32_t flags; // Flags (see OPTION_FLAG_xxx)
	union {
		int64_t *integer; // Pointers to where the option values should be stored
		bool *boolean;
		char **string;
	};
	bool present; // Set after opt_parse to indicate if the option was supplied
};

void opt_parse_usage(int (*print_func)(const char *string, ...), const char *prog_name, const struct option_entry *options);
int opt_parse(int nargs, char **args, struct option_entry *options);
int opt_parse_split_string(char *line, char **output, int max_items);
bool opt_parse_present(char short_opt, const struct option_entry *options);
bool opt_parse_present_long(const char *long_opt, const struct option_entry *options);

#ifdef __cplusplus
}
#endif

#endif // OPTION_PARSER_H