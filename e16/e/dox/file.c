/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2008 Kim Woelders
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
#include "dox.h"
#ifdef USE_WORD_MB
# include <wctype.h>
#endif

static int
exists(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (stat(s, &st) < 0)
      return (0);
   return (1);
}

void
freestrlist(char **l, int num)
{
   if (!l)
      return;
   while (num--)
      Efree(l[num]);
   Efree(l);
}

void
word(char *s, int num, char *wd)
{
   int                 cnt, i;
   char               *start, *finish, *ss, *w;

   if (!s)
      return;
   if (!wd)
      return;
   if (num <= 0)
     {
	*wd = 0;
	return;
     }
   cnt = 0;
   i = 0;
   start = NULL;
   finish = NULL;
   ss = NULL;
   w = wd;

   while (s[i])
     {
	if ((cnt == num) && ((s[i] == ' ') || (s[i] == '\t')))
	  {
	     finish = &s[i];
	     break;
	  }
	if ((s[i] != ' ') && (s[i] != '\t'))
	  {
	     if (i == 0)
	       {
		  cnt++;
		  if (cnt == num)
		     start = &s[i];
	       }
	     else if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
	       {
		  cnt++;
		  if (cnt == num)
		     start = &s[i];
	       }
	  }
	i++;
     }
   if (cnt == num)
     {
	if ((start) && (finish))
	  {
	     for (ss = start; ss < finish; ss++)
		*wd++ = *ss;
	  }
	else if (start)
	  {
	     for (ss = start; *ss != 0; ss++)
		*wd++ = *ss;
	  }
	*wd = 0;
     }
}

#ifdef USE_WORD_MB
void
word_mb(char *s, int num, char *wd, int *spaceflag)
{
   int                 cnt, i;
   char               *start, *finish, *ss, *w;

   int                 wcflg, mbflg;
   struct char_class {
      const char         *name;
      wctype_t            wt;
   }                  *cc, char_class_tbl[] =
   {
#ifdef linux
      /* Will be supported on glibc 2.1.3 or later */
      {
      "jspace", 0},
      {
      "jhira", 0},
      {
      "jkata", 0},
      {
      "jkanji", 0},
      {
      "jdigit", 0},		/* Japanese */
      {
      "hangul", 0},
      {
      "hanja", 0},		/* Korean    */
	 /* {"?????"}, {"?????"}, *//* Chinese   */
#endif
#ifdef sgi
	 /* SGI IRIX (Japanese, Chinese, Korean, etc..) */
      {
      "special", 0},
      {
      "phonogram", 0},
      {
      "ideogram", 0},
#endif
#ifdef sun
	 /* {"?????"}, {"?????"}, */
#endif
#ifdef hpux
	 /* {"?????"}, {"?????"}, */
#endif
      {
      NULL, 0}
   };

   if (!s)
      return;
   if (!wd)
      return;
   if (num <= 0)
     {
	*wd = 0;
	return;
     }

   /*  Check multibyte character class is available or not */
   wcflg = 0;
   for (cc = char_class_tbl; cc->name != NULL; cc++)
     {
	cc->wt = wctype(cc->name);
	if (cc->wt != (wctype_t) 0)
	   wcflg = 1;
     }

   cnt = 0;
   i = 0;
   start = NULL;
   finish = NULL;
   ss = NULL;
   w = wd;
   *spaceflag = 0;

   while (s[i])
     {
	int                 len, oldflg = 1;

	len = mblen(s + i, MB_CUR_MAX);
	if (len < 0)
	  {
	     i++;
	     continue;
	  }

	/*  Check multibyte character class */
	if (wcflg)
	  {
	     wchar_t             wc;

	     mbflg = 1;
	     if ((mbtowc(&wc, s + i, strlen(s + i))) != -1)
	       {
		  for (cc = char_class_tbl; cc->name != NULL; cc++)
		    {
		       if (cc->wt == (wctype_t) 0)
			  continue;

		       if (iswctype(wc, cc->wt) != 0)
			 {
			    mbflg = 2;
			    break;
			 }
		    }
	       }
	  }
	else
	   mbflg = len;

	if ((cnt == num) && ((s[i] == ' ') || (s[i] == '\t') ||
			     (oldflg != mbflg) || (mbflg > 1)))
	  {
	     finish = &s[i];
	     break;
	  }

	if ((s[i] != ' ') && (s[i] != '\t'))
	  {
	     if ((i == 0) ||
		 (s[i - 1] == ' ') || (s[i - 1] == '\t') ||
		 ((oldflg > 1) && (mbflg > 1)) || (oldflg != mbflg))
	       {
		  cnt++;
		  if (cnt == num)
		    {
		       start = &s[i];
		       if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
			  *spaceflag = 1;
		    }
	       }
	  }
	i += len;
	oldflg = mbflg;
     }
   if (cnt == num)
     {
	if ((start) && (finish))
	  {
	     for (ss = start; ss < finish; ss++)
		*wd++ = *ss;
	  }
	else if (start)
	  {
	     for (ss = start; *ss != 0; ss++)
		*wd++ = *ss;
	  }
	*wd = 0;
     }
}
#endif

int
findLocalizedFile(char *fname)
{
   char               *tmp, *lang, *p[3];
   int                 i;

   if (!(lang = setlocale(LC_MESSAGES, NULL)))
      return 0;

   tmp = strdup(fname);
   lang = strdup(lang);		/* lang may be in static space, thus it must
				 * be duplicated before we change it below */
   p[0] = lang + strlen(lang);
   p[1] = strchr(lang, '.');
   p[2] = strchr(lang, '_');

   for (i = 0; i < 3; i++)
     {
	if (p[i] == NULL)
	   continue;

	*p[i] = '\0';
	sprintf(fname, "%s.%s", tmp, lang);
	if (exists(fname))
	  {
	     Efree(tmp);
	     Efree(lang);
	     return 1;
	  }
     }
   strcpy(fname, tmp);
   Efree(tmp);
   Efree(lang);

   return 0;
}
