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
#ifndef __efsd_debug_h
#define __efsd_debug_h

#include <unistd.h>
#include <stdio.h>
#include <efsd_globals.h>

void efsd_debug_print_timestamp(void);

#ifdef DEBUG
#define D(msg) \
{ \
  if (opt_debug) \
    { \
       printf("efsd [%i]: ", getpid()); \
       efsd_debug_print_timestamp(); \
       printf msg; \
       fflush(stdout); \
    } \
}
#else
#define D(msg)
#endif

#ifdef DEBUG_NEST

extern int efsd_debug_nest_level;
void efsd_debug_whitespace(int num);

#define D_ENTER \
{ \
  if (opt_nesting) \
    { \
       efsd_debug_nest_level++; \
       printf("ENTER  "); \
       efsd_debug_print_timestamp(); \
       efsd_debug_whitespace(efsd_debug_nest_level); \
       printf("%s, %u %s()\n", __FILE__, __LINE__, __FUNCTION__); \
       fflush(stdout); \
    } \
}
#define D_RETURN \
{ \
  if (opt_nesting) \
    { \
       printf("RETURN "); \
       efsd_debug_print_timestamp(); \
       efsd_debug_whitespace(efsd_debug_nest_level); \
       printf("%s, %u %s()\n", __FILE__, __LINE__, __FUNCTION__); \
       fflush(stdout); \
       efsd_debug_nest_level--; \
    } \
  return; \
}
#define D_RETURN_(x) \
{ \
  if (opt_nesting) \
    { \
      printf("RETURN "); \
      efsd_debug_print_timestamp(); \
      efsd_debug_whitespace(efsd_debug_nest_level); \
      printf("%s, %u %s()\n", __FILE__, __LINE__, __FUNCTION__); \
      fflush(stdout); \
      efsd_debug_nest_level--; \
    } \
  return x; \
}
#else
#define D_ENTER
#define D_RETURN       return
#define D_RETURN_(x)   return (x)
#endif

#endif 

