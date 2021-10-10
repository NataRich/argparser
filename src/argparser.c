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
	{
		return 0;
	}

	int pos = 0;
	while (str[pos] != '\0')
	{
		if (!isalnum(str[pos]))
		{
			return 0;
		}
		pos++;
	}

	return 1;
}

/**
 * Checks if input is the ending param
 */
static int
is_end_param(struct arg_option_param const *const p)
{
	if (p == NULL)
	{
		argparser_error("Error: Param must not be NULL");
	}

	return p->p_type == 0 && p->b_required == 0 && !has_text(p->s_hint);
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

	if (opt->params == NULL && n_params != 0)
	{
		if (n_params == NINF)
		{
			argparser_error("Error: Option[%d] expected 1 param only but received NULL", _ind);
		}
		else
		{
			argparser_error("Error: Option[%d] expected %d param(s) but received NULL", 
					_ind, n_params);
		}
	}

	if (opt->params != NULL)
	{
		int rn_params = 0;
		while (!is_end_param(opt->params + rn_params))
		{
			rn_params++;
		}

		if (n_params == 0)
		{
			argparser_error("Error: Option[%d] expected NULL param but received %d", 
					_ind, rn_params);
		}

		if (n_params < 0 && rn_params != 1)
		{
			argparser_error("Error: Option[%d] expected %d param(s) but received %d", 
					_ind, n_params, rn_params);
		}

		if (n_params > 0 && n_params != rn_params)
		{
			argparser_error("Error: Option[%d] expected %d param(s) but received %d", 
					_ind, n_params, rn_params);
		}
	}

	// Validates param content
	if (n_params < 0)
	{
		if (!has_text(opt->params[0].s_hint))
		{
			argparser_error(
					"Error: Option[%d].params[0].s_hint should contain valid help text",
					_ind);
		}
	}
	
	if (n_params > 0)
	{
		for (int i = 0; i < n_params; i++)
		{
			if (!has_text(opt->params[i].s_hint))
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
	{
		if (opt->c_shorts[i] != '\0')
		{
			return 0;
		}
	}

	return !has_text(opt->s_long) && !has_text(opt->s_keyword) && opt->n_params == 0
		&& opt->params == NULL && !has_text(opt->s_desc);
}

/**
 * Stores necessary variables
 */
// static struct argparser _vars;

void 
argparser_init(struct arg_option const *const arg_options)
{
	if (arg_options == NULL)
	{
		argparser_error("Error: Argument options must not be NULL");
	}

	int size = 0;
	while (!is_end_opt(arg_options + size))
	{
		size++;
	}

	for (int i = 0; i < size; i++)
	{
		validate_option(arg_options + i);
	}

	// TODO: option initialization	
}

int 
parse_arg(int argc, char const **const argv)
{
	// analysis
	return 1;
}

