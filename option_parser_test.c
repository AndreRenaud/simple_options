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
	char *args[] = {"program", "-f", "fnord", "--number", "7", "--bool", "on"};
	TEST_CHECK(option_parse(7, args, entries) >= 0);
	TEST_CHECK(file && strcmp(file, "fnord") == 0);
	TEST_CHECK(val == 7);
	TEST_CHECK(entries[0].present);
	TEST_CHECK(flag == true);
}

static void split_string(void)
{
	char input[] = " Command arg1 \targ2  arg3 missing ";
	int argc;
	char *argv[4];
	argc = option_parse_split_string(input, argv, 4);
	TEST_CHECK(argc == 4);
	TEST_CHECK(strcmp(argv[0], "Command") == 0);
	TEST_CHECK(strcmp(argv[1], "arg1") == 0);
	TEST_CHECK(strcmp(argv[2], "arg2") == 0);
	TEST_CHECK(strcmp(argv[3], "arg3") == 0);
}

static void bad_strings(void)
{
	int argc;
	char *argv[4];

	char empty[] = "";
	argc = option_parse_split_string(empty, argv, 4);
	TEST_CHECK(argc == 0);

	char blank[] = "     ";
	argc = option_parse_split_string(blank, argv, 4);
	TEST_CHECK(argc == 0);

	char middle[] = "   fnord   ";
	argc = option_parse_split_string(middle, argv, 4);
	TEST_CHECK(argc == 1);
	TEST_CHECK(strcmp(argv[0], "fnord") == 0);

	char single[] = "single";
	argc = option_parse_split_string(single, argv, 4);
	TEST_CHECK(argc == 1);
	TEST_CHECK(strcmp(argv[0], "single") == 0);
}

static void combined(void)
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
	char input[] = {"program -f fnord --number 7 --bool on"};
	char *argv[10];
	int argc;
	argc = option_parse_split_string(input, argv, 10);
	TEST_CHECK(argc == 7);
	TEST_CHECK(option_parse(argc, argv, entries) >= 0);
	TEST_CHECK(file && strcmp(file, "fnord") == 0);
	TEST_CHECK(val == 7);
	TEST_CHECK(entries[0].present);
	TEST_CHECK(flag == true);
}

TEST_LIST = {
	{"simple", simple_args},
	{"split string", split_string},
	{"bad strings", bad_strings},
	{"combined split & parse", combined},
	{NULL, NULL},
};