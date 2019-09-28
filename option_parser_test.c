#include "acutest.h"
#include "option_parser.h"

static void simple_args(void)
{
	char *file = NULL;
	int64_t val = 5;
	bool flag = false;
	struct option_entry entries[] = {
		{"file", 'f', "File to load", OPTION_FLAG_string, .string = &file},
		{"number", 'n', "Number", OPTION_FLAG_int, .integer = &val},
		{"bool", 'b', "Boolean", OPTION_FLAG_bool, .boolean = &flag},
		{NULL, 0},
	};
	const char *args[] = {"program", "-f", "fnord", "--number", "7", "--bool", "on"};
	TEST_CHECK(option_parse(7, args, entries) >= 0);
	TEST_CHECK(file && strcmp(file, "fnord") == 0);
	TEST_CHECK(val == 7);
	TEST_CHECK(entries[0].present);
	TEST_CHECK(flag == true);
}

TEST_LIST = {
	{"simple", simple_args},
	{NULL, NULL},
};