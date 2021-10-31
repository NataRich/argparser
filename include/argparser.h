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

#include <stdbool.h>

/**
 * The end mark of option definition
 */
#define O_END() { 0, 0, "", "", "", "", "" }

/**
 * Argument option structure
 * 
 * This structure contains all the information needed to identify an option. 
 * Processing of comand-line arguments will be based on these options.
 */
struct arg_option
{
	/**
	 * true if this is a boolean option, false otherwise.
	 */
	bool const b;

	/**
	 * A unique char representing the option.
	 * 
	 * This is interpreted using '-', e.g. -f
	 */
	char const ch_short;

	/**
	 * A unique name representing the option.
	 * 
	 * This is interpreted using '--', e.g. --force
	 */
	char const *s_long;

	/**
	 * A unique plain text representing the option.
	 * 
	 * This is interpreted in plain text, e.g. force
	 */
	char const *s_keyword;

	/**
	 * A hint that tells user how to provide parameter(s).
	 * 
	 * This is required if this is not a boolean option, while this must be 
	 * empty if this is a boolean option. This appears in the help message.
	 */
	char const *s_hint;

	/**
	 * A brief description of the option about what it does.
	 * 
	 * This appears in the help message.
	 */
	char const *s_desc;

	/**
	 * A name that specifies to which group this option belongs.
	 * 
	 * This name does not have to be unique.
	 */
	char const *s_group;
};

/**
 * Sets up the project with the passed-in `options` and `version`.
 * 
 * This function checks the format and validity of the given options, including
 *   1) whether the elements of each option are in valid formats,
 *   2) whether the elements that should be unique across all the options are
 * 		actually unique, e.g. `ch_short`, `s_long` and `s_keyword`,
 * and tags the project with the given version. Internally, this function also
 * separates the options into different groups based on the provided `s_group`
 * for later convenience.
 * 
 * Note: argparser does not maintain a deep copy of the options anywhere. 
 * 		 Instead, it uses the address passed in directly. Thus, a good practice
 * 		 of working with argparser is to declare a globally accessible and 
 * 		 unmodifiable `struct arg_option *` variable, e.g. using `const` and/or
 * 		 `static` keywords, etc.
 */
void argparser_setup(struct arg_option const *options, char *version);

/**
 * Parses command-line arguments by the defined options.
 * 
 * This function does a number of things. First, it groups all the arguments
 * based on their types, e.g. executable, flag or parameter, and maintains 
 * deep copies of them internally, except for the executable. Then, it checks 
 * if there are any unknown or duplicated flags. If true, it will exits the 
 * program printing the source of error. Otherwise, it continues to separate
 * flags based on their booleanity. Finally, if nothing goes wrong, command-
 * line arguments have been successfully parsed.
 */
void argparser_parse(int argc, char **argv);

/**
 * Sets the index array of the options containing flags parsed from the
 * command-line arguments that are not boolean and its size.
 * 
 * This function returns the indices in order.
 */
void argparser_flags(int *size, int *arr);

/**
 * Sets the index array of the options containing boolean flags parsed from the
 * command-line arguments and its size.
 * 
 * This function returns the indices in order.
 */
void argparser_bflags(int *size, int *arr);

/**
 * Sets deep copies of parameters parsed from the command-line arguments and 
 * its size.
 * 
 * This function returns the parameters in order.
 */
void argparser_params(int *size, char **arr);

/**
 * Returns a formatted help message of all the previously defined options.
 * 
 * This function formats the help message by groups. The order of groups is
 * the same as the order of the first occurrence of different groups in the 
 * declaration. For example, the order of groups of the following declaration
 * is A > B > C > D.
 *   {..., "A"}, {..., "B"}, {..., "A"}, {..., "C"}, {..., "B"}, {..., "D"}
 * 
 * Note: The return value is dynamically allocated. Thus, user is responsible
 * 		 freeing the pointer.
 */
char *argparser_help();

/**
 * Returns a formatted help message of the option specified by the identifier.
 * 
 * Note: The return value is dynamically allocated. Thus, user is responsible
 * 		 freeing the pointer.
 */
char *argparser_opt_help(char const *id);

/**
 * Returns the version string.
 */
char *argparser_version();

/**
 * Frees up all the internally maintained space.
 */
void argparser_clean();

#endif