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
#include <windows.h>

#include "argparser.h"

// Maintains option grouping data
struct group
{
	char const *name;
	struct arg_option const **opts;
	int size_opts;
};

// Maintains argparser internal data
struct memory
{
	struct arg_option const *opts;
	int size_opts;
	struct group *grps;
	int size_grps;
	int *flags, *bflags;
	char **params;
	int size_flags, size_bflags, size_params;
	char *version;
};

// Maintains internal storage of critical data
static struct memory *mem = NULL;

static int in_flags(int index);
static int in_bflags(int index);
static void add_to_flags(int index);
static void add_to_bflags(int index);
static void add_to_params(char *param);
static void group_options(struct arg_option const *os, int const size);
static int has_duplicated_ids(struct arg_option const *os, int const size);
static int is_valid_option(struct arg_option const *o);
static int is_end_mark(struct arg_option const *o);
static int has_alnum_text(char const *str);
static int has_text(char const *str);

static void argparser_error(char const *format, ...);

void
argparser_setup(struct arg_option const *options, char *version)
{
	static unsigned int called = 0;
	if (0 != called)
		argparser_error("%s() should not be called twice", __func__);

	mem = (struct memory *) calloc(1, sizeof(struct memory));
	if (NULL == mem)
		argparser_error("Memory not allocated in %s()", __func__);

	if (NULL == options)
		argparser_error("The input `options` is NULL in %s()", __func__);
	
	if (NULL == version)
		argparser_error("The input `version` is NULL in %s()", __func__);
	
	if (0 == has_text(version))
		argparser_error("Version cannot be empty string.");
	
	int size_opts = 0;
	struct arg_option const *copy = options;
	while (0 == is_end_mark(copy))
	{
		size_opts++;
		copy += 1;
	}

	for (int i = 0; i < size_opts; i++)
		if (0 == is_valid_option(options + i))
			argparser_error("Options validation fails in %s() due to invalid "
			"format of options[%d]", __func__, i);
	
	if (has_duplicated_ids(options, size_opts))
		argparser_error("Options validation fails in %s() due to duplicated "
		"identifiers declared", __func__);
	
	group_options(options, size_opts);
	
	mem->opts = options;
	mem->size_opts = size_opts;
	mem->version = version;

	called++;
}

void 
argparser_parse(int argc, char **argv)
{
	static unsigned int called = 0;
	if (0 != called)
		argparser_error("%s() should not be called twice", __func__);

	if (argc < 0 || NULL == argv)
		argparser_error("Invalid argc or argv");
	
	if (NULL == mem)
		argparser_error("Incorrect argparser sertup");
	
	for (int i = 1; i < argc; i++)
	{
		char *arg = argv[i];
		int size_arg = strlen(arg) + 1;

		if (size_arg >= 3)
		{
			if ('-' == arg[0])
			{
				if ('-' == arg[1])
				{
					if (3 == size_arg)
						argparser_error("Invalid argument, --");

					bool found = false;
					for (int j = 0; j < mem->size_opts; j++)
					{
						struct arg_option o = mem->opts[j];
						if (0 == strcmp(&arg[2], o.s_long))
						{
							if (o.b && 0 == in_bflags(j))
								add_to_bflags(j);
							else if (!o.b && 0 == in_flags(j))
								add_to_flags(j);

							found = true;
							break;
						}
					}

					if (!found)
						argparser_error("Unknown flag, %s", arg);
				}
				else
				{
					int pos = 1;
					while ('\0' != arg[pos])
					{
						bool found = false;
						for (int j = 0; j < mem->size_opts; j++)
						{
							struct arg_option o = mem->opts[j];
							if (arg[pos] == o.ch_short)
							{
								if (o.b && 0 == in_bflags(j))
									add_to_bflags(j);
								else if (!o.b && 0 == in_flags(j))
									add_to_flags(j);

								found = true;
								break;
							}
						}
						
						if (!found)
							argparser_error("Unknown flag, %s", arg);
						
						pos++;
					}
				}
			}
			else
			{
				bool found = false;
				for (int j = 0; j < mem->size_opts; j++)
				{
					struct arg_option o = mem->opts[j];
					if (0 == strcmp(arg, o.s_keyword))
					{
						if (o.b && 0 == in_bflags(j))
							add_to_bflags(j);
						else if (!o.b && 0 == in_flags(j))
							add_to_flags(j);
						
						found = true;
						break;
					}
				}
				
				if (!found)
					add_to_params(arg);
			}
		}
		else
		{
			if ('-' == arg[0])
				argparser_error("Invalid argument, -");
			
			add_to_params(arg);
		}
	}
}

void 
argparser_flags(int *size, int *arr)
{
	if (NULL == size)
		argparser_error("The input `size` cannot be NULL");

	if (NULL == mem)
		argparser_error("Incorrect argparser sertup");
	
	*size = mem->size_flags;
	arr = mem->flags;
}

void 
argparser_bflags(int *size, int *arr)
{
	if (NULL == size)
		argparser_error("The input `size` cannot be NULL");

	if (NULL == mem)
		argparser_error("Incorrect argparser sertup");
	
	*size = mem->size_bflags;
	arr = mem->bflags;
}

void 
argparser_params(int *size, char **arr)
{
	if (NULL == size)
		argparser_error("The input `size` cannot be NULL");

	if (NULL == mem)
		argparser_error("Incorrect argparser sertup");
	
	*size = mem->size_params;
	arr = mem->params;
}

char *
argparser_help()
{
	// TODO;
	return NULL;
}

char *
argparser_opt_help(char const *id)
{
	if (NULL == id)
		argparser_error("The input `id` cannot be NULL");
	
	if (NULL == mem)
		argparser_error("Incorrect argparser setup");
	
	int ind = -1;
	int const size_id = strlen(id) + 1;
	for (int i = 0; i < mem->size_opts; i++)
	{
		struct arg_option o = mem->opts[i];
		if (2 == size_id && id[0] == o.ch_short)
		{
			ind = i;
			break;
		}

		if (0 == strcmp(id, o.s_long) || 0 == strcmp(id, o.s_keyword))
		{
			ind = i;
			break;
		}
	}

	if (-1 == ind) return NULL;
	
	// struct arg_option o = mem->opts[ind];
	
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int ret = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	if (0 != ret)
	{
		// TODO: formatting
		printf("Console buffer width: %d\n", csbi.dwSize.X);
		printf("Console buffer height: %d\n", csbi.dwSize.Y);
	}

	return NULL;
}

char *
argparser_version()
{
	return mem->version;
}

void 
argparser_clean()
{
	if (NULL != mem)
	{
		if (NULL != mem->grps)
		{
			for (int i = 0; i < mem->size_grps; i++)
				if (NULL != mem->grps[i].opts)
					free(mem->grps[i].opts);
			free(mem->grps);
		}

		if (NULL != mem->flags)
			free(mem->flags);
		
		if (NULL != mem->bflags)
			free(mem->bflags);
		
		if (NULL != mem->params)
			free(mem->params);

		free(mem);
	}
}

/**
 * Returns 1 if the given index is already in flag pool, 0 otherwise
 */
static int
in_flags(int index)
{
	for (int i = 0; i < mem->size_flags; i++)
		if (index == mem->flags[i])
			return 1;
	return 0;
}

/**
 * Returns 1 if the given index is already in boolean flag pool, 0 otherwise
 */
static int
in_bflags(int index)
{
	for (int i = 0; i < mem->size_bflags; i++)
		if (index == mem->bflags[i])
			return 1;
	return 0;
}

/**
 * Adds the index of the corresponding option to flag pool.
 */
static void
add_to_flags(int index)
{
	if (NULL == mem->flags)
	{
		mem->size_flags = 1;
		mem->flags = (int *) calloc(1, sizeof(int));
		if (NULL == mem->flags)
			argparser_error("Memory not allocated during parsing");
		
		mem->flags[0] = index;
	}
	else
	{
		int pos = mem->size_flags++;
		int s = mem->size_flags * sizeof(int);
		mem->flags = (int *) realloc(mem->flags, s);
		if (NULL == mem->flags)
			argparser_error("Memory not reallocated during parsing");
		
		mem->flags[pos] = index;
	}
}

/**
 * Adds the index of the corresponding option to boolean flag pool.
 */
static void
add_to_bflags(int index)
{
	if (NULL == mem->bflags)
	{
		mem->size_bflags = 1;
		mem->bflags = (int *) calloc(1, sizeof(int));
		if (NULL == mem->bflags)
			argparser_error("Memory not allocated during parsing");
		
		mem->bflags[0] = index;
	}
	else
	{
		int pos = mem->size_bflags++;
		int s = mem->size_bflags * sizeof(int);
		mem->bflags = (int *) realloc(mem->bflags, s);
		if (NULL == mem->bflags)
			argparser_error("Memory not reallocated during parsing");
		
		mem->bflags[pos] = index;
	}
}

/**
 * Adds the param to the parameter pool.
 */
static void
add_to_params(char *param)
{
	if (NULL == mem->params)
	{
		mem->size_params = 1;
		mem->params = (char **) calloc(1, sizeof(char *));
		if (NULL == mem->params)
			argparser_error("Memory not allocated during parsing");
		
		mem->params[0] = param;
	}
	else
	{
		int pos = mem->size_params++;
		int s = mem->size_params * sizeof(char *);
		mem->params = (char **) realloc(mem->params, s);
		if (NULL == mem->params)
			argparser_error("Memory not reallocated during parsing");
		
		mem->params[pos] = param;
	}
}

/**
 * Groups all the options based on their group names.
 */
static void
group_options(struct arg_option const *os, int const size)
{
	static unsigned int called = 0;
	if (0 != called)
		argparser_error("%s() should not be called twice", __func__);

	int const SIZE_A = sizeof(struct arg_option *);

	for (int i = 0, size_grps = 0; i < size; i++)
	{
		char const *name = (os + i)->s_group;
		if (0 == has_text(name))
			name = "Options"; // default group name
		
		if(NULL == mem->grps)
		{
			size_grps = 1;
			mem->size_grps = 1;
			mem->grps = (struct group *) calloc(1, sizeof(struct group));
			if (NULL == mem->grps)
				argparser_error("Memory not allocated during grouping");

			mem->grps[0].name = name;
			mem->grps[0].size_opts = 1;

			mem->grps[0].opts = (struct arg_option const **) malloc(SIZE_A);
			if (NULL == mem->grps[0].opts)
				argparser_error("Memory not allocated during grouping");

			mem->grps[0].opts[0] = os;
		}
		else
		{
			int ind_grp = -1;
			for (int j = 0; j < size_grps; j++)
				if (0 == strcmp(name, mem->grps[j].name))
					ind_grp = j;
			if (-1 != ind_grp)
			{
				int ind = mem->grps[ind_grp].size_opts++;
				int s = (ind + 1) * SIZE_A;
				mem->grps[ind_grp].opts = 
					(struct arg_option const **) realloc(mem->grps[ind_grp].opts, s);
				if (NULL == mem->grps[ind_grp].opts)
					argparser_error("Memory not reallocated during grouping");

				mem->grps[ind_grp].opts[ind] = os + i;
			}
			else
			{
				ind_grp = mem->size_grps++;
				int s = (ind_grp + 1) * sizeof(struct group);
				mem->grps = (struct group *) realloc(mem->grps, s);
				if (NULL == mem->grps)
					argparser_error("Memory not reallocated during grouping");
				
				mem->grps[ind_grp].name = name;
				mem->grps[ind_grp].size_opts = 1;

				mem->grps[ind_grp].opts = (struct arg_option const **) malloc(SIZE_A);
				if (NULL == mem->grps[ind_grp].opts)
					argparser_error("Memory not allocated during grouping");

				mem->grps[ind_grp].opts[0] = os + i;
			}
		}
	}

	called++;
}

/**
 * Returns 1 if duplicated identifiers are found, 0 otherwise.
 */
static int 
has_duplicated_ids(struct arg_option const *os, int const size)
{
	for (int i = 0; i < size; i++)
	{
		struct arg_option const *new = os + i;
		for (int j = 0; j < i; j++)
		{
			struct arg_option const *old = os + j;
			
			char new_short = new->ch_short;
			char old_short = old->ch_short;
			if (new_short != '\0' && new_short == old_short)
				return 1;

			if (strcmp(new->s_long, "") != 0 && strcmp(new->s_long, old->s_long) == 0)
				return 1;

			if (strcmp(new->s_keyword, "") != 0 && strcmp(new->s_keyword, old->s_keyword) == 0)
				return 1;
		}
	}

	return 0;
}

/**
 * Returns 1 if the given option is valid in format, 0 otherwise.
 */
static int 
is_valid_option(struct arg_option const *o)
{
	if (0 != o->ch_short && 0 == isalnum(o->ch_short))
		return 0;
	
	if (has_text(o->s_long) && 0 == has_alnum_text(o->s_long))
		return 0;

	if (has_text(o->s_keyword) && 0 == has_alnum_text(o->s_keyword))
		return 0;

	bool has_id = false;
	if (0 != o->ch_short || has_text(o->s_long) || has_text(o->s_keyword))
		has_id = true;
	
	if (false == has_id)
		return 0;

	if (false == o->b && 0 == has_text(o->s_hint))
		return 0;

	if (o->b && has_text(o->s_hint))
		return 0;

	if (0 == has_text(o->s_desc))
		return 0;
	
	return 1;
}

/**
 * Returns 1 if the given option is the end mark, 0 otherwise.
 */
static int
is_end_mark(struct arg_option const *o)
{
	if (o == NULL)
		argparser_error("Option cannot be NULL");

	return o->b == false && o->ch_short == 0 && has_text(o->s_long) == 0 
		&& has_text(o->s_keyword) == 0 && has_text(o->s_hint) == 0 
		&& has_text(o->s_desc) == 0 && has_text(o->s_group) == 0;
}

/**
 * Validates if input string has only alphabets and digits
 */
static int
has_alnum_text(char const *str)
{
	if (0 == has_text(str))
		return 0;

	int pos = 0;
	while (str[pos] != '\0')
		if (0 == isalnum(str[pos++]))
			return 0;
	
	return 1;
}

/**
 * Validates if input string has actual text.
 */
static int
has_text(char const *str)
{
	if (str == NULL || strlen(str) == 0)
		return 0;
	
	int flag = 0;
	for (int pos = 0; str[pos] != '\0'; pos++)
		if (str[pos] != ' ')
		{
			flag = 1;
			break;
		}

	return flag;
}

/**
 * Prints error message and exits program.
 */
static void
argparser_error(char const *format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	va_end(args);

	exit(1);
}


int argparser_debug_size_opts()
{
	return NULL == mem ? -1 : mem->size_opts;
}