/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifndef __efsd_macros
#define __efsd_macros

#include <stdlib.h>
#include <errno.h>

#define EFSD_META_DIR_NAME  ".e_meta"

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

#define NEW(X)    ((X*) malloc(sizeof(X)))
#define FREE(X)   { if (X) { free(X); X = NULL; } }

#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

#if USE_THREADS
#define READDIR(dir, de, de_ptr)  (readdir_r(dir, &de, &de_ptr))
#define CTIME(time_ptr, buf)      (ctime_r(time_ptr, buf))
#else
#define READDIR(dir, de, de_ptr)  (de_ptr = readdir(dir))
#define CTIME(time_ptr, buf)      (buf = ctime(time_ptr))
#endif

#define	SWAP_LONG(y) \
        ((((y)&0xff)<<24) | (((y)&0xff00)<<8) | (((y)&0xff0000)>>8) | (((y)>>24)&0xff))
#define	SWAP_SHORT(y) \
	((((y)&0xff)<<8) | ((u_short)((y)&0xff00)>>8))

#ifndef ENODATA
#define ENODATA  EINVAL
#endif

#endif
