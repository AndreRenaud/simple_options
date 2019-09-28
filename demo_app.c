#include <stdio.h>
#include "option_parser.h"

int main(int argc, const char *argv[])
{
	char *file = NULL;
	int64_t val = 5;
	struct option_entry entries[] = {
		{"file", 'f', "File to load", OPTION_FLAG_string, .string = &file},
		{"number", 'n', "Number", OPTION_FLAG_int, .integer = &val},
		{NULL, 0},
	};
	if (option_parse(argc, argv, entries) < 0)
		return -1;
	printf("file: %s\n", file);
	printf("number: %lld\n", val);
	return 0;
}