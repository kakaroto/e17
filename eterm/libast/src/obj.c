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
 * @file obj.c
 * LibAST Object Infrastructure -- Generic Objects
 *
 * This file contains the basic object class along with its smaller
 * (non-object) analogue, the "null" object.
 *
 * @author Michael Jennings <mej@eterm.org>
 * $Revision$
 * $Date$
 */

static const char cvs_ident[] = "$Id$";

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libast_internal.h>

/* *INDENT-OFF* */
static SPIF_CONST_TYPE(class) o_class = {
    SPIF_DECL_CLASSNAME(obj),
    (spif_func_t) spif_obj_new,
    (spif_func_t) spif_obj_init,
    (spif_func_t) spif_obj_done,
    (spif_func_t) spif_obj_del,
    (spif_func_t) spif_obj_show,
    (spif_func_t) spif_obj_comp,
    (spif_func_t) spif_obj_dup,
    (spif_func_t) spif_obj_type
};
SPIF_TYPE(class) SPIF_CLASS_VAR(obj) = &o_class;
/* *INDENT-ON* */

spif_obj_t
spif_obj_new(void)
{
    spif_obj_t self;

    self = SPIF_ALLOC(obj);
    spif_obj_init(self);
    return self;
}

spif_bool_t
spif_obj_del(spif_obj_t self)
{
    D_OBJ(("Deleting object %8p\n", self));
    spif_obj_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_bool_t
spif_obj_init(spif_obj_t self)
{
    spif_obj_set_class(self, SPIF_CLASS_VAR(obj));
    return TRUE;
}

spif_bool_t
spif_obj_done(spif_obj_t self)
{
    USE_VAR(self);
    return TRUE;
}

spif_class_t
spif_obj_get_class(spif_obj_t self)
{
    return ((self) ? (self->cls) : SPIF_NULL_TYPE(class));
}

spif_bool_t
spif_obj_set_class(spif_obj_t self, spif_class_t cls)
{
    if (SPIF_OBJ_ISNULL(self)) {
        return FALSE;
    }
    SPIF_OBJ_CLASS(self) = cls;
    return TRUE;
}

spif_str_t
spif_obj_show(spif_obj_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];

    if (SPIF_OBJ_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(obj, name, buff, indent, tmp);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_obj_t) %s:  (spif_obj_t) { \"%s\" }\n", name, SPIF_OBJ_CLASSNAME(self));
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }
    return buff;
}

spif_cmp_t
spif_obj_comp(spif_obj_t self, spif_obj_t other)
{
    return (self == other);
}

spif_obj_t
spif_obj_dup(spif_obj_t self)
{
    spif_obj_t tmp;

    tmp = spif_obj_new();
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(obj));
    return tmp;
}

spif_classname_t
spif_obj_type(spif_obj_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}



/**
 * @defgroup DOXGRP_OBJ LibAST Object Infrastructure
 *
 * This group of types, functions, and pre-processor macros implements
 * a mechanism for defining and utilizing objects in native C.
 *
 * C, as you well know, is a procedural language.  It has no native
 * facilities for doing object-oriented programming.  And thusly was
 * born C++ -- native object facilities with much of the same C syntax
 * we all know and love.  But C++ has one very big (and fatal, IMHO)
 * flaw:  it requires a special compiler.
 *
 * That in and of itself is not the end of the world, but it does
 * create a number of issues in terms of portability, standardization,
 * speed, and efficiency.  Since C has been around for so much longer,
 * most C compilers are very stable and reliable, and their
 * optimization routines often do almost as good a job as writing raw
 * assembly code (particularly the vendor compilers).  C++ offers none
 * of these types of advantages, and C++ compiler availability has
 * historically been sketchy at best.
 *
 * There are really 2 possible solutions to this, both accomplishing
 * the same end result (using the native C compiler to manipulate an
 * object model and hierarchy) in two similar, but distinct, ways.
 * Both approaches require the use of some sort of preprocessor.
 * Option #1 would be to use a dedicated preprocessor, either
 * something like m4 or a new creation.  This option would probably
 * allow for cleaner, more native-looking syntax, but it has similar
 * (and potentially worse) portability problems to those of C++.
 *
 * For these reasons, I chose option #2:  a CPP-based (i.e.,
 * macro-based) object model.  As you might imagine, the syntax and
 * usage of such a model bears almost no resemblence whatsoever to
 * that of a native OO language, as it relies heavily on type-casting
 * and namespace safety measures implemented within the existing C/CPP
 * structure.  However, the resultant code is native C, which means
 * you can manipulate data using OO techniques like inheritance,
 * interface classes, etc., without incurring the speed/portability
 * penalties of using the C++ compiler.  Plus, you can build libraries
 * that can be easily linked to both C and C++ programs.
 *
 * If you'd like to see a sample program which demonstrates creation
 * and management of LibAST objects, please look here:
 * @link obj_example.c @endlink.
 */

/**
 * @example obj_example.c
 * Example code for using the LibAST Object Infrastructure
 *
 * This is a contrived, but informational, example of using LibAST's
 * object system.  <MORE HERE>
 *
 * Here's the complete source code:
 */
