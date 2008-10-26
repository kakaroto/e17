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

#ifndef _LIBAST_CONDITION_IF_H_
#define _LIBAST_CONDITION_IF_H_

/*
 * interface goop
 */

/* Standard typecast macros.... */
#define SPIF_CONDITION(o)                         ((spif_condition_t) (o))
#define SPIF_CONDITION_CLASS(o)                   ((spif_conditionclass_t) SPIF_OBJ_CLASS(o))

/* Name of class variable associated with condition interface */
#define SPIF_CONDITIONCLASS_VAR(type)             spif_ ## type ## _conditionclass

/* Check if a condition is NULL */
#define SPIF_CONDITION_ISNULL(o)                  (SPIF_CONDITION(o) == (spif_condition_t) NULL)

/* Check if an object is a condition */
#define SPIF_OBJ_IS_CONDITION(o)                  SPIF_OBJ_IS_TYPE(o, condition)

/* Call a method on an instance of an implementation class */
#define SPIF_CONDITION_CALL_METHOD(o, meth)       SPIF_CONDITION_CLASS(o)->meth

/* Calls to the basic functions. */
#define SPIF_CONDITION_NEW(type)                  SPIF_CONDITION((SPIF_CLASS(SPIF_CONDITIONCLASS_VAR(type)))->noo())
#define SPIF_CONDITION_INIT(o)                    SPIF_OBJ_INIT(o)
#define SPIF_CONDITION_DONE(o)                    SPIF_OBJ_DONE(o)
#define SPIF_CONDITION_DEL(o)                     SPIF_OBJ_DEL(o)
#define SPIF_CONDITION_SHOW(o, b, i)              SPIF_OBJ_SHOW(o, b, i)
#define SPIF_CONDITION_COMP(o1, o2)               SPIF_OBJ_COMP(o1, o2)
#define SPIF_CONDITION_DUP(o)                     SPIF_OBJ_DUP(o)
#define SPIF_CONDITION_TYPE(o)                    SPIF_OBJ_TYPE(o)

#define SPIF_CONDITION_BROADCAST(o)                (spif_bool_t) ((SPIF_CONDITION_CALL_METHOD((o), broadcast))(o))
#define SPIF_CONDITION_SIGNAL(o)                   (spif_bool_t) ((SPIF_CONDITION_CALL_METHOD((o), signal))(o))
#define SPIF_CONDITION_WAIT(o)                     (spif_bool_t) ((SPIF_CONDITION_CALL_METHOD((o), wait))(o))
#define SPIF_CONDITION_WAIT_TIMED(o, t)            (spif_bool_t) ((SPIF_CONDITION_CALL_METHOD((o), wait_timed))((o), (t)))

typedef spif_obj_t spif_condition_t;

SPIF_DECL_OBJ(conditionclass) {
    SPIF_DECL_PARENT_TYPE(class);

    spif_func_t broadcast;
    spif_func_t signal;
    spif_func_t wait;
    spif_func_t wait_timed;
};

#endif /* _LIBAST_CONDITION_IF_H_ */
