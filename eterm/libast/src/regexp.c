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
static SPIF_CONST_TYPE(class) r_class = {
    SPIF_DECL_CLASSNAME(regexp),
    (spif_func_t) spif_regexp_new,
    (spif_func_t) spif_regexp_init,
    (spif_func_t) spif_regexp_done,
    (spif_func_t) spif_regexp_del,
    (spif_func_t) spif_regexp_show,
    (spif_func_t) spif_regexp_cmp,
    (spif_func_t) spif_regexp_dup,
    (spif_func_t) spif_regexp_type
};
SPIF_TYPE(class) SPIF_CLASS_VAR(regexp) = &r_class;
/* *INDENT-ON* */

spif_regexp_t
spif_regexp_new(void)
{
    spif_regexp_t self;

    self = SPIF_ALLOC(regexp);
    spif_regexp_init(self);
    return self;
}

spif_regexp_t
spif_regexp_new_from_ptr(spif_charptr_t old)
{
    spif_regexp_t self;

    self = SPIF_ALLOC(regexp);
    spif_regexp_init_from_ptr(self, old);
    return self;
}

spif_bool_t
spif_regexp_del(spif_regexp_t self)
{
    spif_regexp_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_bool_t
spif_regexp_init(spif_regexp_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(regexp));
    return TRUE;
}

spif_bool_t
spif_regexp_init_from_ptr(spif_regexp_t self, spif_charptr_t old)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(regexp));
    return TRUE;
}

spif_bool_t
spif_regexp_done(spif_regexp_t self)
{
    REQUIRE_RVAL(!SPIF_REGEXP_ISNULL(self), FALSE);
    return TRUE;
}

spif_regexp_t
spif_regexp_dup(spif_regexp_t orig)
{
    spif_regexp_t self;

    REQUIRE_RVAL(!SPIF_REGEXP_ISNULL(orig), FALSE);
    return self;
}

spif_cmp_t
spif_regexp_cmp(spif_regexp_t self, spif_regexp_t other)
{

}

spif_regexp_t
spif_regexp_show(spif_regexp_t self, spif_charptr_t name, spif_regexp_t buff, size_t indent)
{
    char tmp[4096];

    if (SPIF_REGEXP_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(regexp, name, buff, indent);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_regexp_t) %s:  %8p {\n", name, self);
    if (SPIF_REGEXP_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }

    snprintf(tmp, sizeof(tmp), "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

spif_classname_t
spif_regexp_type(spif_regexp_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}
