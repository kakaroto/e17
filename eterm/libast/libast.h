/*
 * Copyright (C) 1997-2001, Michael Jennings
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

#ifndef _LIBAST_H_
#define _LIBAST_H_

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#ifdef WITH_DMALLOC
# include <dmalloc.h>
#elif defined(HAVE_MALLOC_H)
# include <malloc.h>
#endif
#ifdef HAVE_REGEX_H
# include <regex.h>
#endif

#ifdef LIBAST_X11_SUPPORT
# include <X11/Xatom.h>
# include <X11/X.h>
# include <X11/Intrinsic.h>
# ifdef LIBAST_IMLIB2_SUPPORT
#  include <Imlib2.h>
# endif
#endif

/******************************* GENERIC GOOP *********************************/
#ifndef TRUE
# define TRUE    ((unsigned char)(1))
# define FALSE   ((unsigned char)(0))
#endif

#define USE_VAR(x)   (void) x

#ifdef MIN
# undef MIN
#endif
#ifdef MAX
# undef MAX
#endif
#ifdef __GNUC__
# define MIN(a,b)                       __extension__ ({__typeof__(a) aa = (a); __typeof__(b) bb = (b); (aa < bb) ? (aa) : (bb);})
# define MAX(a,b)                       __extension__ ({__typeof__(a) aa = (a); __typeof__(b) bb = (b); (aa > bb) ? (aa) : (bb);})
# define LOWER_BOUND(current, other)    __extension__ ({__typeof__(other) o = (other); ((current) < o) ? ((current) = o) : (current);})
# define AT_LEAST(current, other)       LOWER_BOUND(current, other)
# define MAX_IT(current, other)         LOWER_BOUND(current, other)
# define UPPER_BOUND(current, other)    __extension__ ({__typeof__(other) o = (other); ((current) > o) ? ((current) = o) : (current);})
# define AT_MOST(current, other)        UPPER_BOUND(current, other)
# define MIN_IT(current, other)         UPPER_BOUND(current, other)
# define BOUND(val, min, max)           __extension__ ({__typeof__(min) m1 = (min); __typeof__(max) m2 = (max); ((val) < m1) ? ((val) = m1) : (((val) > m2) ? ((val) = m2) : (val));})
# define CONTAIN(val, min, max)         BOUND(val, min, max)
# define SWAP_IT(one, two, tmp)         do {(tmp) = (one); (one) = (two); (two) = (tmp);} while (0)
#else
# define MIN(a,b)	                (((a) < (b)) ? (a) : (b))
# define MAX(a,b)                       (((a) > (b)) ? (a) : (b))
# define LOWER_BOUND(current, other)    (((current) < (other)) ? ((current) = (other)) : (current))
# define AT_LEAST(current, other)       LOWER_BOUND(current, other)
# define MAX_IT(current, other)         LOWER_BOUND(current, other)
# define UPPER_BOUND(current, other)    (((current) > (other)) ? ((current) = (other)) : (current))
# define AT_MOST(current, other)        UPPER_BOUND(current, other)
# define MIN_IT(current, other)         UPPER_BOUND(current, other)
# define BOUND(val, min, max)           (((val) < (min)) ? ((val) = (min)) : (((val) > (max)) ? ((val) = (max)) : (val)))
# define CONTAIN(val, min, max)         BOUND(val, min, max)
# define SWAP_IT(one, two, tmp)         do {(tmp) = (one); (one) = (two); (two) = (tmp);} while (0)
#endif

/****************************** DEBUGGING GOOP ********************************/
#ifndef LIBAST_DEBUG_FD
# define LIBAST_DEBUG_FD  (stderr)
#endif
#ifndef DEBUG
# define DEBUG 0
#endif

#define DEBUG_LEVEL       (libast_debug_level)
#define DEBUG_FLAGS       (libast_debug_flags)

/* A NOP.  Does nothing. */
#define NOP ((void)0)

/* A macro and an #define to FIXME-ize individual calls or entire code blocks. */
#define FIXME_NOP(x)
#define FIXME_BLOCK 0

/* An "unused block" marker similar to the above. */
#define UNUSED_BLOCK 0

/* The basic debugging output leader. */
#if defined(__FILE__) && defined(__LINE__)
# ifdef __GNUC__
#  define __DEBUG()  fprintf(LIBAST_DEBUG_FD, "[%lu] %12s | %4d: %s(): ", (unsigned long) time(NULL), __FILE__, __LINE__, __FUNCTION__)
# else
#  define __DEBUG()  fprintf(LIBAST_DEBUG_FD, "[%lu] %12s | %4d: ", (unsigned long) time(NULL), __FILE__, __LINE__)
# endif
#else
# define __DEBUG()   NOP
#endif

/* A quick and dirty macro to say, "Hi!  I got here without crashing!" */
#define MOO()  do {__DEBUG(); libast_dprintf("Moo.\n");} while (0)

/* Assertion/abort macros which are quite a bit more useful than assert() and abort(). */
#if DEBUG >= 1
# if defined(__FILE__) && defined(__LINE__)
#  ifdef __GNUC__
#   define ASSERT(x)  do {if (!(x)) {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed in %s() at %s:%d:  %s\n", __FUNCTION__, __FILE__, __LINE__, #x);} \
                                                    else {print_warning("ASSERT failed in %s() at %s:%d:  %s\n", __FUNCTION__, __FILE__, __LINE__, #x);}}} while (0)
#   define ASSERT_RVAL(x, val)  do {if (!(x)) {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed in %s() at %s:%d:  %s\n", __FUNCTION__, __FILE__, __LINE__, #x);} \
                                                              else {print_warning("ASSERT failed in %s() at %s:%d:  %s\n", __FUNCTION__, __FILE__, __LINE__, #x);} \
                                               return (val);}} while (0)
#   define ASSERT_NOTREACHED()  do {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed in %s() at %s:%d:  This code should not be reached.\n", __FUNCTION__, __FILE__, __LINE__);} \
                                                   else {print_warning("ASSERT failed in %s() at %s:%d:  This code should not be reached.\n", __FUNCTION__, __FILE__, __LINE__);} \
                                    } while (0)
#   define ASSERT_NOTREACHED_RVAL(val)  do {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed in %s() at %s:%d:  This code should not be reached.\n", __FUNCTION__, __FILE__, __LINE__);} \
                                                           else {print_warning("ASSERT failed in %s() at %s:%d:  This code should not be reached.\n", __FUNCTION__, __FILE__, __LINE__);} \
                                            return (val);} while (0)
#   define ABORT() fatal_error("Aborting in %s() at %s:%d.\n", __FUNCTION__, __FILE__, __LINE__)
#  else
#   define ASSERT(x)  do {if (!(x)) {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed at %s:%d:  %s\n", __FILE__, __LINE__, #x);} \
                                                    else {print_warning("ASSERT failed at %s:%d:  %s\n", __FILE__, __LINE__, #x);}}} while (0)
#   define ASSERT_RVAL(x, val)  do {if (!(x)) {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed at %s:%d:  %s\n", __FILE__, __LINE__, #x);} \
                                                              else {print_warning("ASSERT failed at %s:%d:  %s\n", __FILE__, __LINE__, #x);} \
                                               return (val);}} while (0)
#   define ASSERT_NOTREACHED()  do {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed at %s:%d:  This code should not be reached.\n", __FILE__, __LINE__);} \
                                                   else {print_warning("ASSERT failed at %s:%d:  This code should not be reached.\n", __FILE__, __LINE__);} \
                                    } while (0)
#   define ASSERT_NOTREACHED_RVAL(val)  do {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed at %s:%d:  This code should not be reached.\n", __FILE__, __LINE__);} \
                                                           else {print_warning("ASSERT failed at %s:%d:  This code should not be reached.\n", __FILE__, __LINE__);} \
                                            return (val);} while (0)
#   define ABORT() fatal_error("Aborting at %s:%d.\n", __FILE__, __LINE__)
#  endif
# else
#  define ASSERT(x)  do {if (!(x)) {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed:  %s\n", #x);} \
                                                   else {print_warning("ASSERT failed:  %s\n", #x);}}} while (0)
#  define ASSERT_RVAL(x, val)  do {if (!(x)) {if (DEBUG_LEVEL>=1) {fatal_error("ASSERT failed:  %s\n", #x);} \
                                                             else {print_warning("ASSERT failed:  %s\n", #x);} return (val);}} while (0)
#  define ASSERT_NOTREACHED()           return
#  define ASSERT_NOTREACHED_RVAL(x)     return (x)
#  define ABORT()                       fatal_error("Aborting.\n")
# endif
# define REQUIRE(x)                     do {if (!(x)) {if (DEBUG_LEVEL>=1) {__DEBUG(); libast_dprintf("REQUIRE failed:  %s\n", #x);} return;}} while (0)
# define REQUIRE_RVAL(x, v)             do {if (!(x)) {if (DEBUG_LEVEL>=1) {__DEBUG(); libast_dprintf("REQUIRE failed:  %s\n", #x);} return (v);}} while (0)
#else
# define ASSERT(x)                      NOP
# define ASSERT_RVAL(x, val)            NOP
# define ASSERT_NOTREACHED()            return
# define ASSERT_NOTREACHED_RVAL(val)    return (val)
# define ABORT()                        fatal_error("Aborting.\n")
# define REQUIRE(x)                     do {if (!(x)) return;} while (0)
# define REQUIRE_RVAL(x, v)             do {if (!(x)) return (v);} while (0)
#endif

#define NONULL(x) ((x) ? (x) : ("<null>"))

/* Macros for printing debugging messages */
#if DEBUG >= 1
# ifndef DPRINTF
#  define DPRINTF(x)           do { __DEBUG(); libast_dprintf x; } while (0)
# endif
# define DPRINTF1(x)           do { if (DEBUG_LEVEL >= 1) {__DEBUG(); libast_dprintf x;} } while (0)
# define DPRINTF2(x)           do { if (DEBUG_LEVEL >= 2) {__DEBUG(); libast_dprintf x;} } while (0)
# define DPRINTF3(x)           do { if (DEBUG_LEVEL >= 3) {__DEBUG(); libast_dprintf x;} } while (0)
# define DPRINTF4(x)           do { if (DEBUG_LEVEL >= 4) {__DEBUG(); libast_dprintf x;} } while (0)
# define DPRINTF5(x)           do { if (DEBUG_LEVEL >= 5) {__DEBUG(); libast_dprintf x;} } while (0)
# define DPRINTF6(x)           do { if (DEBUG_LEVEL >= 6) {__DEBUG(); libast_dprintf x;} } while (0)
# define DPRINTF7(x)           do { if (DEBUG_LEVEL >= 7) {__DEBUG(); libast_dprintf x;} } while (0)
# define DPRINTF8(x)           do { if (DEBUG_LEVEL >= 8) {__DEBUG(); libast_dprintf x;} } while (0)
# define DPRINTF9(x)           do { if (DEBUG_LEVEL >= 9) {__DEBUG(); libast_dprintf x;} } while (0)
#else
# ifndef DPRINTF
#  define DPRINTF(x)           NOP
# endif
# define DPRINTF1(x)           NOP
# define DPRINTF2(x)           NOP
# define DPRINTF3(x)           NOP
# define DPRINTF4(x)           NOP
# define DPRINTF5(x)           NOP
# define DPRINTF6(x)           NOP
# define DPRINTF7(x)           NOP
# define DPRINTF8(x)           NOP
# define DPRINTF9(x)           NOP
#endif

/* Use this for stuff that you only want turned on in dire situations */
#define D_NEVER(x)             NOP

#define DEBUG_CONF             3
#define D_CONF(x)              DPRINTF3(x)
#define DEBUG_MEM              5
#define D_MEM(x)               DPRINTF5(x)
#define DEBUG_STRINGS          9999
#define D_STRINGS(x)           D_NEVER(x)
#define DEBUG_PARSE            9999
#define D_PARSE(x)             D_NEVER(x)



/********************************* MEM GOOP ***********************************/
#if (DEBUG >= DEBUG_MEM)
# define MALLOC(sz)                             libast_malloc(__FILE__, __LINE__, (sz))
# define CALLOC(type,n)                         libast_calloc(__FILE__, __LINE__, (n), (sizeof(type)))
# define REALLOC(mem,sz)                        libast_realloc(#mem, __FILE__, __LINE__, (mem), (sz))
# define FREE(ptr)                              do { libast_free(#ptr, __FILE__, __LINE__, (ptr)); (ptr) = NULL; } while (0)
# define STRDUP(s)                              libast_strdup(#s, __FILE__, __LINE__, (s))
# define MALLOC_DUMP()                          libast_dump_mem_tables()
# define X_CREATE_PIXMAP(d, win, w, h, depth)   libast_x_create_pixmap(__FILE__, __LINE__, (d), (win), (w), (h), (depth))
# define X_FREE_PIXMAP(d, p)                    libast_x_free_pixmap(#p, __FILE__, __LINE__, (d), (p))
# ifdef HAVE_LIBIMLIB2
#  define IMLIB_REGISTER_PIXMAP(p)              libast_imlib_register_pixmap(#p, __FILE__, __LINE__, (p))
#  define IMLIB_FREE_PIXMAP(p)                  libast_imlib_free_pixmap(#p, __FILE__, __LINE__, (p))
# else
#  define IMLIB_REGISTER_PIXMAP(p)              NOP
#  define IMLIB_FREE_PIXMAP(p)                  NOP
# endif
# define PIXMAP_DUMP()                          libast_dump_pixmap_tables()
# define X_CREATE_GC(d, win, f, gcv)            libast_x_create_gc(__FILE__, __LINE__, (d), (win), (f), (gcv))
# define X_FREE_GC(d, gc)                       libast_x_free_gc(#gc, __FILE__, __LINE__, (d), (gc))
# define GC_DUMP()                              libast_dump_gc_tables()
# define MALLOC_MOD 25
# define REALLOC_MOD 25
# define CALLOC_MOD 25
# define FREE_MOD 25
#else
# define MALLOC(sz)                             malloc(sz)
# define CALLOC(type,n)                         calloc((n),(sizeof(type)))
# define REALLOC(mem,sz)                        ((sz) ? ((mem) ? (realloc((mem), (sz))) : (malloc(sz))) : ((mem) ? (free(mem), NULL) : (NULL)))
# define FREE(ptr)                              do { free(ptr); (ptr) = NULL; } while (0)
# define STRDUP(s)                              strdup(s)
# define MALLOC_DUMP()                          NOP
# define X_CREATE_PIXMAP(d, win, w, h, depth)   XCreatePixmap((d), (win), (w), (h), (depth))
# define X_FREE_PIXMAP(d, p)                    XFreePixmap((d), (p))
# ifdef HAVE_LIBIMLIB2
#  define IMLIB_REGISTER_PIXMAP(p)              NOP
#  define IMLIB_FREE_PIXMAP(p)                  imlib_free_pixmap_and_mask(p)
# else
#  define IMLIB_REGISTER_PIXMAP(p)              NOP
#  define IMLIB_FREE_PIXMAP(p)                  NOP
# endif
# define PIXMAP_DUMP()                          NOP
# define X_CREATE_GC(d, win, f, gcv)            XCreateGC((d), (win), (f), (gcv))
# define X_FREE_GC(d, gc)                       XFreeGC((d), (gc))
# define GC_DUMP()                              NOP
#endif

/* Fast memset() macro contributed by vendu */
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
    if (((unsigned long) count) >= 4 * sizeof(long)) { \
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



/******************************* STRINGS GOOP *********************************/

#ifdef __GNUC__
# define SWAP(a, b)  __extension__ ({__typeof__(a) tmp = (a); (a) = (b); (b) = tmp;})
#else
# define SWAP(a, b)  do {void *tmp = ((void *)(a)); (a) = (b); (b) = tmp;} while (0)
#endif

#define CONST_STRLEN(x)            (sizeof(x) - 1)
#define BEG_STRCASECMP(s, constr)  (strncasecmp(s, constr, CONST_STRLEN(constr)))



/******************************** CONF GOOP ***********************************/

#if defined(PATH_MAX) && (PATH_MAX < 255)
#  undef PATH_MAX
#endif
#ifndef PATH_MAX
#  define PATH_MAX 255
#endif

#define CONF_BEGIN_CHAR                 ((char) 1)
#define CONF_BEGIN_STRING               "\001"
#define CONF_END_CHAR                   ((char) 2)
#define CONF_END_STRING                 "\002"

#define BOOL_OPT_ISTRUE(s)  (!strcasecmp((s), true_vals[0]) || !strcasecmp((s), true_vals[1]) \
                             || !strcasecmp((s), true_vals[2]) || !strcasecmp((s), true_vals[3]))
#define BOOL_OPT_ISFALSE(s) (!strcasecmp((s), false_vals[0]) || !strcasecmp((s), false_vals[1]) \
                             || !strcasecmp((s), false_vals[2]) || !strcasecmp((s), false_vals[3]))

/* The context table */
#define ctx_name_to_id(the_id, n, i) do { \
                                       for ((i)=0; (i) <= ctx_idx; (i)++) { \
                                         if (!strcasecmp((n), context[(i)].name)) { \
		                           (the_id) = (i); \
					   break; \
					 } \
			               } \
                                       if ((i) > ctx_idx) { \
                                         print_error("Parsing file %s, line %lu:  No such context \"%s\"\n", file_peek_path(), file_peek_line(), (n)); \
                                         (the_id) = 0; \
                                       } \
                                     } while (0)
#define ctx_id_to_name(id)         (context[(id)].name)
#define ctx_id_to_func(id)         (context[(id)].handler)

/* The context state stack.  This keeps track of the current context and each previous one. */
#define ctx_push(ctx)              conf_register_context_state(ctx)
#define ctx_pop()                  (ctx_state_idx--)
#define ctx_peek()                 (ctx_state[ctx_state_idx])
#define ctx_peek_id()              (ctx_state[ctx_state_idx].ctx_id)
#define ctx_peek_state()           (ctx_state[ctx_state_idx].state)
#define ctx_peek_last_id()         (ctx_state[(ctx_state_idx?ctx_state_idx-1:0)].ctx_id)
#define ctx_peek_last_state()      (ctx_state[(ctx_state_idx?ctx_state_idx-1:0)].state)
#define ctx_poke_state(q)          ((ctx_state[ctx_state_idx].state) = (q))
#define ctx_get_depth()            (ctx_state_idx)
#define ctx_begin(idx)             do { \
                                     char *name; \
                                     name = get_word(idx, buff); \
                                     ctx_name_to_id(id, name, i); \
                                     ctx_push(id); \
                                     state = (*ctx_id_to_func(id))(CONF_BEGIN_STRING, ctx_peek_last_state()); \
                                     ctx_poke_state(state); \
                                     FREE(name); \
                                   } while (0)
#define ctx_end()                  do { \
                                     if (ctx_get_depth()) { \
                                       state = (*ctx_id_to_func(id))(CONF_END_STRING, ctx_peek_state()); \
                                       ctx_poke_state(NULL); \
                                       ctx_pop(); \
                                       id = ctx_peek_id(); \
                                       ctx_poke_state(state); \
                                       file_poke_skip(0); \
                                     } \
                                   } while (0)

/* The file state stack */
#define FILE_SKIP_TO_END           (0x01)
#define FILE_PREPROC               (0x02)
#define file_push(f, p, o, l, fl)  conf_register_fstate(f, p, o, l, fl)
#define file_pop()                 (fstate_idx--)
#define file_peek()                (fstate[fstate_idx])
#define file_peek_fp()             (fstate[fstate_idx].fp)
#define file_peek_path()           (fstate[fstate_idx].path)
#define file_peek_outfile()        (fstate[fstate_idx].outfile)
#define file_peek_line()           (fstate[fstate_idx].line)
#define file_peek_skip()           (fstate[fstate_idx].flags & FILE_SKIP_TO_END)
#define file_peek_preproc()        (fstate[fstate_idx].flags & FILE_PREPROC)

#define file_poke_fp(f)            ((fstate[fstate_idx].fp) = (f))
#define file_poke_path(p)          ((fstate[fstate_idx].path) = (p))
#define file_poke_outfile(o)       ((fstate[fstate_idx].outfile) = (o))
#define file_poke_line(l)          ((fstate[fstate_idx].line) = (l))
#define file_skip_to_end()         ((fstate[fstate_idx].flags) |= (FILE_SKIP_TO_END))
#define file_poke_skip(s)          do {if (s) {fstate[fstate_idx].flags |= FILE_SKIP_TO_END;} else {fstate[fstate_idx].flags &= ~(FILE_SKIP_TO_END);} } while (0)
#define file_poke_preproc(s)       do {if (s) {fstate[fstate_idx].flags |= FILE_PREPROC;} else {fstate[fstate_idx].flags &= ~(FILE_PREPROC);} } while (0)
#define file_poke(f, p, o, l, fl)  do {file_poke_fp(f); file_poke_path(p); file_poke_outfile(o); file_poke_line(l); fstate[fstate_idx].flags = (fl);} while (0)

#define file_inc_line()            (fstate[fstate_idx].line++)

/* Contexts */
typedef void * (*ctx_handler_t)(char *, void *);
typedef struct context_struct {
  char *name;
  ctx_handler_t handler;
} ctx_t;
typedef struct ctx_state_struct {
  unsigned char ctx_id;
  void *state;
} ctx_state_t;

/* Parser states */
typedef struct file_state_struct {
  FILE *fp;
  char *path, *outfile;
  unsigned long line;
  unsigned char flags;
} fstate_t;

/* Built-in functions */
typedef char * (*conf_func_ptr_t) (char *);
typedef struct conf_func_struct {
  char *name;
  conf_func_ptr_t ptr;
} conf_func_t;

typedef struct conf_var_struct {
  char *var, *value;
  struct conf_var_struct *next;
} conf_var_t;

extern fstate_t *fstate;
extern unsigned char fstate_idx;



/******************************** PROTOTYPES **********************************/

/* msgs.c */
extern void libast_set_program_name(const char *);
extern void libast_set_program_version(const char *);
extern int libast_dprintf(const char *, ...);
extern void print_error(const char *fmt, ...);
extern void print_warning(const char *fmt, ...);
extern void fatal_error(const char *fmt, ...);

/* debug.c */
extern unsigned int DEBUG_LEVEL;

/* mem.c */
extern void memrec_init(void);
extern void *libast_malloc(const char *, unsigned long, size_t);
extern void *libast_realloc(const char *, const char *, unsigned long, void *, size_t);
extern void *libast_calloc(const char *, unsigned long, size_t, size_t);
extern void libast_free(const char *, const char *, unsigned long, void *);
extern char *libast_strdup(const char *, const char *, unsigned long, const char *);
extern void libast_dump_mem_tables(void);
#ifdef LIBAST_X11_SUPPORT
extern Pixmap libast_x_create_pixmap(const char *, unsigned long, Display *, Drawable, unsigned int, unsigned int, unsigned int);
extern void libast_x_free_pixmap(const char *, const char *, unsigned long, Display *, Pixmap);
# ifdef LIBAST_IMLIB2_SUPPORT
extern void libast_imlib_register_pixmap(const char *var, const char *filename, unsigned long line, Pixmap p);
extern void libast_imlib_free_pixmap(const char *var, const char *filename, unsigned long line, Pixmap p);
# endif
extern void libast_dump_pixmap_tables(void);
extern GC libast_x_create_gc(const char *, unsigned long, Display *, Drawable, unsigned long, XGCValues *);
extern void libast_x_free_gc(const char *, const char *, unsigned long, Display *, GC);
extern void libast_dump_gc_tables(void);
#endif
extern void free_array(void **, size_t);

/* file.c */
extern int libast_temp_file(char *, size_t);

/* strings.c */
extern char *left_str(const char *, unsigned long);
extern char *mid_str(const char *, unsigned long, unsigned long);
extern char *right_str(const char *, unsigned long);
#if defined(HAVE_REGEX_H)
extern unsigned char regexp_match(const char *, const char *);
extern unsigned char regexp_match_r(const char *str, const char *pattern, regex_t **rexp);
#endif
extern char **split(const char *, const char *);
extern char **split_regexp(const char *, const char *);
extern char *join(const char *, char **);
extern char *get_word(unsigned long, const char *);
extern char *get_pword(unsigned long, const char *);
extern unsigned long num_words(const char *);
extern char *chomp(char *);
extern char *strip_whitespace(char *);
extern char *downcase_str(char *);
extern char *upcase_str(char *);
#ifndef HAVE_STRCASESTR
extern char *strcasestr(const char *, const char *);
#endif
#ifndef HAVE_STRCASECHR
extern char *strcasechr(const char *, const char);
#endif
#ifndef HAVE_STRCASEPBRK
extern char *strcasepbrk(const char *, const char *);
#endif
#ifndef HAVE_STRREV
extern char *strrev(char *);
#endif
#if !(HAVE_STRSEP)
extern char *strsep(char **, char *);
#endif
extern char *safe_str(char *, unsigned short);
extern char *garbage_collect(char *, size_t);
extern char *file_garbage_collect(char *, size_t);
extern char *condense_whitespace(char *);
extern void hex_dump(void *, size_t);
#ifndef HAVE_MEMMEM
extern void *memmem(const void *, size_t, const void *, size_t);
#endif
#ifndef HAVE_USLEEP
extern void usleep(unsigned long);
#endif
#ifndef HAVE_SNPRINTF
extern int vsnprintf(char *str, size_t count, const char *fmt, va_list args);
extern int snprintf(char *str, size_t count, const char *fmt, ...);
#endif

/* conf.c */
extern void conf_init_subsystem(void);
extern unsigned char conf_register_context(char *name, ctx_handler_t handler);
extern unsigned char conf_register_fstate(FILE *fp, char *path, char *outfile, unsigned long line, unsigned char flags);
extern unsigned char conf_register_builtin(char *name, conf_func_ptr_t ptr);
extern unsigned char conf_register_context_state(unsigned char ctx_id);
extern void conf_free_subsystem(void);
extern char *shell_expand(char *);
extern char *conf_find_file(const char *file, const char *dir, const char *pathlist);
extern FILE *open_config_file(char *name);
extern void conf_parse_line(FILE *fp, char *buff);
extern char *conf_parse(char *conf_name, const char *dir, const char *path);

#endif /* _LIBAST_H_ */
