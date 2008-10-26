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

#ifndef _LIBAST_MUTEX_IF_H_
#define _LIBAST_MUTEX_IF_H_

/*
 * interface goop
 */

/* Standard typecast macros.... */
#define SPIF_MUTEX(o)                                    ((spif_mutex_t) (o))
#define SPIF_MUTEX_CLASS(o)                              ((spif_mutexclass_t) SPIF_OBJ_CLASS(o))

/* Name of class variable associated with mutex interface */
#define SPIF_MUTEXCLASS_VAR(type)                        spif_ ## type ## _mutexclass

/* Check if a mutex is NULL */
#define SPIF_MUTEX_ISNULL(o)                             (SPIF_MUTEX(o) == (spif_mutex_t) NULL)

/* Check if an object is a mutex */
#define SPIF_OBJ_IS_MUTEX(o)                             SPIF_OBJ_IS_TYPE(o, mutex)

/* Call a method on an instance of an implementation class */
#define SPIF_MUTEX_CALL_METHOD(o, meth)                  SPIF_MUTEX_CLASS(o)->meth

/* Calls to the basic functions. */
#define SPIF_MUTEX_NEW(type)                             SPIF_MUTEX((SPIF_CLASS(SPIF_MUTEXCLASS_VAR(type)))->noo())
#define SPIF_MUTEX_INIT(o)                               SPIF_OBJ_INIT(o)
#define SPIF_MUTEX_DONE(o)                               SPIF_OBJ_DONE(o)
#define SPIF_MUTEX_DEL(o)                                SPIF_OBJ_DEL(o)
#define SPIF_MUTEX_SHOW(o, b, i)                         SPIF_OBJ_SHOW(o, b, i)
#define SPIF_MUTEX_COMP(o1, o2)                          SPIF_OBJ_COMP(o1, o2)
#define SPIF_MUTEX_DUP(o)                                SPIF_OBJ_DUP(o)
#define SPIF_MUTEX_TYPE(o)                               SPIF_OBJ_TYPE(o)

#define SPIF_MUTEX_LOCK()                                (spif_bool_t) ((SPIF_MUTEX_CALL_METHOD((o), lock))(o))
#define SPIF_MUTEX_LOCK_NOWAIT()                         (spif_bool_t) ((SPIF_MUTEX_CALL_METHOD((o), lock_nowait))(o))
#define SPIF_MUTEX_UNLOCK()                              (spif_bool_t) ((SPIF_MUTEX_CALL_METHOD((o), unlock))(o))

typedef spif_obj_t spif_mutex_t;

SPIF_DECL_OBJ(mutexclass) {
    SPIF_DECL_PARENT_TYPE(class);

    spif_func_t lock;
    spif_func_t lock_nowait;
    spif_func_t unlock;
};

#endif /* _LIBAST_MUTEX_IF_H_ */
