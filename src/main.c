#include <stdio.h>
#include <string.h>

#include "main.h"
#include "argparser.h"

int
main(int argc, char *argv[])
{
    struct arg_option arg_options[] = {
		{ 
			false, 'e', "expense", "", "<money> <card> <item> <remark>", 
			"Adds an expense record" 
		},
		{
			false, 'i', "income", "", "<money> <card> <item> <remark>",
			"Adds an income record"
		},
		{ 
			false, 'f', "fetch", "", "[yymmdd]", 
			"Fetches all records of the specified day or today" 
		},
		{ 
			false, 'd', "delete", "", "<serialno>", 
			"Deletes record of the given serial number" 
		},
		{ 
			false, 0, "sort", "", "<new/old/high/low>", 
			"Sorts records in the given order" 
		},
		{ 
			false, 0, "from", "", "<datestr>",
			"Provides a start point for range operations (inclusive)" 
		},
		{ 
			false, 0, "to", "", "<datestr>",
			"Provides a finish point for range operations (inclusive)" 
		},
		{ true, 'h', "help", "", "", "Prints help message" },
		{ true, 'w', "week", "", "", "Signals the date string in format of yyww" },
		{ true, 'v', "verbose", "", "", "Prints verbose messages" },
		{ true, 0, "now", "", "", "Gets today's date information: year, month, week, date" },
		O_END()
	};

	argparser_setup(arg_options, "v1.0.0");

	argparser_parse(argc, argv);

	int size_bf = 0, size_f = 0, size_p = 0;
	int *bf = NULL, *f = NULL;
	char **p = NULL;

	argparser_flags(&size_f, f);
	argparser_bflags(&size_bf, bf);
	argparser_params(&size_p, p);

	printf("size_flags: %d, size_bflags: %d, size_params: %d\n", size_f, size_bf, size_p);

	argparser_opt_help("f");

	argparser_clean();

	return 0;
}
