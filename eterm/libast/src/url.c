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
static spif_const_class_t u_class = {
    SPIF_DECL_CLASSNAME(url),
    (spif_newfunc_t) spif_url_new,
    (spif_memberfunc_t) spif_url_init,
    (spif_memberfunc_t) spif_url_done,
    (spif_memberfunc_t) spif_url_del,
    (spif_func_t) spif_url_show,
    (spif_func_t) spif_url_comp,
    (spif_func_t) spif_url_dup,
    (spif_func_t) spif_url_type
};
spif_class_t SPIF_CLASS_VAR(url) = &u_class;
/* *INDENT-ON* */

spif_url_t
spif_url_new(void)
{
    spif_url_t self;

    self = SPIF_ALLOC(url);
    spif_url_init(self);
    return self;
}

spif_url_t
spif_url_new_from_str(spif_str_t other)
{
    spif_url_t self;

    self = SPIF_ALLOC(url);
    spif_url_init_from_str(self, other);
    return self;
}

spif_url_t
spif_url_new_from_ptr(spif_charptr_t other)
{
    spif_url_t self;

    self = SPIF_ALLOC(url);
    spif_url_init_from_ptr(self, other);
    return self;
}

spif_bool_t
spif_url_del(spif_url_t self)
{
    spif_url_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_bool_t
spif_url_init(spif_url_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_url_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(url));
    return TRUE;
}

spif_bool_t
spif_url_init_from_str(spif_url_t self, spif_str_t other)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_url_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(url));
    return TRUE;
}

spif_bool_t
spif_url_init_from_ptr(spif_url_t self, spif_charptr_t other)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_url_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(url));
    return TRUE;
}

spif_bool_t
spif_url_done(spif_url_t self)
{
    USE_VAR(self);
    return TRUE;
}

spif_str_t
spif_url_show(spif_url_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_url_t) %s:  {\n", name);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }
    return buff;
}

spif_cmp_t
spif_url_comp(spif_url_t self, spif_url_t other)
{
    return (self == other);
}

spif_url_t
spif_url_dup(spif_url_t self)
{
    spif_url_t tmp;

    tmp = spif_url_new();
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(url));
    return tmp;
}

spif_classname_t
spif_url_type(spif_url_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}

spif_str_t
spif_url_get_proto(spif_url_t self)
{
}

spif_bool_t
spif_url_set_proto(spif_url_t self, spif_str_t newproto)
{
}

spif_str_t
spif_url_get_user(spif_url_t self)
{
}

spif_bool_t
spif_url_set_user(spif_url_t self, spif_str_t newuser)
{
}

spif_str_t
spif_url_get_passwd(spif_url_t self)
{
}

spif_bool_t
spif_url_set_passwd(spif_url_t self, spif_str_t newpasswd)
{
}

spif_str_t
spif_url_get_host(spif_url_t self)
{
}

spif_bool_t
spif_url_set_host(spif_url_t self, spif_str_t newhost)
{
}

spif_str_t
spif_url_get_port(spif_url_t self)
{
}

spif_bool_t
spif_url_set_port(spif_url_t self, spif_str_t newport)
{
}

spif_str_t
spif_url_get_path(spif_url_t self)
{
}

spif_bool_t
spif_url_set_path(spif_url_t self, spif_str_t newpath)
{
}

spif_str_t
spif_url_get_query(spif_url_t self)
{
}

spif_bool_t
spif_url_set_query(spif_url_t self, spif_str_t newquery)
{
}
