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
 *  File: regex_func.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */
#include "regex_func.h"

/*
 * Match string against the extended regular expression 
 * in pattern, treating errors as no match. 
 * 
 * return 0 for match, 1 for no match 
 */
/* --> TODO: error handling regerror()! */

int regex_match (const char *string, char *pattern)
{
  int status;
  regex_t re;

  status = regcomp (&re, pattern, REG_EXTENDED | REG_NOSUB);

  if (status != 0)
  {
    return status;
  }

  status = regex_match2 (&re, string, pattern);
  regfree (&re);

  return status;
}

int regex_match2 (regex_t *re, const char *string, char *pattern)
{
  int status;

  status = regexec (re, string, (size_t) 0, NULL, 0);

  return status;
}

int regex_sub (const char *string, char *pattern, struct substrings **ss_ptr,
               int maxsub, int *numsub)
{
  regex_t re;
  int status;
  char err_buf[1024];

  status = regcomp (&re, pattern, REG_EXTENDED);

  if (status != 0)
  {
    regerror (status, &re, err_buf, (size_t) 1024);
    printf ("regex compile: %s\n", err_buf);

    return status;
  }

  status = regex_sub2 (&re, string, pattern, ss_ptr, maxsub, numsub);

  if (status != 0)
  {
    regerror (status, &re, err_buf, (size_t) 1024);
    printf ("regex exec: %s\n", err_buf);
  }

  regfree (&re);

  return status;
}

int regex_sub2 (regex_t *re, const char *string, char *pattern,
                struct substrings **ss_ptr, int maxsub, int *numsub)
{
  int status, i;
  regmatch_t *submatch;
  struct substrings *ss = NULL;
  int string_len = strlen (string);
  
  maxsub = maxsub + 1;

  submatch = malloc (maxsub * sizeof (*submatch));

  status = regexec (re, string, (size_t) maxsub, submatch, 0);

  if (status == REG_NOMATCH)
  {
    free (submatch);
    return status;
  }

  ss = malloc (maxsub * sizeof (*ss));

  *ss_ptr = ss;

  for (i = 0; i < maxsub ; i++)
  {
    if (submatch[i].rm_so == submatch[i].rm_eo && submatch[i].rm_eo == -1)
    {
      break;
    }

    ss[i].match = malloc (submatch[i].rm_eo - submatch[i].rm_so + 1);
    strncpy (ss[i].match, string + submatch[i].rm_so,
             submatch[i].rm_eo - submatch[i].rm_so);
    if (ss[i].match[submatch[i].rm_eo - submatch[i].rm_so] != '\0')
    {
      ss[i].match[submatch[i].rm_eo - submatch[i].rm_so] = '\0';
    }


    ss[i].start = malloc (submatch[i].rm_so + 1);
    strncpy (ss[i].start, string, submatch[i].rm_so);
    if (ss[i].start[submatch[i].rm_so] != '\0')
    {
      ss[i].start[submatch[i].rm_so] = '\0';
    }

    ss[i].end = malloc (string_len - submatch[i].rm_eo + 1);
    strncpy (ss[i].end, string + submatch[i].rm_eo,
             string_len - submatch[i].rm_eo);
    if (ss[i].end[string_len - submatch[i].rm_eo] != '\0')
    {
      ss[i].end[string_len - submatch[i].rm_eo] = '\0';
    }

    *numsub = i + 1;
  }

  free (submatch);

  return status;
}
