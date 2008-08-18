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
#ifndef __evfs_debug_h
#define __evfs_debug_h

#include <unistd.h>
#include <stdio.h>

#ifdef DEBUG

void evfs_debug_output_start(void);
void evfs_debug_output_end(void);

#define D(fmt, args...) \
{ \
  if (opt_debug) \
    { \
      evfs_debug_output_start(); \
      printf(fmt, ## args); \
      evfs_debug_output_end(); \
    } \
}
#else
#define D(msg, args...)
#endif

#ifdef DEBUG_NEST

void evfs_debug_enter(const char *file, const char *func);
void evfs_debug_return(const char *file, const char *func);

#define D_ENTER \
{ \
  if (opt_nesting) \
    { evfs_debug_enter(__FILE__, __FUNCTION__); } \
}

#define D_RETURN \
{ \
  if (opt_nesting) \
    { evfs_debug_return(__FILE__, __FUNCTION__); } \
\
  return; \
}

#define D_RETURN_(x) \
{ \
  if (opt_nesting) \
    { evfs_debug_return(__FILE__, __FUNCTION__); } \
\
  return x; \
}
#else
#define D_ENTER
#define D_RETURN       return
#define D_RETURN_(x)   return (x)
#endif

#endif
