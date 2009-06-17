/* The Not Game
 *
 * Original concept and Java implementation by Rob Coffey.  Concept
 * and name used with permission.
 *
 * The Not Game for Gtk+, Copyright 1999, Michael Jennings
 *
 * This program is free software and is distributed under the terms of
 * the Artistic License.  Please see the file "Artistic" supplied with
 * this program for license terms.
 */

#ifndef _DEBUG_H
# define _DEBUG_H

#include <stdlib.h>

/* Assert macros stolen from my work on Ebar.  If these macros break with your cpp, let me know -- mej@eterm.org */
# define NOP ((void)0)

#if defined(__FILE__) && defined(__LINE__)
# ifdef __FUNCTION__
#  define __DEBUG()  printf("[debug] %12s | %4d | %30s: ", __FILE__, __LINE__, __FUNCTION__)
# else
#  define __DEBUG()  printf("[debug] %12s | %4d: ", __FILE__, __LINE__)
# endif
#endif

#if defined(__FILE__) && defined(__LINE__)
# define ASSERT(x)  do {if (!(x)) {print_error("ASSERT failed at %s:%d:  %s", __FILE__, __LINE__, #x);}} while (0)
# define ASSERT_RVAL(x, val)  do {if (!(x)) {print_error("ASSERT failed at %s:%d:  %s", __FILE__, __LINE__, #x); return (val);}} while (0)
# define ABORT() clean_exit("Aborting at %s:%d.", __FILE__, __LINE__)
#else
# define ASSERT(x)  do {if (!(x)) {print_error("ASSERT failed:  %s", #x);}} while (0)
# define ASSERT_RVAL(x, val)  do {if (!(x)) {print_error("ASSERT failed:  %s", #x); return (val);}} while (0)
# define ABORT() clean_exit("Aborting.")
#endif

#ifndef __DEBUG
# define __DEBUG()		NOP
#endif

#define REQUIRE(x) do {if (!(x)) {__DEBUG(); printf("REQUIRE failed:  %s\n", #x); return;}} while (0)
#define REQUIRE_RVAL(x, v) do {if (!(x)) {__DEBUG(); printf("REQUIRE failed:  %s\n", #x); return (v);}} while (0)

#define NONULL(x)  ((x) ? (x) : (""))

#if (DEBUG > 0)
#  define D(x) do {__DEBUG(); printf x;} while (0)
#else
#  define D(x) NOP
#endif

#if (SIZEOF_LONG == 8)
# define MEMSET_LONG() l |= l<<32
#else
# define MEMSET_LONG() ((void)0)
#endif

#define MEMSET(s, c, count) do { \
    char *end = (char *)(s) + (count); \
    long l; \
    long *l_dest = (long *)(s); \
    char *c_dest; \
 \
    /* areas of less than 4 * sizeof(long) are set in 1-byte chunks. */ \
    if ((count) >= 4 * sizeof(long)) { \
        /* fill l with c. */ \
        l = (c) | (c)<<8; \
        l |= l<<16; \
        MEMSET_LONG(); \
 \
        /* fill in 1-byte chunks until boundary of long is reached. */ \
        if ((unsigned long)l_dest & (unsigned long)(sizeof(long) -1)) { \
            c_dest = (char *)l_dest; \
            while ((unsigned long)c_dest & (unsigned long)(sizeof(long) -1)) { \
                *(c_dest++) = (c); \
            } \
            l_dest = (long *)c_dest; \
        } \
 \
        /* fill in long-size chunks as long as possible. */ \
        while (((unsigned long) (end - (char *)l_dest)) >= sizeof(long)) { \
            *(l_dest++) = l; \
        } \
    } \
 \
    /* fill the tail in 1-byte chunks. */ \
    if ((char *)l_dest < end) { \
        c_dest = (char *)l_dest; \
        *(c_dest++) = (c); \
        while (c_dest < end) { \
            *(c_dest++) = (c); \
        } \
    } \
  } while (0)

#endif /* _DEBUG_H */
