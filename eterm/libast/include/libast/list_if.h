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

#ifndef _LIBAST_LIST_IF_H_
#define _LIBAST_LIST_IF_H_

/*
 * interface goop
 */

/* Standard typecast macros.... */
#define SPIF_LIST(o)                                    (SPIF_CAST(obj) (o))
#define SPIF_LIST_CLASS(o)                              (SPIF_CAST(listclass) SPIF_OBJ_CLASS(o))

/* Call a method on an instance of an implementation class */
#define SPIF_LIST_CALL_METHOD(obj, meth)                SPIF_LIST_CLASS(obj)->meth

/* Calls to the basic functions. */
#define SPIF_LIST_NEW(type)                             SPIF_LIST((SPIF_CLASS(SPIF_CLASS_VAR(type)))->noo())
#define SPIF_LIST_INIT(obj)                             SPIF_OBJ_INIT(obj)
#define SPIF_LIST_DONE(obj)                             SPIF_OBJ_DONE(obj)
#define SPIF_LIST_DEL(obj)                              SPIF_OBJ_DEL(obj)
#define SPIF_LIST_SHOW(obj, b, i)                       SPIF_OBJ_SHOW(obj, b, i)
#define SPIF_LIST_COMP(o1, o2)                          SPIF_OBJ_COMP(o1, o2)
#define SPIF_LIST_DUP(obj)                              SPIF_OBJ_DUP(obj)
#define SPIF_LIST_TYPE(obj)                             SPIF_OBJ_TYPE(obj)

#define SPIF_LIST_APPEND(obj, item)                     SPIF_CAST(bool) ((SPIF_LIST_CALL_METHOD(obj, append))(obj, item))
#define SPIF_LIST_CONTAINS(obj, item)                   SPIF_CAST(bool) ((SPIF_LIST_CALL_METHOD((obj), contains))(obj, item))
#define SPIF_LIST_COUNT(obj)                            SPIF_CAST_C(size_t) ((SPIF_LIST_CALL_METHOD((obj), count))(obj))
#define SPIF_LIST_GET(obj, index)                       SPIF_CAST(obj) ((SPIF_LIST_CALL_METHOD((obj), get))(obj, index))
#define SPIF_LIST_INDEX(obj, item)                      SPIF_CAST_C(size_t) ((SPIF_LIST_CALL_METHOD((obj), index))(obj, item))
#define SPIF_LIST_INSERT(obj, item)                     SPIF_CAST(bool) ((SPIF_LIST_CALL_METHOD((obj), insert))(obj, item))
#define SPIF_LIST_INSERT_AT(obj, item, index)           SPIF_CAST(bool) ((SPIF_LIST_CALL_METHOD((obj), insert_at))(obj, item, index))
#define SPIF_LIST_ITERATOR(obj)                         SPIF_CAST(bool) ((SPIF_LIST_CALL_METHOD((obj), iterator))(obj))
#define SPIF_LIST_NEXT(obj)                             SPIF_CAST(obj) ((SPIF_LIST_CALL_METHOD((obj), next))(obj))
#define SPIF_LIST_PREPEND(obj, item)                    SPIF_CAST(bool) ((SPIF_LIST_CALL_METHOD((obj), prepend))(obj, item))
#define SPIF_LIST_REMOVE(obj, item)                     SPIF_CAST(obj) ((SPIF_LIST_CALL_METHOD((obj), remove))(obj, item))
#define SPIF_LIST_REMOVE_AT(obj, index)                 SPIF_CAST(obj) ((SPIF_LIST_CALL_METHOD((obj), remove_at))(obj, index))

typedef spif_obj_t spif_list_t;
typedef struct spif_listclass_t_struct *spif_listclass_t;
typedef struct spif_listclass_t_struct spif_const_listclass_t;

struct spif_listclass_t_struct {
  spif_const_class_t parent;

  spif_memberfunc_t append;
  spif_memberfunc_t contains;
  spif_memberfunc_t count;
  spif_memberfunc_t get;
  spif_memberfunc_t index;
  spif_memberfunc_t insert;
  spif_memberfunc_t insert_at;
  spif_memberfunc_t iterator;
  spif_memberfunc_t next;
  spif_memberfunc_t prepend;
  spif_memberfunc_t remove;
  spif_memberfunc_t remove_at;
};

#endif /* _LIBAST_LIST_IF_H_ */
