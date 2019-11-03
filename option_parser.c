#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "option_parser.h"

#define OPT_ITERATE(opt, options) struct option_entry *opt = options; opt && (opt->long_opt || opt->short_opt); opt++

static bool needs_param(const struct option_entry *o)
{
	return o->flags & (OPTION_FLAG_bool | OPTION_FLAG_int | OPTION_FLAG_string);
}

void opt_parse_usage(int (*print_func)(const char *string, ...), const char *prog_name, const struct option_entry *options)
{
	print_func("Usage: %s [options]\n", prog_name);
	print_func("\t-h, --help\tDisplay this help message\n");
	for (const OPT_ITERATE(o, options)) {
		print_func("\t");
		if (o->short_opt)
			print_func("-%c", o->short_opt);
		if (o->long_opt)
			print_func("%s--%s", o->short_opt ? ", " : "    ", o->long_opt);
		print_func("\t%s", o->description ? o->description : "");
		if (o->flags & OPTION_FLAG_int)
			print_func(" [default: %lld]", *o->integer);
		if (o->flags & OPTION_FLAG_string && *o->string)
			print_func(" [default: \"%s\"]", *o->string);
		if (o->flags & OPTION_FLAG_bool)
			print_func(" [default: %s]", *o->boolean ? "true" : "false");
		if (o->flags & OPTION_FLAG_required)
			print_func("%s[required]", (needs_param(o) || o->description) ? " " : "");
		print_func("\n");
	}
}

static void set_param(struct option_entry *o, const char *val)
{
	if (o->flags & OPTION_FLAG_bool) {
		*o->boolean = strcasecmp(val, "true") == 0 ||
		              strcasecmp(val, "t") == 0 ||
		              strcasecmp(val, "yes") == 0 ||
		              strcasecmp(val, "on") == 0 ||
		              strcasecmp(val, "enable") == 0;
	} else if (o->flags & OPTION_FLAG_int) {
		*o->integer = strtoll(val, NULL, 0);
	} else if (o->flags & OPTION_FLAG_string) {
		*o->string = val;
	}
}

static bool options_valid(const struct option_entry *options)
{
	if (!options)
		return false;
	for (const OPT_ITERATE(o, options)) {
		// Only one type can be used
		if (((o->flags & OPTION_FLAG_bool) && (o->flags & OPTION_FLAG_int)) ||
			((o->flags & OPTION_FLAG_bool) && (o->flags & OPTION_FLAG_string)) ||
			((o->flags & OPTION_FLAG_int) && (o->flags & OPTION_FLAG_string)))
			return false;
		// Option flags must have an area to store the data
		// Assume that the union of pointers will all overlap
		if (needs_param(o) && !o->integer)
			return false;
	}
	return true;
}

int opt_parse(int nargs, const char **args, struct option_entry *options)
{
	int swap_pos = nargs - 1;
	if (!args || !nargs)
		return -1;
	if (!options_valid(options))
		return -1;
	for (OPT_ITERATE(o, options))
		o->present = false;
	for (int i = 1; i <= swap_pos; i++) {
		const char *arg = args[i];
		if (!arg)
			continue;
		if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0)
			return -1;
		if (arg[0] == '-' && arg[1] == '-') {
			char *end = strchr(arg, '=');
			int len = strlen(&arg[2]);
			if (end)
				len = end - arg - 2;
			bool found = false;
			for (OPT_ITERATE(o, options)) {
				if (strncmp(o->long_opt, &arg[2], len) == 0) {
					o->present = true;
					if (needs_param(o)) {
						if (end)
							set_param(o, end + 1);
						else if (i < nargs - 1)
							set_param(o, args[++i]);
						else
							return -1;
					}
					found = true;
					break;
				}
			}
			if (!found)
				return -1;
		} else if (arg[0] == '-') {
			bool found = false;
			for (OPT_ITERATE(o, options)) {
				if (o->short_opt == arg[1]) {
					o->present = true;
					found = true;
					if (needs_param(o)) {
						if (arg[2] == '=')
							set_param(o, &arg[3]);
						else if (i < nargs - 1) {
							set_param(o, args[++i]);
						}
						else
							return -1;
					}
					break;
				}
			}
			if (!found)
				return -1;
		} else {
			// This is a non-option argument. Just move it to the end of the arg list
			for (int j = i; j < nargs - 1; j++) {
				const char *tmp = args[j + 1];
				args[j + 1] = args[j];
				args[j] = tmp;
			}
			args[nargs - 1] = arg;
			swap_pos--;
			i--; // We have to re-process this position again, since we've just swapped it
		}
	}

	for (const OPT_ITERATE(o, options)) {
		if ((o->flags & OPTION_FLAG_required) && !o->present)
			return -1;
	}
	return swap_pos + 1;
}

bool opt_parse_present(char short_opt, const struct option_entry *options)
{
	for (const OPT_ITERATE(o, options)) {
		if (o->short_opt == short_opt)
			return o->present;
	}
	return false;
}

bool opt_parse_present_long(const char *long_opt, const struct option_entry *options)
{
	for (const OPT_ITERATE(o, options)) {
		if (o->long_opt && strcmp(o->long_opt, long_opt) == 0)
			return o->present;
	}
	return false;
}

static bool is_whitespace(char ch)
{
	return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
}

int opt_parse_split_string(char *line, const char **output, int max_items)
{
	char *pos = line;
	int nitems = 0;

	while (pos && *pos && nitems < max_items) {
		while (is_whitespace(*pos) && *pos)
			pos++;

		if (!*pos)
			break;
		output[nitems++] = pos;
		while (!is_whitespace(*pos) && *pos)
			pos++;
		if (!*pos)
			break;
		*pos = '\0';
		pos++;
	}

	return nitems;
}
