/*
 * Copyright (C) 1997-2003, Michael Jennings
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

/**
 * @file libast_internal.h
 * LibAST header file for internal-use-only stuff.
 *
 * This file contains all macros, structure definitions, etc. which
 * are restricted to internal LibAST use only.
 *
 * @author Michael Jennings <mej@eterm.org>
 * @version $Revision$
 * @date $Date$
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
#endif

#include "config.h"
#include "libast.h"

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



/******************************** CONF GOOP ***********************************/
/* The context table */
#define ctx_name_to_id(the_id, n, i) do { \
                                       for ((i)=0; (i) <= ctx_idx; (i)++) { \
                                         if (!strcasecmp((n), context[(i)].name)) { \
                                           (the_id) = (i); \
                                           break; \
                                         } \
                                       } \
                                       if ((i) > ctx_idx) { \
                                         print_error("Parsing file %s, line %lu:  No such context \"%s\"\n", \
                                                     file_peek_path(), file_peek_line(), (n)); \
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

typedef struct context_struct {
  char *name;
  ctx_handler_t handler;
} ctx_t;

typedef struct ctx_state_struct {
  unsigned char ctx_id;
  void *state;
} ctx_state_t;

/* Built-in functions */
typedef struct conf_func_struct {
  char *name;
  conf_func_ptr_t ptr;
} conf_func_t;

typedef struct conf_var_struct {
  char *var, *value;
  struct conf_var_struct *next;
} conf_var_t;



/******************************* OPTIONS GOOP **********************************/

#define CHECK_BAD()  do { \
                       SPIFOPT_BADOPTS_SET(SPIFOPT_BADOPTS_GET() + 1); \
                       if (SPIFOPT_BADOPTS_GET() >= SPIFOPT_ALLOWBAD_GET()) { \
                         print_error("Error threshold exceeded, giving up.\n"); \
                         SPIFOPT_HELPHANDLER(); \
                       } else { \
                         print_error("Attempting to continue, but strange things may happen.\n"); \
                       } \
                     } while(0)


#endif /* _LIBAST_INTERNAL_H_ */
