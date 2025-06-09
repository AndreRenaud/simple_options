#include "acutest.h"
#include "simple_options.h"

static void simple_args(void)
{
	const char *file = NULL;
	const char *filename = NULL;
	int64_t val = 5;
	float fval = 1.0;
	bool flag = false;
	struct option_entry entries[] = {
		{"filename", 'z', "Alternative filename", OPTION_FLAG_string, .string = &filename},
		{"file", 'f', "File to load", OPTION_FLAG_string, .string = &file},
		{"number", 'n', "Number", OPTION_FLAG_int, .integer = &val},
		{"bool", 'b', "Boolean", OPTION_FLAG_bool, .boolean = &flag},
		{"float", 'l', "Floating", OPTION_FLAG_float, .floating = &fval},
		{"missing", 'm', "Missing"},
		{NULL, 0},
	};
	const char *args[] = {"program", "--file", "fnord", "--number", "7", "--bool", "on", "--float", "0.57", "--filename", "wibble"};
	TEST_CHECK(opt_parse(11, args, entries) >= 0);
	TEST_CHECK(file && strcmp(file, "fnord") == 0);
	TEST_CHECK(filename && strcmp(filename, "wibble") == 0);
	TEST_CHECK(val == 7);
	TEST_CHECK(opt_parse_present('f', entries));
	TEST_CHECK(!opt_parse_present('m', entries));
	TEST_CHECK(opt_parse_present_long("bool", entries));
	TEST_CHECK(opt_parse_present_long("file", entries));
	TEST_CHECK(!opt_parse_present_long("unknown", entries));
	TEST_CHECK(opt_parse_present_long("float", entries));
	TEST_CHECK(fval > 0.56999 && fval < 0.57001);
	TEST_CHECK(flag == true);
}

static void split_string(void)
{
	char input[] = " Command arg1 \targ2  arg3 missing ";
	int argc;
	const char *argv[4];
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
	const char *argv[4];

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
	char input[] = {"program -f fnord --number=4 --bool=on -n7"};
	const char *argv[10];
	int argc;
	argc = opt_parse_split_string(input, argv, 10);
	TEST_CHECK(argc == 6);
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
	const char *args[] = {"program"};
	TEST_CHECK(opt_parse(1, args, entries) < 0);
	const char *new_args[] = {"program", "-f"};
	TEST_CHECK(opt_parse(2, new_args, entries) >= 0);
}

static void extra_params(void)
{
	int extra;
	const char *foo = NULL;
	struct option_entry entries[] = {
		{"foo", 'f', NULL, OPTION_FLAG_string, .string = &foo},
		{NULL},
	};

	const char *args[] = {"prorgram", "extra"};
	extra = opt_parse(2, args, entries);
	TEST_CHECK(extra == 1);
	TEST_CHECK(strcmp(args[extra], "extra") == 0);

	const char *args2[] = {"program", "extra", "-f", "foo"};
	extra = opt_parse(4, args2, entries);
	TEST_CHECK(extra == 3);
	TEST_CHECK(strcmp(args2[extra], "extra") == 0);

	const char *args3[] = {"program", "extra1", "extra2", "extra3", "-f", "foo", "extra4"};
	extra = opt_parse(7, args3, entries);
	TEST_CHECK(extra == 3);
	TEST_CHECK(strcmp(args3[extra], "extra1") == 0);
}

static void equals_values(void)
{
	const char *foo = NULL;
	const char *fnord = NULL;
	struct option_entry entries[] = {
		{"foo", 'f', NULL, OPTION_FLAG_string, .string = &foo},
		{"fnord", 'n', NULL, OPTION_FLAG_string, .string = &fnord},
		{NULL},
	};
	const char *args[] = {"program", "-f=wibble", "--fnord=blah"};

	TEST_CHECK(opt_parse(3, args, entries) >= 0);
	TEST_CHECK(strcmp(fnord, "blah") == 0);
	TEST_CHECK(strcmp(foo, "wibble") == 0);
}

TEST_LIST = {
	{"simple", simple_args},
	{"split string", split_string},
	{"bad strings", bad_strings},
	{"combined split & parse", combined},
	{"required params", required},
	{"extra params", extra_params},
	{"equals values", equals_values},
	{NULL, NULL},
};
