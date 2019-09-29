#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "option_parser.h"

static int dump_usage(const char *prog_name, struct option_entry *options)
{
	fprintf(stderr, "Usage: %s [options]\n", prog_name);
	fprintf(stderr, "\t-h, --help\tDisplay this help message\n");
	for (struct option_entry *o = options; o && (o->long_opt || o->short_opt); o++) {
		fprintf(stderr, "\t");
		if (o->short_opt)
			fprintf(stderr, "-%c", o->short_opt);
		if (o->long_opt)
			fprintf(stderr, "%s--%s", o->short_opt ? ", " : "    ", o->long_opt);
		if (o->description)
			fprintf(stderr, "\t%s", o->description);
		if (o->flags & OPTION_FLAG_int)
			fprintf(stderr, " [default: %lld]", *o->integer);
		if (o->flags & OPTION_FLAG_string && *o->string)
			fprintf(stderr, " [default: \"%s\"]", *o->string);
		if (o->flags & OPTION_FLAG_bool)
			fprintf(stderr, " [default: %s]", *o->boolean ? "true" : "false");
		if (o->flags & OPTION_FLAG_required)
			fprintf(stderr, " [required]");
		fprintf(stderr, "\n");
	}
	return -1;
}

static bool needs_param(struct option_entry *o)
{
	return o->flags & (OPTION_FLAG_bool | OPTION_FLAG_int | OPTION_FLAG_string);
}

static void set_param(struct option_entry *o, char *val)
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

int option_parse(int nargs, char **args, struct option_entry *options)
{
	if (!args || !nargs || !options)
		return -1;
	for (struct option_entry *o = options; o->long_opt || o->short_opt; o++)
		o->present = false;
	for (int i = 1; i < nargs; i++) {
		char *arg = args[i];
		if (!arg)
			continue;
		if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0)
			return dump_usage(args[0], options);
		if (arg[0] == '-' && arg[1] == '-') {
 			char *end = strchr(arg, '=');
 			int len = strlen(&arg[2]);
 			if (end)
 				len = end - arg - 2;
 			bool found = false;
	 		for (struct option_entry *o = options; o->long_opt || o->short_opt; o++) {
	 			if (strncmp(o->long_opt, &arg[2], len) == 0) {
	 				o->present = true;
	 				if (needs_param(o)) {
	 					if (end)
	 						set_param(o, end + 1);
	 					else if (i < nargs - 1)
	 						set_param(o, args[++i]);
	 					else
	 						return dump_usage(args[0], options);
	 				}
	 				found = true;
	 				break;
	 			}
	 		}
	 		if (!found) {
	 			fprintf(stderr, "Invalid option \"%s\"\n", &arg[2]);
	 			return dump_usage(args[0], options);
	 		}
		} else if (arg[0] == '-') {
			bool found = false;
	 		for (struct option_entry *o = options; o->long_opt || o->short_opt; o++) {
	 			if (o->short_opt == arg[1]) {
	 				o->present = true;
	 				found = true;
	 				if (needs_param(o)) {
	 					if (arg[2] == '=')
	 						set_param(o, &arg[3]);
	 					else if (i < nargs - 1)
	 						set_param(o, args[++i]);
	 					else
	 						return dump_usage(args[0], options);
	 				}
	 				break;
	 			}
	 		}
	 		if (!found) {
	 			fprintf(stderr, "Invalid option '%c'\n", arg[1]);
	 			return dump_usage(args[0], options);
	 		}
		} else {
			return dump_usage(args[0], options);
		}
	}
	return 0;
}

static bool is_whitespace(char ch)
{
	return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
}

int option_parse_split_string(char *line, char **output, int max_items)
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
