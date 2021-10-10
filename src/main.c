#include <stdio.h>
#include <string.h>

#include "main.h"
#include "argparser.h"

int
main(int argc, char *argv[])
{
	struct arg_option_param p_add[] = {
		P_STR_REQ("<money>"), P_STR_REQ("<last_4_digits>"), 
		P_STR_REQ("<item>"), P_STR_REQ("<remark>"), 
		P_END()
	};
	struct arg_option_param p_fetch[] = { 
		P_STR_OPT("[yymmdd]"), 
		P_END() 
	};
	struct arg_option_param p_delete[] = { 
		P_STR_REQ("<serial_no>"), 
		P_END() 
	};
	struct arg_option_param p_help[] = {
		P_STR_OPT("[option]"),
		P_END()
	};
	struct arg_option_param p_sort[] = { 
		P_STR_REQ("<new/old/high/low>"), 
		P_END() 
	};
	struct arg_option_param p_from[] = { 
		P_STR_REQ("<yymmdd/yymm/yyww/yy>"), 
		P_END() 
	};
	struct arg_option_param p_to[] = { 
		P_STR_REQ("<yymmdd/yymm/yyww/yy>"), 
		P_END() 
	};

    struct arg_option arg_options[] = {
		O_FUNC({'h'}, "help", "", 1, p_help, "Prints help message"),
		O_FUNC({'a'}, "add", "", 4, p_add, "Adds an expense or income record"),
		O_FUNC({'f'}, "fetch", "", 1, p_fetch, 
				"Fetches all records of the specified day or today"),
		O_FUNC({'d'}, "delete", "", 1, p_delete, "Deletes record of the given serial number"),
		O_FUNC({ 0 }, "sort", "", 1, p_sort, "Sorts records in the given order"),
		O_FUNC({ 0 }, "from", "", 1, p_from,
				"Provides a start point for range operations (inclusive)"),
		O_FUNC({ 0 }, "to", "", 1, p_to,
			"Provides a finish point for range operations (inclusive)"),
		O_BOOL({'e'}, "expense", "Does expense-related operations only"),
		O_BOOL({'i'}, "income", "Does income-related operations only"),
		O_BOOL({'w'}, "week", "Signals the date string in format of yyww"),
		O_BOOL({'v'}, "verbose", "Prints verbose messages"),
		O_BOOL({ 0 }, "now", "Gets today's date information: year, month, week, date"),
		O_END()
	};

	argparser_init(arg_options);

	int res = parse_arg(arg_options, argc, argv);

	if (res)
	{
		printf("All good\n");
		int b_size, f_size, a_size;
		get_bool_options_size(&b_size);
		get_func_options_size(&f_size);
		get_in_args_size(&a_size);
		printf("bool: %d func: %d in_args: %d\n", b_size, f_size, a_size);
	}
	else
		printf("Parse failure\n");

	

	return 0;
}
