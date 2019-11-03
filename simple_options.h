/**
 * @author Andre Renaud <andre@ignavus.net>
 * @description Simple command line option parsing library
 * Provides ability to create simple unix-style command line
 * flags parsing functions with minimal code
 * Only external requirements: strcmp, strcasecmp
 */

#ifndef SIMPLE_OPTIONS_H
#define SIMPLE_OPTIONS_H

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
		const char **string;
	};
	bool present; // Set after opt_parse to indicate if the option was supplied
};

/**
 * Parse a list of option/parameters for a command
 * @param nargs The number of items in 'args'
 * @param args a list of strings to parse
 * @param options The options to check for
 * @return < 0 on failure, >= 0 on success
 *    The args may be rearranged to put non-option arguments at the end of the list.
 *.   The return value will point to the beginning of the non-option arguments, or == nargs - 1
 *.   if there are no non-option arguments
 */
int opt_parse(int nargs, const char **args, struct option_entry *options);

/**
 * Display the help/usage for a given list of options
 * @param print_func Function to be used to display the output, printf style
 * @param prog_name The name of the program to display usage (typically argv[0])
 * @param options The list of options to display the information for
 */
void opt_parse_usage(int (*print_func)(const char *string, ...), const char *prog_name, const struct option_entry *options);

/**
 * Splits a single string into the white space separated items
 * @param line original single string of input (note: This will have nul bytes
 *.       inserted to break up the elements)
 * @param output Array to store the pointers to the individual words in
 * @param max_items The maximum number of items to store in 'output'
 * @return < 0 on failure, numnber of items stored in 'output' on success
 */
int opt_parse_split_string(char *line, const char **output, int max_items);

/**
 * Determines if an option was seen after a call to opt_parse
 * Note: If the option was supplied using the 'long_opt' instead, this will still return true
 * @param short_opt The short_opt of the option to check for
 * @param options The list of options to check in
 * @return false if short_opt was not seen by opt_parse, true if it was
 */
bool opt_parse_present(char short_opt, const struct option_entry *options);

/**
 * Determines if a long option was seen after a call to opt_parse
 * Note: If the option was supplied using the 'short_opt' instead, this will still return true
 * @param long_opt The long_opt of the option to check for
 * @param options The list of options to check in
 * @return false if long_opt was not seen by opt_parse, true if it was
 */
bool opt_parse_present_long(const char *long_opt, const struct option_entry *options);

#ifdef __cplusplus
}
#endif

#endif // SIMPLE_OPTIONS_H
