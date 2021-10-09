#include <stdio.h>
#include <string.h>

#include "main.h"
#include "argparser.h"

int
main(int argc, char *argv[])
{
	struct arg_option_param p_add[] = {
		{T_STR, 1, "<money>"}, {T_STR, 1, "<last_4_digits>"},
		{T_STR, 1, "<item>"}, {T_STR, 1, "<remark>"}, { 0, 0, ""}
	};
	struct arg_option_param p_fetch[] = { {T_STR, 0, "?<yymmdd>"}, { 0, 0, ""} };
	struct arg_option_param p_from[] = { {T_STR, 1, "<yymmdd/yymm/yyww/yy>"}, { 0, 0, ""} };
	struct arg_option_param p_to[] = { {T_STR, 1, "<yymmdd/yymm/yyww/yy>"}, { 0, 0, ""} };
	struct arg_option_param p_delete[] = { {T_STR, 1, "<serial_no>"}, { 0, 0, ""} };
	struct arg_option_param p_sort[] = { {T_STR, 1, "new/old/high/low"}, { 0, 0, ""} };

    struct arg_option arg_options[] = {
		{
			{'a'}, "add", "", 4, p_add,
			"Adds an expense or income record",
		},
		{
			{'f'}, "fetch", "", 1, p_fetch,
			"Fetches all records of the specified day or today",
		},
		{
			{'d'}, "delete", "", 1, p_delete,
			"Deletes record of the given serial number",
		},
		{
			{ 0 }, "sort", "", 1, p_sort,
			"Sorts records in the given order",
		},
		{
			{ 0 }, "from", "", 1, p_from,
			"Provides a start point for range operations (inclusive)",
		},
		{
			{ 0 }, "to", "", 1, p_to,
			"Provides a finish point for range operations (inclusive)",
		},
		// boolean options 
		{
			{'e'}, "expense", "", 0, NULL,
			"Does expense-related operations only",
		},
		{
			{'i'}, "income", "", 0, NULL,
			"Does income-related operations only",
		},
		{
			{'w'}, "week", "", 0, NULL,
			"Signals the date string in format of yyww",
		},
		{
			{'v'}, "verbose", "", 0, NULL,
			"Prints verbose messages",
		},
		{
			{ 0 }, "now", "", 0, NULL,
			"Gets today's date information: year, month, week, date",
		},
		// ending mark
		{
			{ 0 }, "", "", 0, NULL, ""
		}
	};

	argparser_init(arg_options, argc, argv);

	printf("All good\n");

	return 0;
}
