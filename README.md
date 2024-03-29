# Simple Options
Build: <img src="https://github.com/AndreRenaud/simple_options/actions/workflows/build_and_test.yml/badge.svg" alt="Workflow"/>

<img src="https://raw.githubusercontent.com/AndreRenaud/simple_options/master/logo.png" alt="Simple Options Logo" width="480"/>

Simple Options is a minimal command line arguments parsing library designed
to provide minimal argument parsing in a single function call - no need
to iterate over the arguments/getopt.
It is designed for simple command line utilities (or in-built CLI-style
commands) with standard command line behaviour.

It supports both single-character short, and multi-character long style
arguments, ie: `-f filename` or `--file filename`.

It also provides some utility functions for automatically breaking down a
single line of text into the whitespace separate arguments of a typical
command line application. A simple --help style feature is built in,
displaying standard usage behaviour.

It works with standard argc/argv values from main, but does not assume
any stdio access. It makes no stdio printf/fprintf calls, and as such is
suitable for embedded systems with different command interfaces, or when
interacting with a non stdin/stdout/stderr system.

## Example Usage
```c
#include <stdio.h>
#include "simple_options.h"

int main(int argc, const char *argv[]) {
	const char *file = NULL;
	int64_t val = 5;
	struct option_entry entries[] = {
		{"file", 'f', "File to load", OPTION_FLAG_string, .string = &file},
		{"number", 'n', "Number", OPTION_FLAG_int, .integer = &val},
		{"desc", 0, NULL, OPTION_FLAG_required},
		{NULL, 0},
	};
	if (opt_parse(argc, argv, entries) < 0) {
		opt_parse_usage(printf, argv[0], entries);
		return -1;
	}
	if (opt_parse_present('f', entries))
		printf("file: %s\n", file);
	printf("number: %lld\n", val);
	return 0;
}
```


```sh
% ./demo_app
Usage: ./demo_app [options]
	-h, --help	Display this help message
	-f, --file	File to load
	-n, --number	Number [default: 5]
	    --desc	[required]
% ./demo_app -f foo
Usage: ./demo_app [options]
	-h, --help	Display this help message
	-f, --file	File to load [default: "foo"]
	-n, --number	Number [default: 5]
	    --desc	[required]
% ./demo_app -f foo --desc wibble
file: foo
number: 5
Extra arg 4: wibble
```
