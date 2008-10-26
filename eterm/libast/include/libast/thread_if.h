/*
 * Copyright (C) 1997-2004, Michael Jennings
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

#ifndef _LIBAST_THREAD_IF_H_
#define _LIBAST_THREAD_IF_H_

/*
 * interface goop
 */

/* Standard typecast macros.... */
#define SPIF_THREAD(o)                        ((spif_thread_t) (o))
#define SPIF_THREAD_CLASS(o)                  ((spif_threadclass_t) SPIF_OBJ_CLASS(o))

/* Name of class variable associated with thread interface */
#define SPIF_THREADCLASS_VAR(type)            spif_ ## type ## _threadclass

/* Check if a thread is NULL */
#define SPIF_THREAD_ISNULL(o)                 (SPIF_THREAD(o) == (spif_thread_t) NULL)

/* Check if an object is a thread */
#define SPIF_OBJ_IS_THREAD(o)                 SPIF_OBJ_IS_TYPE(o, thread)

/* Call a method on an instance of an implementation class */
#define SPIF_THREAD_CALL_METHOD(o, meth)      SPIF_THREAD_CLASS(o)->meth

/* Calls to the basic functions. */
#define SPIF_THREAD_NEW(type)                 SPIF_THREAD((SPIF_CLASS(SPIF_THREADCLASS_VAR(type)))->noo())
#define SPIF_THREAD_INIT(o)                   SPIF_OBJ_INIT(o)
#define SPIF_THREAD_DONE(o)                   SPIF_OBJ_DONE(o)
#define SPIF_THREAD_DEL(o)                    SPIF_OBJ_DEL(o)
#define SPIF_THREAD_SHOW(o, b, i)             SPIF_OBJ_SHOW(o, b, i)
#define SPIF_THREAD_COMP(o1, o2)              SPIF_OBJ_COMP(o1, o2)
#define SPIF_THREAD_DUP(o)                    SPIF_OBJ_DUP(o)
#define SPIF_THREAD_TYPE(o)                   SPIF_OBJ_TYPE(o)

#define SPIF_THREAD_NEW_WITH_FUNC(type, f, d) SPIF_THREAD((SPIF_CLASS(SPIF_THREADCLASS_VAR(type)))->new_with_func((f), (d)))
#define SPIF_THREAD_INIT_WITH_FUNC(o, f, d)   (spif_bool_t) ((SPIF_THREAD_CALL_METHOD((o), init_with_func))((o), (f), (d)))
#define SPIF_THREAD_DETACH(o)                 (spif_bool_t) ((SPIF_THREAD_CALL_METHOD((o), detach))(o))
#define SPIF_THREAD_GET_CONDITION(o)          (spif_condition_t) ((SPIF_THREAD_CALL_METHOD((o), get_condition))())
#define SPIF_THREAD_GET_MUTEX(o)              (spif_mutex_t) ((SPIF_THREAD_CALL_METHOD((o), get_mutex))())
#define SPIF_THREAD_KILL(o, s)                (spif_bool_t) ((SPIF_THREAD_CALL_METHOD((o), kill))((o), (s)))
#define SPIF_THREAD_RUN(o)                    (spif_bool_t) ((SPIF_THREAD_CALL_METHOD((o), run))(o))
#define SPIF_THREAD_TLS_CALLOC(o, c, s)       (spif_tls_handle_t) ((SPIF_THREAD_CALL_METHOD((o), tls_calloc))((o), (c), (s)))
#define SPIF_THREAD_TLS_FREE(o, i)            (spif_bool_t) ((SPIF_THREAD_CALL_METHOD((o), tls_free))((o), (i)))
#define SPIF_THREAD_TLS_GET(o, i)             (spif_ptr_t) ((SPIF_THREAD_CALL_METHOD((o), tls_get))((o), (i)))
#define SPIF_THREAD_TLS_MALLOC(o, s)          (spif_tls_handle_t) ((SPIF_THREAD_CALL_METHOD((o), tls_malloc))((o), (s)))
#define SPIF_THREAD_TLS_REALLOC(o, i, s)      (spif_bool_t) ((SPIF_THREAD_CALL_METHOD((o), tls_realloc))((o), (i), (s)))
#define SPIF_THREAD_WAIT(o, c)                (spif_bool_t) ((SPIF_THREAD_CALL_METHOD((o), wait))((o), (c)))
#define SPIF_THREAD_WAIT_FOR(o, t)            (spif_bool_t) ((SPIF_THREAD_CALL_METHOD((o), wait_for))((o), (t)))

typedef spif_obj_t spif_thread_t;
typedef spif_ptr_t spif_thread_data_t;
typedef spif_thread_data_t (*spif_thread_func_t)(spif_thread_data_t);
typedef spif_listidx_t spif_tls_handle_t;

SPIF_DECL_OBJ(threadclass) {
    SPIF_DECL_PARENT_TYPE(class);

    spif_func_t new_with_func;
    spif_func_t init_with_func;
    spif_func_t detach;
    spif_func_t get_condition;
    spif_func_t get_mutex;
    spif_func_t kill;
    spif_func_t run;
    spif_func_t tls_calloc;
    spif_func_t tls_free;
    spif_func_t tls_get;
    spif_func_t tls_malloc;
    spif_func_t tls_realloc;
    spif_func_t wait;
    spif_func_t wait_for;
};

#endif /* _LIBAST_THREAD_IF_H_ */
