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

#ifndef _LIBAST_OBJ_H_
#define _LIBAST_OBJ_H_

/* Internal libast use only.  These abstract malloc() and free() for class
   constructors/destructors.  Spankings to anyone who abuses these macros! */
#define SPIF_TYPE(type)              (spif_ ## type ## _t)
#define SPIF_CONST_TYPE(type)        (spif_const_ ## type ## _t)
#define SPIF_ALLOC(type)             SPIF_TYPE(type) MALLOC(sizeof SPIF_CONST_TYPE(type))
#define SPIF_DEALLOC(obj)            FREE(obj)
#define SPIF_DECL_CNVAR(type)        "!spif_" #type "_t!"
#define SPIF_DECL_CLASSNAME(type)    spif_classname_t spif_ ## type ## _classname = SPIF_DECL_CNVAR(type)

/* Check to see if a pointer references an object.  Increasing levels
   of accuracy at the expense of some speed for the higher debug levels.
   This is also internal.  It's used by other SPIF_OBJ_IS_*() macros. */
#if DEBUG == 0
#  define SPIF_OBJ_IS_TYPE(o, type)  (1)
#elsif DEBUG <= 4
#  define SPIF_OBJ_IS_TYPE(o, type)  (!SPIF_OBJ_ISNULL(o))
#else
#  define SPIF_OBJ_IS_TYPE(o, type)  ((!SPIF_OBJ_ISNULL(o)) && (SPIF_OBJ_CLASSNAME(o) == SPIF_CLASSNAME(type)))
#endif

/* Cast an arbitrary object pointer to a pointer to a nullobj.  Coincidentally,
   a nullobj *is* an arbitrary object pointer.  Even moreso than an obj. :-) */
#define SPIF_NULLOBJ(obj)            ((spif_nullobj_t) (obj))

/* Cast an arbitrary object pointer to an obj.  Any object of sufficient size
   and/or complexity should be derived from this type. */
#define SPIF_OBJ(obj)                ((spif_obj_t) (obj))
#define SPIF_OBJ_CLASSNAME(obj)      (SPIF_OBJ(obj)->classname)

/* Check to see if a pointer references an obj. */
#define SPIF_OBJ_IS_OBJ(o)           (SPIF_OBJ_IS_TYPE(o, obj))

/* Converts a type (such as "obj") to the name of its classname variable. */
#define SPIF_CLASSNAME(type)         ((spif_classname_t) (spif_ ## type ## _classname))

/* Used for testing the NULL-ness of objects. */
#define SPIF_NULL_TYPE(type)         (SPIF_TYPE(type) (NULL))
#define SPIF_OBJ_ISNULL(o)           (SPIF_OBJ(o) == SPIF_NULL_TYPE(obj))

/* Converts a type (such as "obj") to a string denoting a NULL object of that type. */
#define SPIF_NULL_STR(type)          ("{ ((spif_" #type "_t) NULL) }")

#define SPIF_OBJ_SIZEOF(type)        (sizeof SPIF_CONST_TYPE(type))

/* The type for the classname variables.  I don't see any reason why this
   would be anything but a const char *, but you never know.  :-) */
typedef const char *spif_classname_t;

/* Types for null objects */
typedef struct spif_nullobj_t_struct *spif_nullobj_t;
typedef struct spif_nullobj_t_struct spif_const_nullobj_t;

/* A nullobj contains...well, nothing.  Any class that is really small and/or
   needs to be very memory-efficient should be derived from this class. */
struct spif_nullobj_t_struct {
};

/* An obj is the most basic object type.  It contains simply a pointer to
   the class name (a const char * so you can test it with ==). */
typedef struct spif_obj_t_struct *spif_obj_t;
typedef struct spif_obj_t_struct spif_const_obj_t;

struct spif_obj_t_struct {
  spif_classname_t classname;
};


extern spif_nullobj_t spif_nullobj_new(void);
extern spif_bool_t spif_nullobj_del(spif_nullobj_t);
extern spif_bool_t spif_nullobj_init(spif_nullobj_t);
extern spif_bool_t spif_nullobj_done(spif_nullobj_t);

extern spif_classname_t spif_obj_classname;
extern spif_obj_t spif_obj_new(void);
extern spif_bool_t spif_obj_del(spif_obj_t);
extern spif_bool_t spif_obj_init(spif_obj_t);
extern spif_bool_t spif_obj_done(spif_obj_t);
extern spif_classname_t spif_obj_get_classname(spif_obj_t);
extern spif_bool_t spif_obj_set_classname(spif_obj_t, spif_classname_t);
extern spif_bool_t spif_obj_show(spif_obj_t);

#endif /* _LIBAST_OBJ_H_ */
