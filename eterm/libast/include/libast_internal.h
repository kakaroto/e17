/*
 * Copyright (C) 1997-2002, Michael Jennings
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

#ifndef _LIBAST_INTERNAL_H_
#define _LIBAST_INTERNAL_H_

/* This GNU goop has to go before the system headers */
#ifdef __GNUC__
# ifndef __USE_GNU
#  define __USE_GNU
# endif
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# ifndef _BSD_SOURCE
#  define _BSD_SOURCE
# endif
# ifndef inline
#  define inline __inline__
# endif
#endif

#include "config.h"
#include "libast.h"

#ifdef HAVE_REGEX_H
# include <regex.h>
#endif
#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

/******************************** MSGS GOOP ***********************************/
extern char *libast_program_name, *libast_program_version;



/********************************* MEM GOOP ***********************************/
#define LIBAST_FNAME_LEN  20

typedef struct ptr_struct {
  void *ptr;
  size_t size;
  char file[LIBAST_FNAME_LEN + 1];
  unsigned long line;
} ptr_t;
typedef struct memrec_struct {
  unsigned long cnt;
  ptr_t *ptrs;
} memrec_t;


#endif /* _LIBAST_INTERNAL_H_ */
