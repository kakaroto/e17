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

/*
 * Generic macro goop
 */

/* Macros to construct object types from the basenames of the types (obj, str, etc.) */
#define SPIF_TYPE(type)                  spif_ ## type ## _t
#define SPIF_CONST_TYPE(type)            spif_const_ ## type ## _t

/* Macros to allocate and deallocate memory for an object.  For use only in
   object constructors/destructors, not in end-user programs. */
#define SPIF_ALLOC(type)                 SPIF_CAST(type) MALLOC(SPIF_SIZEOF_TYPE(type))
#define SPIF_DEALLOC(obj)                FREE(obj)

/* Macros for specifying the classname variables for each class type.  Each subclass of
   spif_obj_t must define this variable using these macros. */
#define SPIF_DECL_CLASSNAME(type)        "!spif_" #type "_t!"

/* Typecast macros */
#define SPIF_CAST_C(type)                (type)
#define SPIF_CONST_CAST_C(type)          (const type)
#define SPIF_CAST(type)                  (SPIF_TYPE(type))
#define SPIF_CONST_CAST(type)            (const SPIF_TYPE(type))
#define SPIF_CAST_PTR(type)              (SPIF_TYPE(type) *)
#define SPIF_CONST_CAST_PTR(type)        (const SPIF_TYPE(type) *)

/* Cast the NULL pointer to a particular object type. */
#define SPIF_NULL_TYPE(type)             (SPIF_CAST(type) (NULL))
#define SPIF_NULL_CTYPE(type)            (SPIF_CAST_C(type) (NULL))

/* Converts a type (such as "obj") to a string denoting a NULL object of that type. */
#define SPIF_NULLSTR_TYPE(type)          "{ ((spif_" #type "_t) NULL) }"
#define SPIF_NULLSTR_CTYPE(type)         "{ ((" #type ") NULL) }"

/* Our own version of the sizeof() operator since objects are actually pointers. */
#define SPIF_SIZEOF_TYPE(type)           (sizeof(SPIF_CONST_TYPE(type)))


/* Cast an arbitrary object pointer to a pointer to a nullobj.  Coincidentally,
   a nullobj *is* an arbitrary object pointer.  Even moreso than an obj. :-) */
#define SPIF_NULLOBJ(obj)                ((spif_nullobj_t) (obj))

/* Typecase macros for classes */
#define SPIF_CLASS(cls)                  ((SPIF_TYPE(class)) (cls))
#define SPIF_CONST_CLASS(cls)            ((SPIF_CONST_TYPE(class)) (cls))

/* Assembles the name of the object class variable. */
#define SPIF_CLASS_VAR(type)             spif_ ## type ## _class

/* Check to see if a pointer references an object.  Increasing levels
   of accuracy at the expense of some speed for the higher debug levels.
   This is also internal.  It's used by other SPIF_OBJ_IS_*() macros. */
#define SPIF_OBJ_IS_TYPE(o, type)        ((!SPIF_OBJ_ISNULL(o)) && (SPIF_OBJ_CLASS(o) == SPIF_CLASS_VAR(type)))
#if DEBUG == 0
#  define SPIF_OBJ_CHECK_TYPE(o, type)   (1)
#elsif DEBUG <= 4
#  define SPIF_OBJ_CHECK_TYPE(o, type)   (!SPIF_OBJ_ISNULL(o))
#else
#  define SPIF_OBJ_CHECK_TYPE(o, type)   SPIF_OBJ_IS_TYPE(o, type)
#endif

/* Cast an arbitrary object pointer to an obj.  Any object of sufficient size
   and/or complexity should be derived from this type. */
#define SPIF_OBJ(obj)                    ((spif_obj_t) (obj))

/* Check to see if a pointer references an obj. */
#define SPIF_OBJ_IS_OBJ(o)               (SPIF_OBJ_IS_TYPE(o, obj))

/* Used for testing the NULL-ness of objects. */
#define SPIF_OBJ_ISNULL(o)               (SPIF_OBJ(o) == SPIF_NULL_TYPE(obj))

/* Access the implementation class member of an object. */
#define SPIF_OBJ_CLASS(obj)              (SPIF_CLASS(SPIF_OBJ(obj)->cls))

/* Get the classname...very cool. */
#define SPIF_OBJ_CLASSNAME(obj)          (SPIF_CAST(classname) SPIF_OBJ_CLASS(obj))

/* Call a method on an instance of an implementation class */
#define SPIF_OBJ_CALL_METHOD(obj, meth)  SPIF_OBJ_CLASS(obj)->meth

/* Calls to the basic functions. */
#define SPIF_OBJ_NEW()                   SPIF_CAST(obj) (SPIF_CLASS(SPIF_CLASS_VAR(obj)))->(noo)()
#define SPIF_OBJ_INIT(o)                 SPIF_CAST(bool) (SPIF_OBJ_CALL_METHOD((o), init)(o))
#define SPIF_OBJ_DONE(o)                 SPIF_CAST(bool) (SPIF_OBJ_CALL_METHOD((o), done)(o))
#define SPIF_OBJ_DEL(o)                  SPIF_CAST(bool) (SPIF_OBJ_CALL_METHOD((o), del)(o))
#define SPIF_OBJ_SHOW(o, b, i)           SPIF_CAST(str) (SPIF_OBJ_CALL_METHOD((o), show)(o, #o, b, i))
#define SPIF_OBJ_COMP(o1, o2)            SPIF_CAST(cmp) (SPIF_OBJ_CALL_METHOD((o1),  comp)(o1, o2))
#define SPIF_OBJ_DUP(o)                  SPIF_CAST(obj) (SPIF_OBJ_CALL_METHOD((o), dup)(o))
#define SPIF_OBJ_TYPE(o)                 SPIF_CAST(classname) (SPIF_OBJ_CALL_METHOD((o), type)(o))

/* Convenience macro */
#define SPIF_SHOW(o, fd)                 do { \
                                           spif_str_t tmp__; \
                                           tmp__ = SPIF_OBJ_SHOW(o, SPIF_NULL_TYPE(str), 0); \
                                           fprintf(fd, "%s\n", SPIF_STR_STR(tmp__)); \
                                           spif_str_del(tmp__); \
                                         } while (0)



/* Types for null objects */
typedef struct spif_nullobj_t_struct *spif_nullobj_t;
typedef struct spif_nullobj_t_struct spif_const_nullobj_t;

/* An obj is the most basic object type.  It contains simply a pointer to
   the class name (a const char * so you can test it with ==). */
typedef struct spif_obj_t_struct *spif_obj_t;
typedef struct spif_obj_t_struct spif_const_obj_t;

/* The type for the classname variables.  I don't see any reason why this
   would be anything but a const char *, but you never know.  :-) */
typedef const char *spif_classname_t;

/* The class contains the function pointers for the generic object functions. */
typedef struct spif_class_t_struct *spif_class_t;
typedef struct spif_class_t_struct spif_const_class_t;

/* Generic function pointers. */
typedef spif_obj_t (*spif_newfunc_t)(void);
typedef spif_bool_t (*spif_memberfunc_t)(spif_obj_t, ...);
typedef void * (*spif_func_t)();



/* A nullobj contains...well, nothing.  Any class that is really small and/or
   needs to be very memory-efficient should be derived from this class. */
struct spif_nullobj_t_struct {
};

struct spif_class_t_struct {
  spif_classname_t classname;

  spif_newfunc_t noo;  /* FIXME:  Do we really need this? */
  spif_memberfunc_t init;
  spif_memberfunc_t done;
  spif_memberfunc_t del;
  spif_func_t show;
  spif_func_t comp;
  spif_func_t dup;
  spif_func_t type;
};

struct spif_obj_t_struct {
  spif_class_t cls;
};

/* We need typedef's from here... */
#include <libast/str.h>

extern spif_class_t SPIF_CLASS_VAR(obj);
extern spif_nullobj_t spif_nullobj_new(void);
extern spif_bool_t spif_nullobj_del(spif_nullobj_t);
extern spif_bool_t spif_nullobj_init(spif_nullobj_t);
extern spif_bool_t spif_nullobj_done(spif_nullobj_t);
extern spif_obj_t spif_obj_new(void);
extern spif_bool_t spif_obj_del(spif_obj_t);
extern spif_bool_t spif_obj_init(spif_obj_t);
extern spif_bool_t spif_obj_done(spif_obj_t);
extern spif_class_t spif_obj_get_class(spif_obj_t);
extern spif_bool_t spif_obj_set_class(spif_obj_t, spif_class_t);
extern spif_str_t spif_obj_show(spif_obj_t, spif_charptr_t, spif_str_t, size_t);
extern spif_cmp_t spif_obj_comp(spif_obj_t, spif_obj_t);
extern spif_obj_t spif_obj_dup(spif_obj_t);
extern spif_classname_t spif_obj_type(spif_obj_t);

#endif /* _LIBAST_OBJ_H_ */
