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

/* Convenience macros for declaring object structures and types. */
#define SPIF_DECL_OBJ_STRUCT(t)          struct spif_ ## t ## _t_struct
#define SPIF_DEFINE_OBJ(t)               SPIF_DEFINE_TYPE(t, SPIF_DECL_OBJ_STRUCT(t)); SPIF_DECL_OBJ_STRUCT(t)

/* Cast an arbitrary object pointer to a pointer to a nullobj.  Coincidentally,
   a nullobj *is* an arbitrary object pointer.  Even moreso than an obj. :-) */
#define SPIF_NULLOBJ(obj)                ((spif_nullobj_t) (obj))

/* Typecase macros for classes */
#define SPIF_CLASS(cls)                  (SPIF_CAST(class) (cls))
#define SPIF_CONST_CLASS(cls)            (SPIF_CONST_CAST(class) (cls))

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

#define SPIF_OBJ_SHOW_NULL(t, n, b, i)   do { \
                                           memset(tmp, ' ', (i)); \
                                           snprintf(tmp + (i), sizeof(tmp) - (i), "(spif_" #t "_t) %s:  " \
                                                     SPIF_NULLSTR_TYPE(t) "\n", NONULL(n)); \
                                           if (SPIF_STR_ISNULL(b)) { \
                                             (b) = spif_str_new_from_ptr(tmp); \
                                           } else { \
                                             spif_str_append_from_ptr((b), tmp); \
                                           } \
                                         } while (0)


/* The type for the classname variables.  I don't see any reason why this
   would be anything but a const char *, but you never know.  :-) */
typedef const char *spif_classname_t;

/* Generic function pointer. */
typedef void * (*spif_func_t)();

/* A nullobj contains...well, nothing.  Any class that is really small and/or
   needs to be very memory-efficient should be derived from this class. */
SPIF_DEFINE_OBJ(nullobj) {
};

/* The class contains the function pointers for the generic object functions. */
SPIF_DEFINE_OBJ(class) {
  spif_classname_t classname;

  spif_func_t noo;  /* FIXME:  Do we really need this? */
  spif_func_t init;
  spif_func_t done;
  spif_func_t del;
  spif_func_t show;
  spif_func_t comp;
  spif_func_t dup;
  spif_func_t type;
};

/* An obj is the most basic object type.  It contains simply a pointer to
   the class name (a const char * so you can test it with ==). */
SPIF_DEFINE_OBJ(obj) {
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
