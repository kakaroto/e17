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
#ifndef _UTIL_H_
#define _UTIL_H_

#include "config.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#define __PRINTF__ __attribute__((__format__(__printf__, 1, 2)))
#else
#define __UNUSED__
#define __PRINTF__
#endif

#if HAVE_STRDUP
#define USE_LIBC_STRDUP  1	/* Use libc strdup if present */
#endif
#if HAVE_STRNDUP
#define USE_LIBC_STRNDUP 1	/* Use libc strndup if present */
#endif

/* memory.c */
#define Ecalloc     calloc
#define Emalloc     malloc
#define Efree       free
#define Erealloc    realloc

#define _EFREE(p)    do { if (p) { Efree(p); p = NULL; } } while (0)
#define _EFDUP(p, s) do { if (p) Efree(p); p = Estrdup(s); } while (0)

char               *Estrtrim(char *s);

#if USE_LIBC_STRDUP
#define Estrdup(s) ((s) ? strdup(s) : NULL)
#else
char               *Estrdup(const char *s);
#endif

#if USE_LIBC_STRNDUP
#define Estrndup(s,n) ((s) ? strndup(s,n) : NULL)
#else
char               *Estrndup(const char *s, int n);
#endif

char               *Estrdupcat2(char *ss, const char *s1, const char *s2);

char              **StrlistDup(char **lst, int num);
void                StrlistFree(char **lst, int num);
char               *StrlistJoin(char **lst, int num);
char               *StrlistEncodeEscaped(char *buf, int len, char **lst,
					 int num);
char              **StrlistDecodeEscaped(const char *str, int *pnum);
char              **StrlistFromString(const char *str, int delim, int *num);

void                Esetenv(const char *name, const char *value);

/* misc.c */
void __PRINTF__     Eprintf(const char *fmt, ...);

#ifdef HAVE_SNPRINTF
#define Evsnprintf vsnprintf
#define Esnprintf snprintf
#else /* HAVE_SNPRINTF */
int                 Evsnprintf(char *str, size_t count, const char *fmt,
			       va_list args);

#ifdef HAVE_STDARG_H
int                 Esnprintf(char *str, size_t count, const char *fmt, ...);

#else
int                 Esnprintf(va_alist);
#endif
#endif /* HAVE_SNPRINTF */

#endif /* _UTIL_H_ */
