/* Copyright (C) 2004 Andreas Volz and various contributors
 * 
 * Permission  is  hereby granted, free of charge, to any person ob-
 * taining a copy of  this  software  and  associated  documentation
 * files  (the "Software"), to deal in the Software without restric-
 * tion, including without limitation the rights to use, copy, modi-
 * fy, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is  fur-
 * nished to do so, subject to the following conditions:
 * 
 * The  above  copyright  notice and this permission notice shall be
 * included in all copies of the  Software,  its  documentation  and
 * marketing & publicity materials, and acknowledgment shall be giv-
 * en in the documentation, materials  and  software  packages  that
 * this Software was used.
 * 
 * THE  SOFTWARE  IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO  THE  WARRANTIES
 * OF  MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONIN-
 * FRINGEMENT. IN NO EVENT SHALL  THE  AUTHORS  BE  LIABLE  FOR  ANY
 * CLAIM,  DAMAGES  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CON-
 * TRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR  IN  CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  File: regex_func.h
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#ifndef _REGEX_FUNC_H
#define _REGEX_FUNC_H

#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

struct substrings
{
  char *match;
  char *start;
  char *end;
};

int regex_match (const char *string, char *pattern);
int regex_match2 (regex_t *re, const char *string, char *pattern);
int regex_sub (const char *string, char *pattern, struct substrings **ss_ptr,
               int maxsub, int *numsub);
int regex_sub2 (regex_t *re, const char *string, char *pattern,
                struct substrings **ss_ptr, int maxsub, int *numsub);



#endif /* _REGEX_FUNC_H */
