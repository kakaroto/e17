/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004 Kim Woelders
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

#if 0				/* Not used yet */

#include <iconv.h>
static iconv_t      iconv_cd = NULL;

char               *
Eiconv(const char *txt, size_t len)
{
   char                buf[4096];
   char               *pi, *po;
   size_t              err, ni, no;

   pi = (char *)txt;
   po = buf;
   ni = (len > 0) ? len : strlen(txt);
   no = sizeof(buf);
   err = iconv(iconv_cd, &pi, &ni, &po, &no);

   po = Estrndup(buf, sizeof(buf) - no);

   return po;
}

#endif

void
LangInit(void)
{
   const char         *str;

   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   if (!XSupportsLocale())
      setlocale(LC_ALL, "C");
   XSetLocaleModifiers("");

   /* I dont want any internationalisation of my numeric input & output */
   setlocale(LC_NUMERIC, "C");

   str = getenv("E_CHARSET");
#if 0
   if (str == NULL)
      str = "UTF-8";
#endif
   if (str)
     {
	bind_textdomain_codeset(PACKAGE, str);
#if 0
	iconv_cd = iconv_open(str, "ISO_8859-1");
#endif
     }
}

#if 0				/* Not used yet */

static void
LangExit(void)
{
   if (iconv_cd)
      iconv_close(iconv_cd);
}

#endif
