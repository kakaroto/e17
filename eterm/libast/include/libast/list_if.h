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
#define SPIF_LIST_INST(obj)                             SPIF_IFINST(obj)
#define SPIF_LIST_CLASS(cls)                            ((spif_ifclass_t) (cls))

/* Access the implementation class member of an object. */
#define SPIF_LIST_IFCLASS(obj)                          (SPIF_IFINST(obj)->ifclass)

/* Call a method on an instance of an implementation class */
#define SPIF_LIST_CALL_METHOD(obj, meth)                (SPIF_LIST_IFCLASS(obj)->(meth))

/* Calls to the basic functions. */
#define SPIF_LIST_NEW(type)                             SPIF_IFCLASS_NEW(SPIF_IFCLASS_VAR(list, type))
#define SPIF_LIST_INIT(obj)                             SPIF_IFINST_INIT(obj)
#define SPIF_LIST_DONE(obj)                             SPIF_IFINST_DONE(obj)
#define SPIF_LIST_DEL(obj)                              SPIF_IFINST_DEL(obj)
#define SPIF_LIST_SHOW(obj)                             SPIF_IFINST_SHOW(obj)

#define SPIF_LIST_APPEND(obj, item)                     ((SPIF_LIST_CALL_METHOD((obj), (append)))(obj, item))
#define SPIF_LIST_CONTAINS(obj, item)                   ((SPIF_LIST_CALL_METHOD((obj), (contains)))(obj, item))
#define SPIF_LIST_COUNT(obj)                            ((SPIF_LIST_CALL_METHOD((obj), (count)))(obj))
#define SPIF_LIST_GET(obj, index)                       ((SPIF_LIST_CALL_METHOD((obj), (get)))(obj, index))
#define SPIF_LIST_INDEX(obj, item)                      ((SPIF_LIST_CALL_METHOD((obj), (index)))(obj, item))
#define SPIF_LIST_INSERT(obj, item)                     ((SPIF_LIST_CALL_METHOD((obj), (insert)))(obj, item))
#define SPIF_LIST_INSERT_AT(obj, item, index)           ((SPIF_LIST_CALL_METHOD((obj), (insert_at)))(obj, item, index))
#define SPIF_LIST_ITERATOR(obj)                         ((SPIF_LIST_CALL_METHOD((obj), (iterator)))(obj))
#define SPIF_LIST_NEXT(obj)                             ((SPIF_LIST_CALL_METHOD((obj), (next)))(obj))
#define SPIF_LIST_PREPEND(obj, item)                    ((SPIF_LIST_CALL_METHOD((obj), (prepend)))(obj, item))
#define SPIF_LIST_REMOVE(obj)                           ((SPIF_LIST_CALL_METHOD((obj), (remove)))(obj))
#define SPIF_LIST_REMOVE_AT(obj, index)                 ((SPIF_LIST_CALL_METHOD((obj), (remove_at)))(obj, index))

/* The ifclass object contains the function pointers for the interface methods. */
typedef struct spif_list_ifclass_t_struct *spif_list_ifclass_t;
typedef struct spif_list_ifclass_t_struct spif_const_list_ifclass_t;

struct spif_list_ifclass_t_struct {
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
