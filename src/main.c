#include <stdio.h>
#include <string.h>

#include "main.h"
#include "argparser.h"

int
main(int argc, char *argv[])
{
    struct arg_option arg_options[] = {
		{ {'h'}, "help", "", 1, { "[option]" }, "Prints help message" },
		{ 
			{'a'}, "add", "", 4, { "<money>", "<card>", "<item>", "<remark>" }, 
			"Adds an expense or income record" 
		},
		{ 
			{'f'}, "fetch", "", 1, { "[yymmdd]" }, 
			"Fetches all records of the specified day or today" 
		},
		{ 
			{'d'}, "delete", "", 1, { "<serialno>" }, 
			"Deletes record of the given serial number" 
		},
		{ 
			{ 0 }, "sort", "", 1, { "<new/old/high/low>" }, 
			"Sorts records in the given order" 
		},
		{ 
			{ 0 }, "from", "", 1, { "<datestr>" },
			"Provides a start point for range operations (inclusive)" 
		},
		{ 
			{ 0 }, "to", "", 1, { "<datestr>" },
			"Provides a finish point for range operations (inclusive)" 
		},
		{ {'e'}, "expense", "", 0, { 0 }, "Does expense-related operations only" },
		{ {'i'}, "income", "", 0, { 0 }, "Does income-related operations only" },
		{ {'w'}, "week", "", 0, { 0 },"Signals the date string in format of yyww" },
		{ {'v'}, "verbose", "", 0, { 0 }, "Prints verbose messages" },
		{ { 0 }, "now", "", 0, { 0 }, "Gets today's date information: year, month, week, date" },
		O_END()
	};

	argparser_init(arg_options);

	int res = parse_arg(arg_options, argc, argv);

	if (res)
	{
		int b_size, f_size, a_size;
		get_bool_options_size(&b_size);
		get_func_options_size(&f_size);
		get_in_args_size(&a_size);
		printf("bool: %d func: %d in_args: %d\n", b_size, f_size, a_size);
		
		char help[1024] = { 0 };
		get_help(arg_options, help);
		printf("\n%s\n", help);

		char help1[1024] = { 0 };
		char help2[1024] = { 0 };
		get_opt_help(arg_options + 2, help1);
		get_opt_help(arg_options + 8, help2);
		printf("\n%s\n\n%s\n", help1, help2);
		
		printf("All good\n");
	}
	else
		printf("Parse failure\n");

	

	return 0;
}
