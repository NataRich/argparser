/**
 * argparser - Argument parser library implementations
 *
 * MIT License
 *
 * Copyright (c) 2021 Zihan Zhao
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "argparser.h"

#define MAX_BUFFER 1024

/**
 * Prints error message and exits program
 */
static void
argparser_error(char const *format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);

	exit(1);
}

/**
 * Validates if input string has actual text
 */
static int
has_text(char const *const str)
{
	return str != NULL && strlen(str) != 0;
}

/**
 * Validates if input string has only alphabets or numbers
 */
static int
has_only_alnum(char const *const str)
{
	if (!has_text(str))
		return 0;

	int pos = 0;
	while (str[pos] != '\0')
		if (!isalnum(str[pos++]))
			return 0;
	
	return 1;
}

/**
 * Gets the size of parameter hints
 */
static int
get_hints_size(char const * const*s_hints)
{
	int size = 0;
	while (s_hints[size] != NULL && size < MAX_NPARAM)
		size++;
	return size;
}

/**
 * Validates an arg_option structure
 */
static void
validate_option(struct arg_option const *const opt)
{
	static int _ind = 0;

	// Validates option identifiers
	int id_exist = 0;
	for (int i = 0; i < MAX_NFLAGS; i++)
	{
		if (opt->c_shorts[i] != '\0')
		{
			if (isalnum(opt->c_shorts[i]))
			{
				id_exist = 1;
				break;
			}
			else
			{
				argparser_error("Error: Option[%d].c_shorts[%d] must be alnum only",
						_ind, i);
			}
		}
	}

	if (!id_exist && has_text(opt->s_long))
	{
		if (strlen(opt->s_long) <= MAX_STRLEN - 1)
		{
			if (has_only_alnum(opt->s_long))
			{
				id_exist = 1;
			}
			else
			{
				argparser_error("Error: Option[%d].s_long must be alnum only", _ind);
			}
		}
		else
		{
			argparser_error("Error: Option[%d].s_long must be shorter than %d chars", 
					_ind, MAX_STRLEN);
		}
	}

	if (!id_exist && has_text(opt->s_keyword))
	{
		if (strlen(opt->s_keyword) <= MAX_STRLEN - 1)
		{
			if (has_only_alnum(opt->s_keyword))
			{
				id_exist = 1;
			}
			else
			{
				argparser_error("Error: Option[%d].s_keyword must be alnum only", _ind);
			}
		}
		else
		{
			argparser_error("Error: Option[%d].s_keyword must be shorter than %d chars", 
					_ind, MAX_STRLEN);
		}
	}

	if (!id_exist)
	{
		argparser_error("Error: Option[%d] must have at least one identifier", _ind);
	}

	// Validates number of params
	int n_params = opt->n_params;
	if (n_params < 0)
	{
		if (n_params != NINF)
		{
			argparser_error("Error: Option[%d].n_params is invalid; use %d for variable length", 
					_ind, NINF);
		}
	}

	if (n_params > MAX_NPARAM)
	{
		argparser_error("Error: Option[%d].n_params should not exceed %d", _ind, MAX_NPARAM);
	}

	if (opt->s_hints[0] == NULL && n_params != 0)
	{
		if (n_params == NINF)
		{
			argparser_error("Error: Option[%d] expected 1 hint only but received none", _ind);
		}
		else
		{
			argparser_error("Error: Option[%d] expected %d hints but received none", 
					_ind, n_params);
		}
	}

	if (opt->s_hints[0] != NULL)
	{
		int rn_hints = get_hints_size(opt->s_hints);

		if (n_params == 0)
		{
			argparser_error("Error: Option[%d] expected 0 hints but received %d", 
					_ind, rn_hints);
		}

		if (n_params < 0 && rn_hints != 1)
		{
			argparser_error("Error: Option[%d] expected 1 hint but received %d", 
					_ind, rn_hints);
		}

		if (n_params > 0 && n_params != rn_hints)
		{
			argparser_error("Error: Option[%d] expected %d hints but received %d", 
					_ind, n_params, rn_hints);
		}
	}

	// Validates param content
	if (n_params < 0)
	{
		if (!has_text(opt->s_hints[0]))
		{
			argparser_error(
					"Error: Option[%d].s_hints[0] should contain valid help text",
					_ind);
		}
	}
	
	if (n_params > 0)
	{
		for (int i = 0; i < n_params; i++)
		{
			if (!has_text(opt->s_hints[i]))
			{
				argparser_error(
						"Error: Option[%d].params[%d].s_hint should contain valid help text",
						_ind, i);
			}
		}
	}

	// Validates help message
	if (!has_text(opt->s_desc))
	{
		argparser_error("Error: Option[%d].s_desc should contain valid text", _ind);
	}

	_ind++;
}

/**
 * Checks if input is ending mark
 */
static int
is_end_opt(struct arg_option const *const opt)
{
	if (opt == NULL)
	{
		argparser_error("Error: Option must not be NULL");
	}

	for (int i = 0; i < MAX_NFLAGS; i++)
		if (opt->c_shorts[i] != '\0')
			return 0;

	return !has_text(opt->s_long) && !has_text(opt->s_keyword) && opt->n_params == 0
		&& opt->s_hints[0] == NULL && !has_text(opt->s_desc);
}

/**
 * Gets the size of arg_options
 */
static int
get_options_size(struct arg_option const *const options)
{
	int size = 0;
	while (!is_end_opt(options + size))
		size++;
	return size;
}

void 
argparser_init(struct arg_option const *const options)
{
	if (options == NULL)
		argparser_error("Error: Argument options must not be NULL");

	int size = get_options_size(options);
	for (int i = 0; i < size; i++)
		validate_option(options + i);

	for (int i = 0; i < size; i++)
	{
		struct arg_option const *const new = options + i;
		for (int j = 0; j < i; j++)
		{
			struct arg_option const *const old = options + j;
			for (int k = 0; k < MAX_NFLAGS; k++)
			{
				char new_short = new->c_shorts[k];
				if (new_short == '\0')
					continue;

				for (int l = 0; l < MAX_NFLAGS; l++)
				{
					char old_short = old->c_shorts[l];
					if (old_short == '\0')
						break;

					if (new_short == old_short)
					{
						argparser_error("Error: Option[%d].c_shorts[%d] and [%d].c_shorts[%d]"
								" had duplicate value '%c'", i, k, j, l, new_short);
					}
				}
			}

			if (strcmp(new->s_long, "") != 0 && strcmp(new->s_long, old->s_long) == 0)
			{
				argparser_error("Error: Option[%d] and [%d] had duplicate .s_long \"%s\"", 
						i, j, new->s_long);
			}

			if (strcmp(new->s_keyword, "") != 0 && strcmp(new->s_keyword, old->s_keyword) == 0)
			{
				argparser_error("Error: Option[%d] and [%d] had duplicate .s_keyword \"%s\"", 
						i, j, new->s_keyword);
			}
		}
	}
}

static int ind_func[MAX_BUFFER] = { -1 };
static int n_ind_func = 0;
static int ind_bool[MAX_BUFFER] = { -1 };
static int n_ind_bool = 0;
static char *in_arg[MAX_BUFFER] = { 0 };
static int n_in_arg = 0;

/**
 * Checks if input arg_option is a bool option
 */
static int
is_bool_opt(struct arg_option const *const opt)
{
	if (opt == NULL)
	{
		argparser_error("Error: Option must not be null\n\t"
				"in %s (at %d line) in %s", __func__, __LINE__, __FILE__);
	}

	return opt->n_params == 0;
}

/**
 * Adds to bool option pool
 */
static void
add_bool(int ind)
{
	ind_bool[n_ind_bool++] = ind;
	if (n_ind_bool >= MAX_BUFFER)
	{
		argparser_error("Error: Bool buffer overflow\n\t"
				"in %s (at %d line) in %s", __func__, __LINE__, __FILE__);	
	}
}

/**
 * Adds to func option pool
 */
static void
add_func(int ind)
{
	ind_func[n_ind_func++] = ind;
	if (n_ind_func >= MAX_BUFFER)
	{
		argparser_error("Error: Func buffer overflow\n\t"
				"in %s (at %d line) in %s", __func__, __LINE__, __FILE__);	
	}
}

/**
 * Searches the given string in c_shorts, s_long, and s_keyword for a match
 */
static int
search(struct arg_option const *const opts, int opts_size, char const *const arg)
{
	int found = 0;

	if (arg[0] == '-')
	{
		if (arg[1] == '-')
		{
			for (int i = 0; i < opts_size; i++)
			{
				if (strcmp(&arg[2], opts[i].s_long) == 0)
				{
					if (is_bool_opt(opts + i))
						add_bool(i);
					else
						add_func(i);
					
					found = 1; // set found
					break;
				}
			}

			if (found == 0)
			{
				argparser_error("Error: Unknown option %s", arg);
			}
		}
		else
		{
			int len = strlen(arg);
			int pos = 1;
			
			while (pos < len)
			{
				int flag = 0;
				for (int i = 0; i < opts_size; i++)
				{
					for (int j = 0; j < MAX_NFLAGS; j++)
					{
						if (opts[i].c_shorts[j] == '\0')
							break;

						if (opts[i].c_shorts[j] == arg[pos])
						{
							if (is_bool_opt(opts + i))
								add_bool(i);
							else
								add_func(i);
						
							pos++;
							flag = 1;
							break;
						}
					}
					
					if (flag)
						break;
				}

				if (!flag)
				{
					argparser_error("Error: Unknown option -%c", arg[pos]);
				}
			}

			found = 1;
		}

	}
	else
	{
		for (int i = 0; i < opts_size; i++)
		{
			if (strcmp(opts[i].s_keyword, arg) == 0)
			{
				if (is_bool_opt(opts + i))
					add_bool(i);
				else
					add_func(i);
				
				found = 1; // set found
				break;
			}
		}
	}

	return found;
}

int 
parse_arg(struct arg_option const *const options, int argc, char **argv)
{
	int opt_size = get_options_size(options);
	for (int i = 1; i < argc; i++)
	{
		if (search(options, opt_size, argv[i]) == 0)
		{
			in_arg[n_in_arg++] = argv[i];
			if (n_in_arg >= MAX_BUFFER)
			{
				argparser_error("Error: Argument buffer overflow\n\t"
						"in %s (at %d line) in %s", __func__, __LINE__, __FILE__);
			}
		}

	}

	return 1;
}

void
get_func_options_size(int *size)
{
	*size = n_ind_func;
}

void 
get_func_options(int *ind_funcs)
{
	for (int i = 0; i < n_ind_func; i++)
		ind_funcs[i] = ind_func[i];
}

void
get_bool_options_size(int *size)
{
	*size = n_ind_bool;
}

void
get_bool_options(int *ind_bools)
{
	for (int i = 0; i < n_ind_bool; i++)
		ind_bools[i] = ind_bool[i];
}

void
get_in_args_size(int *size)
{
	*size = n_in_arg;
}

void
get_in_args(char **in_args)
{
	for (int i = 0; i < n_in_arg; i++)
		in_args[i] = in_arg[i];
}

/**
 * Formats help message of a specific option
 */
static void
format_help(struct arg_option const *const opt, char *help)
{
	char buffer[MAX_BUFFER] = { 0 };
	strcat(buffer, "    ");

	for (int i = 0; i < MAX_NFLAGS; i++)
	{
		char ch = opt->c_shorts[i];
		if (ch == '\0')
			break;

		char temp[10];
		sprintf(temp, "-%c, ", ch);
		strcat(buffer, temp);
	}

	if (strcmp(opt->s_long, "") != 0)
	{
		char temp[MAX_STRLEN];
		sprintf(temp, "--%s, ", opt->s_long);
		strcat(buffer, temp);
	}

	if (strcmp(opt->s_keyword, "") != 0)
	{
		char temp[MAX_STRLEN];
		sprintf(temp, "%s, ", opt->s_keyword);
		strcat(buffer, temp);
	}

	buffer[strlen(buffer) - 2] = '\0';

	if (opt->n_params < 0)
	{
		char temp[50];
		sprintf(temp, "    %s", opt->s_hints[0]);
		strcat(buffer, temp);
	}
	else if (opt->n_params > 0)
	{
		strcat(buffer, "    ");

		for (int i = 0; i < opt->n_params; i++)
		{
			char temp[50];
			sprintf(temp, "%s ", opt->s_hints[i]);
			strcat(buffer, temp);
		}
	}

	strcat(buffer, "\n        ");
	strcat(buffer, opt->s_desc);
	strcat(buffer, "\n\n");

	strcat(help, buffer);
}

void 
get_help(struct arg_option const *const opts, char *help)
{
	char f_buffer[MAX_BUFFER] = { 0 };
	char b_buffer[MAX_BUFFER] = { 0 };
	
	strcat(f_buffer, "Functional Options: \n");
	strcat(b_buffer, "Bool Options: \n");

	int size = get_options_size(opts);
	for (int i = 0; i < size; i++)
	{
		if (opts[i].n_params == 0)
			format_help(opts + i, b_buffer);
		else
			format_help(opts + i, f_buffer);
	}

	strcat(help, f_buffer);
	strcat(help, b_buffer);
}

void 
get_opt_help(struct arg_option const *const opt, char *help)
{
	if (opt->n_params == 0)
		strcat(help, "Bool Option:\n");
	else
		strcat(help, "Functional Option: \n");

	format_help(opt, help);
}
