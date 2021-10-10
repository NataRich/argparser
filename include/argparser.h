/**
 * argparser - Argument parser library 
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

#ifndef ARGPARSER_INCLUDE_ARGPARSER_H_
#define ARGPARSER_INCLUDE_ARGPARSER_H_

#define MAX_NFLAGS 3
#define MAX_STRLEN 20
#define MAX_NPARAM 10
#define NINF -1

#define O_END() { { 0 }, "", "", 0, { 0 }, "" }

/**
 * Argument option parameter type definitions
 */
enum p_type
{
	T_INT = 10,
	T_DOUBLE,
	T_STR
};

/**
 * Argument option parameter structure
 */
struct arg_option_param
{
	enum p_type p_type;
	unsigned int b_required;
	char const *s_hint;
};

/**
 * Argument option structure
 */
struct arg_option
{
	char const c_shorts[MAX_NFLAGS];
	char const s_long[MAX_STRLEN];
	char const s_keyword[MAX_STRLEN];
	int n_params;
	char const *s_hints[MAX_NPARAM];
	char const *const s_desc;
};

/**
 * Validates the given arg_options
 */
void argparser_init(struct arg_option const *const options);

/**
 * Parses arguments
 */
int parse_arg(struct arg_option const *const options, int argc, char **argv);

/**
 * Gets the number of function options
 */
void get_func_options_size(int *size);

/**
 * Gets function options
 */
void get_func_options(int *ind_funcs);

/**
 * Gets the number of bool options
 */
void get_bool_options_size(int *size);

/**
 * Gets boolean options
 */
void get_bool_options(int *ind_bools);

/**
 * Gets the number of input arguments
 */
void get_in_args_size(int *size);

/**
 * Gets input arguments
 */
void get_in_args(char **in_args);

/**
 * Gets help message
 */
void get_help(struct arg_option const *const opts, char *help);

/**
 * Gets help message of a specific option
 */
void get_opt_help(struct arg_option const *const opt, char *help);

#endif
