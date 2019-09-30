#include "acutest.h"
#include "option_parser.h"

static void simple_args(void)
{
	const char *file = NULL;
	int64_t val = 5;
	bool flag = false;
	struct option_entry entries[] = {
		{"file", 'f', "File to load", OPTION_FLAG_string, .string = &file},
		{"number", 'n', "Number", OPTION_FLAG_int, .integer = &val},
		{"bool", 'b', "Boolean", OPTION_FLAG_bool, .boolean = &flag},
		{"missing", 'm', "Missing"},
		{NULL, 0},
	};
	char *args[] = {"program", "-f", "fnord", "--number", "7", "--bool", "on"};
	TEST_CHECK(opt_parse(7, args, entries) >= 0);
	TEST_CHECK(file && strcmp(file, "fnord") == 0);
	TEST_CHECK(val == 7);
	TEST_CHECK(opt_parse_present('f', entries));
	TEST_CHECK(!opt_parse_present('m', entries));
	TEST_CHECK(opt_parse_present_long("bool", entries));
	TEST_CHECK(opt_parse_present_long("file", entries));
	TEST_CHECK(flag == true);
}

static void split_string(void)
{
	char input[] = " Command arg1 \targ2  arg3 missing ";
	int argc;
	char *argv[4];
	argc = opt_parse_split_string(input, argv, 4);
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
	argc = opt_parse_split_string(empty, argv, 4);
	TEST_CHECK(argc == 0);

	char blank[] = "     ";
	argc = opt_parse_split_string(blank, argv, 4);
	TEST_CHECK(argc == 0);

	char middle[] = "   fnord   ";
	argc = opt_parse_split_string(middle, argv, 4);
	TEST_CHECK(argc == 1);
	TEST_CHECK(strcmp(argv[0], "fnord") == 0);

	char single[] = "single";
	argc = opt_parse_split_string(single, argv, 4);
	TEST_CHECK(argc == 1);
	TEST_CHECK(strcmp(argv[0], "single") == 0);
}

static void combined(void)
{
	const char *file = NULL;
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
	argc = opt_parse_split_string(input, argv, 10);
	TEST_CHECK(argc == 7);
	TEST_CHECK(opt_parse(argc, argv, entries) >= 0);
	TEST_CHECK(file && strcmp(file, "fnord") == 0);
	TEST_CHECK(val == 7);
	TEST_CHECK(entries[0].present);
	TEST_CHECK(flag == true);
}

static void required(void)
{
	struct option_entry entries[] = {
		{"foo", 'f', NULL, OPTION_FLAG_required},
		{NULL},
	};
	char *args[] = {"program"};
	TEST_CHECK(opt_parse(1, args, entries) < 0);
	char *new_args[] = {"program", "-f"};
	TEST_CHECK(opt_parse(2, new_args, entries) >= 0);
}

TEST_LIST = {
	{"simple", simple_args},
	{"split string", split_string},
	{"bad strings", bad_strings},
	{"combined split & parse", combined},
	{"required params", required},
	{NULL, NULL},
};
