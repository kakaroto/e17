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

static const char cvs_ident[] = "$Id$";

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libast_internal.h>

/* *INDENT-OFF* */
static spif_const_class_t o_class = {
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
spif_class_t SPIF_CLASS_VAR(obj) = &o_class;
/* *INDENT-ON* */

spif_nullobj_t
spif_nullobj_new(void)
{
    /* DO NOT USE */
    return ((spif_nullobj_t) (NULL));
}

spif_bool_t
spif_nullobj_del(spif_nullobj_t self)
{
    /* DO NOT USE */
    USE_VAR(self);
    return TRUE;
}

spif_bool_t
spif_nullobj_init(spif_nullobj_t self)
{
    /* DO NOT USE */
    USE_VAR(self);
    return TRUE;
}

spif_bool_t
spif_nullobj_done(spif_nullobj_t self)
{
    /* DO NOT USE */
    USE_VAR(self);
    return TRUE;
}

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
        SPIF_OBJ_SHOW_NULL("obj", name, buff, indent);
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
