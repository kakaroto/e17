/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

static int
isafter(int p, const char *s1, const char *s2)
{
   int                 i, j;
   int                 len, len2;
   int                 match;

   EDBUG(8, "isafter");
   len = strlen(s1);
   len2 = strlen(s2);

   match = 0;
   for (i = p; i < len; i++)
     {
	if (s1[i] == s2[0])
	  {
	     match = 1;
	     for (j = 0; j < len2; j++)
	       {
		  if ((i + j) >= len)
		     EDBUG_RETURN(-1);
		  if (s1[i + j] != s2[j])
		     match = 0;
	       }
	  }
	if (match)
	   EDBUG_RETURN(i + len2);
     }
   EDBUG_RETURN(-1);
}

int
matchregexp(const char *rx, const char *s)
{
   int                 i, l, m;
   int                 len, lenr;
   int                 match;
   char                rx2[1024];

   EDBUG(6, "matchregexp");
   if (!s)
      EDBUG_RETURN(0);
   if (!rx)
      EDBUG_RETURN(0);

   len = strlen(s);
   l = 0;
   lenr = 0;
   match = 1;
   if ((strcmp(rx, "*") || rx[0] == 0) && s[0] == 0)
      EDBUG_RETURN(0);

   if (rx[0] != '*')
     {
	m = 0;
	while ((rx[l] != '*') && (rx[l]) && (m < 1023))
	   rx2[m++] = rx[l++];
	rx2[m] = 0;
	lenr = strlen(rx2);
	if (lenr > len)
	   EDBUG_RETURN(0);
	for (i = 0; i < lenr; i++)
	  {
	     if (s[i] != rx[i])
		EDBUG_RETURN(0);
	  }
     }
   if ((!rx[l]) && (s[lenr]))
      EDBUG_RETURN(0);
   for (i = lenr; i < len; i++)
     {
	if (rx[l])
	   l++;
	if (rx[l])
	  {
	     m = 0;
	     while ((rx[l] != '*') && (rx[l]) && (m < 1023))
		rx2[m++] = rx[l++];
	     rx2[m] = 0;
	     i = isafter(i, s, rx2);
	     if (i < 0)
		EDBUG_RETURN(0);
	  }
	else
	   EDBUG_RETURN(match);
     }
   EDBUG_RETURN(match);
}
