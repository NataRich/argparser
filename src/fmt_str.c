/**
 * fmt_str - String formatting library implementations
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

#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "fmt_str.h"

static int index_of_newline(char const *test, int begin_index);
static int index_of_char(char const *test, int begin_index);
static int index_of_delim(char const *test, int begin_index, int end_index);

int 
strwrap(char *dest, char const *src, size_t lw, char const *prefix, 
        char const *postfix)
{
    int len_src = strlen(src);
    int len_pre = 0, len_post = 0;
    if (NULL != prefix && 0 != strcmp(prefix, ""))
        len_pre = strlen(prefix);
    
    if (NULL != postfix && 0 != strcmp(postfix, ""))
        len_post = strlen(postfix);

    int len_extra = len_pre + len_post;
    int begin_index = 0, end_index = lw - 1 - len_extra;
    while (begin_index < len_src && end_index < len_src)
    {
        int delim_index = index_of_delim(src, begin_index, end_index);
        if (-1 == delim_index) // hard wrap
        {
            strcat(dest, prefix);
            strncat(dest, &src[begin_index], end_index - begin_index + 1);
            strcat(dest, postfix);
            strcat(dest, "\n");
            begin_index = index_of_char(src, end_index + 1);
        }
        else // normal wrap
        {
            strcat(dest, prefix);
            strncat(dest, &src[begin_index], delim_index - begin_index + 1);
            strcat(dest, postfix);
            strcat(dest, "\n");
            begin_index = index_of_char(src, delim_index + 1);
        }

        if (-1 == begin_index)
            return 1;

        end_index = begin_index + lw - 1 - len_extra;
    }

    if (begin_index < len_src)
    {
        strcat(dest, prefix);
        strcat(dest, &src[begin_index]);
        strcat(dest, postfix);
        strcat(dest, "\n");
    }

    return 1;
}

int 
strjoin(char *dest, char const *lstr, char const *rstr, int indent)
{
    int rbegin_index = 0, rend_index = 0;
    int lbegin_index = 0, lend_index = 0;
    int len_rstr = strlen(rstr);
    int len_lstr = strlen(lstr);

    while (rbegin_index < len_rstr)
    {
        if (lbegin_index < len_lstr)
        {
            lend_index = index_of_newline(lstr, lbegin_index);
            if (-1 == lend_index)
                lend_index = len_lstr - 1;
            strncat(dest, &lstr[lbegin_index], lend_index - lbegin_index);

            int rem = indent - (lend_index - lbegin_index);
            for (int i = 0; i < rem; i++)
                strcat(dest, " ");

            lbegin_index = lend_index + 1;
        }
        else
        {
            for (int i = 0; i < indent; i++)
                strcat(dest, " ");
        }
        
        rend_index = index_of_newline(rstr, rbegin_index);
        if (-1 == rend_index)
            rend_index = len_rstr - 1;
        strncat(dest, &rstr[rbegin_index], rend_index - rbegin_index + 1);
        rbegin_index = rend_index + 1;
    }

    if (lbegin_index < len_lstr)
        strcat(dest, &lstr[lbegin_index]);
    
    return 1;
}

/**
 * Returns the index of the first newline character from the beginning.
 */
static int
index_of_newline(char const *test, int begin_index)
{
    for (int i = begin_index; test[i] != '\0'; i++)
        if ('\n' == test[i])
            return i;
    return -1;
}

/**
 * Returns the index of the first alphabet, digit or non-whitespace from the 
 * beginning.
 */
static int
index_of_char(char const *test, int begin_index)
{
    for (int i = begin_index; test[i] != '\0'; i++)
        if (isalnum(test[i]) || ' ' != test[i])
            return i;
    return -1;
}

/**
 * Returns the index of the first delimiter from the end.
 * 
 * Every character that is not an alphabet, digit or open character is 
 * considered as a delimiter.
 */
static int
index_of_delim(char const *test, int begin_index, int end_index)
{
    char *open_chars = "<'\"[{(";
    int s = strlen(open_chars);
    for (int i = end_index; i > begin_index; i--)
    {
        if (0 == isalnum(test[i]))
        {
            bool is_open_char = false;
            for (int j = 0; j < s; j++)
            {
                if (test[i] == open_chars[j])
                {
                    is_open_char = true;
                    break;
                }
            }
            
            if (!is_open_char) return i;
        }
    }

    return -1;
}