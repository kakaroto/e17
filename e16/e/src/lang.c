/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "lang.h"

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#if HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#ifndef ENABLE_NLS
#define bindtextdomain(pkg,locale)
#define textdomain(pkg)
#define bind_textdomain_codeset(pkg,enc)
#endif

#if HAVE_ICONV

#include <iconv.h>
static iconv_t      iconv_cd_int2utf8 = NULL;
static iconv_t      iconv_cd_utf82int = NULL;
static iconv_t      iconv_cd_int2loc = NULL;
static iconv_t      iconv_cd_loc2int = NULL;

static char        *
Eiconv(iconv_t icd, const char *txt, size_t len)
{
   char                buf[4096];
   char               *pi, *po;
   size_t              err, ni, no;

   pi = (char *)txt;
   po = buf;
   ni = (len > 0) ? len : strlen(txt);
   if (!icd)
     {
	Eprintf("*** WARNING - Missing conversion\n");
	return Estrndup(txt, ni);
     }
   no = sizeof(buf);
   err = iconv(icd, &pi, &ni, &po, &no);

   po = Estrndup(buf, sizeof(buf) - no);

   return po;
}

#endif

/* Convert locale to internal format (alloc always) */
char               *
EstrLoc2Int(const char *str, int len)
{
   if (str == NULL)
      return NULL;

#if HAVE_ICONV
   if (iconv_cd_loc2int)
      return Eiconv(iconv_cd_loc2int, str, len);
#endif

   if (len <= 0)
      len = strlen(str);
   return Estrndup(str, len);
}

/* Convert UTF-8 to internal format (alloc always) */
char               *
EstrUtf82Int(const char *str, int len)
{
   if (str == NULL)
      return NULL;

#if HAVE_ICONV
   if (iconv_cd_utf82int)
      return Eiconv(iconv_cd_utf82int, str, len);
#endif

   if (len <= 0)
      len = strlen(str);
   return Estrndup(str, len);
}

/* Convert internal to required (alloc only if necessary) */
const char         *
EstrInt2Enc(const char *str, int want_utf8)
{
#if HAVE_ICONV
   if (Mode.locale.utf8_int == want_utf8)
      return str;

   if (str == NULL)
      return NULL;

   if (want_utf8)
      return Eiconv(iconv_cd_int2utf8, str, strlen(str));

   return Eiconv(iconv_cd_int2loc, str, strlen(str));
#else
   return str;
#endif
}

/* Free string returned by EstrInt2Enc() */
void
EstrInt2EncFree(const char *str, int want_utf8)
{
#if HAVE_ICONV
   if (Mode.locale.utf8_int == want_utf8)
      return;

   if (str)
      Efree((char *)str);
#endif
}

void
LangInit(void)
{
   const char         *enc_loc, *enc_int;

   /* Set up things according to env vars */
   setlocale(LC_ALL, "");

   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   if (!XSupportsLocale())
      setlocale(LC_ALL, "C");
   XSetLocaleModifiers("");

   /* I dont want any internationalisation of my numeric input & output */
   setlocale(LC_NUMERIC, "C");

   /* Get the environment character encoding */
#if HAVE_LANGINFO_CODESET
   enc_loc = nl_langinfo(CODESET);
#else
   enc_loc = "ISO-8859-1";
#endif

   /* Debug - possibility to set desired internal representation */
   enc_int = getenv("E_CHARSET");
   if (enc_int)
      bind_textdomain_codeset(PACKAGE, enc_int);
   else
      enc_int = enc_loc;

   Mode.locale.lang = setlocale(LC_MESSAGES, NULL);
   if (EventDebug(EDBUG_TYPE_VERBOSE))
     {
	Eprintf("Locale: %s\n", setlocale(LC_ALL, NULL));
	Eprintf("Character encoding: locale=%s internal=%s\n", enc_loc,
		enc_int);
     }

   if (!strcasecmp(enc_loc, "utf8") || !strcasecmp(enc_loc, "utf-8"))
      Mode.locale.utf8_loc = 1;
   if (!strcasecmp(enc_int, "utf8") || !strcasecmp(enc_int, "utf-8"))
      Mode.locale.utf8_int = 1;

#if HAVE_ICONV
   if (Mode.locale.utf8_int)
     {
	iconv_cd_loc2int = iconv_open("UTF-8", enc_loc);
	iconv_cd_int2loc = iconv_open(enc_loc, "UTF-8");
	iconv_cd_utf82int = iconv_cd_int2utf8 = NULL;
     }
   else
     {
	iconv_cd_loc2int = iconv_cd_int2loc = NULL;
	iconv_cd_utf82int = iconv_open(enc_loc, "UTF-8");
	iconv_cd_int2utf8 = iconv_open("UTF-8", enc_loc);
     }
#endif
}

#if 0				/* Not used yet */

static void
LangExit(void)
{
   if (iconv_cd)
      iconv_close(iconv_cd);
}

#endif
