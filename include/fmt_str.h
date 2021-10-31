/**
 * fmt_str - String formatting library 
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

#ifndef ARGPARSER_INCLUDE_FMT_STR_H_
#define ARGPARSER_INCLUDE_FMT_STR_H_

/**
 * Wraps the source string so that each line has a maximum of `size` chars,
 * including `prefix` and `postfix`, excluding the new line character at the
 * end of each line. Returns 0 if errors are encountered, 1 otherwise.
 * 
 * This function splits the source string at the very last delimiter char in
 * each line. If the delimiter is not found, it performs hardwrap at the last
 * character. If the source string is shorter than `size`, then `src` is 
 * directly copied to `dest` with return value 0.
 * 
 * Note: This function does not change the string `dest` originally holds. It
 *       only appends new string(s) to the end of `dest`.
 */
int strwrap(char *dest, char const *src, size_t lw, char const *prefix, 
            char const *postfix);

/**
 * Joins two strings that contain multiple lines by lines.
 * 
 * This function appends each line of `rstr` to the end of each line of `lstr`.
 * If there are more lines in `lstr`, indentations specified by `indent` will
 * be prepended to each of the remaining lines of `rstr`.
 * 
 * Note: This function does not change the string `dest` originally holds. It
 *       only appends new string(s) to the end of `dest`.
 */
int strjoin(char *dest, char const *lstr, char const *rstr, int indent);

#endif