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

static const char __attribute__((unused)) cvs_ident[] = "$Id$";

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libast_internal.h>
#include <dlfcn.h>

/* *INDENT-OFF* */
static SPIF_CONST_TYPE(moduleclass) s_class = {
    {
        SPIF_DECL_CLASSNAME(module),
        (spif_func_t) spif_module_new,
        (spif_func_t) spif_module_init,
        (spif_func_t) spif_module_done,
        (spif_func_t) spif_module_del,
        (spif_func_t) spif_module_show,
        (spif_func_t) spif_module_comp,
        (spif_func_t) spif_module_dup,
        (spif_func_t) spif_module_type
    },
    (spif_func_t) spif_module_call,
    (spif_func_t) spif_module_getsym,
    (spif_func_t) spif_module_load,
    (spif_func_t) spif_module_run,
    (spif_func_t) spif_module_unload,
};
SPIF_TYPE(class) SPIF_CLASS_VAR(module) = (spif_class_t) &s_class;
SPIF_TYPE(moduleclass) SPIF_MODULECLASS_VAR(module) = &s_class;
/* *INDENT-ON* */

static const size_t buff_inc = 4096;

spif_module_t
spif_module_new(void)
{
    spif_module_t self;

    self = SPIF_ALLOC(module);
    if (!spif_module_init(self)) {
        SPIF_DEALLOC(self);
        self = (spif_module_t) NULL;
    }
    return self;
}

spif_bool_t
spif_module_init(spif_module_t self)
{
    ASSERT_RVAL(!SPIF_MODULE_ISNULL(self), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_MODULECLASS_VAR(module)));
    self->name = (spif_str_t) NULL;
    self->path = (spif_str_t) NULL;
    self->module_handle = (spif_ptr_t) NULL;
    self->main_handle = dlopen(NULL, RTLD_LAZY);
    return TRUE;
}

spif_bool_t
spif_module_done(spif_module_t self)
{
    spif_bool_t ret = TRUE;

    ASSERT_RVAL(!SPIF_MODULE_ISNULL(self), FALSE);
    if (self->module_handle) {
        ret = spif_module_unload(self);
        self->module_handle = (spif_ptr_t) NULL;
    }
    if (!SPIF_STR_ISNULL(self->name)) {
        spif_str_del(self->name);
        self->name = (spif_str_t) NULL;
    }
    if (!SPIF_STR_ISNULL(self->path)) {
        spif_str_del(self->path);
        self->path = (spif_str_t) NULL;
    }
    return ret;
}

spif_bool_t
spif_module_del(spif_module_t self)
{
    ASSERT_RVAL(!SPIF_MODULE_ISNULL(self), FALSE);
    spif_module_done(self);
    dlclose(self->main_handle);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_str_t
spif_module_show(spif_module_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    spif_char_t tmp[4096];

    if (SPIF_MODULE_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(module, name, buff, indent, tmp);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf((char *) tmp + indent, sizeof(tmp) - indent,
             "(spif_module_t) %s:  %10p { \"",
             name, (spif_ptr_t) self);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }

    indent += 2;
    if (indent < sizeof(tmp)) {
        memset(tmp, ' ', indent);
    }
    buff = spif_str_show(self->name, SPIF_CHARPTR("name"), buff, indent);
    buff = spif_str_show(self->path, SPIF_CHARPTR("path"), buff, indent);
    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(spif_ptr_t) module_handle:  0x%p\n", self->module_handle);
    spif_str_append_from_ptr(buff, tmp);

    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(spif_ptr_t) main_handle:  0x%p\n", self->main_handle);
    spif_str_append_from_ptr(buff, tmp);

    snprintf((char *) tmp, sizeof(tmp), "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

spif_cmp_t
spif_module_comp(spif_module_t self, spif_module_t other)
{
    spif_cmp_t ret;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    SPIF_OBJ_COMP_CHECK_NULL(self->name, other->name);
    if (!SPIF_CMP_IS_EQUAL(ret = spif_str_comp(self->name, other->name))) {
        return ret;
    }
    SPIF_OBJ_COMP_CHECK_NULL(self->path, other->path);
    if (!SPIF_CMP_IS_EQUAL(ret = spif_str_comp(self->path, other->path))) {
        return ret;
    }
    return SPIF_CMP_FROM_INT((int) self->module_handle - (int) other->module_handle);
}

spif_module_t
spif_module_dup(spif_module_t self)
{
    spif_module_t tmp;

    ASSERT_RVAL(!SPIF_MODULE_ISNULL(self), (spif_module_t) NULL);
    tmp = SPIF_ALLOC(module);
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(module));
    tmp->name = spif_str_dup(self->name);
    tmp->path = spif_str_dup(self->path);
    return tmp;
}

spif_classname_t
spif_module_type(spif_module_t self)
{
    ASSERT_RVAL(!SPIF_MODULE_ISNULL(self), (spif_classname_t) SPIF_NULLSTR_TYPE(classname));
    return SPIF_OBJ_CLASSNAME(self);
}

spif_ptr_t
spif_module_call(spif_module_t self, spif_charptr_t fname, spif_ptr_t data)
{
    spif_func_t fp;
    spif_charptr_t err;

    fp = (spif_func_t) spif_module_getsym(self, fname);
    if (SPIF_PTR_ISNULL(err)) {
        /* No error.  Proceed. */
        return (fp)(data);
    } else {
        libast_print_warning("Unable to call function %s() from module \"%s\" -- %s\n", fname, SPIF_STR_STR(self->name), err);
    }
    return (spif_ptr_t) NULL;
}

spif_ptr_t
spif_module_getsym(spif_module_t self, spif_charptr_t sym)
{
    spif_ptr_t psym;
    spif_charptr_t err;

    ASSERT_RVAL(!SPIF_MODULE_ISNULL(self), (spif_ptr_t) NULL);
    ASSERT_RVAL(!SPIF_PTR_ISNULL(sym), (spif_ptr_t) NULL);
    REQUIRE_RVAL(!SPIF_PTR_ISNULL(self->main_handle), (spif_ptr_t) NULL);
    dlerror();
    psym = (spif_ptr_t) dlsym(self->module_handle, (const char *) sym);
    err = (spif_charptr_t) dlerror();
    if (SPIF_PTR_ISNULL(err)) {
        /* No error.  Proceed. */
        return psym;
    } else {
        psym = (spif_ptr_t) dlsym(self->main_handle, (const char *) sym);
        err = (spif_charptr_t) dlerror();
        if (SPIF_PTR_ISNULL(err)) {
            /* No error.  Proceed. */
            return psym;
        } else {
#ifdef RTLD_DEFAULT
            psym = (spif_ptr_t) dlsym(RTLD_DEFAULT, (const char *) sym);
            err = (spif_charptr_t) dlerror();
            if (SPIF_PTR_ISNULL(err)) {
                /* No error.  Proceed. */
                return psym;
            } else
#endif
                libast_print_warning("Unable to resolve symbol \"%s\" from module \"%s\" -- %s\n", sym,
                                     SPIF_STR_STR(self->name), err);
        }
    }
    return (spif_ptr_t) NULL;
}

spif_bool_t
spif_module_load(spif_module_t self)
{
    spif_func_t fp;

    ASSERT_RVAL(!SPIF_MODULE_ISNULL(self), FALSE);
    REQUIRE_RVAL(!SPIF_STR_ISNULL(self->path), FALSE);

    if (SPIF_STR_ISNULL(self->name)) {
        spif_stridx_t idx;

        idx = spif_str_rindex(self->path, '/');
        if (idx == spif_str_get_len(self->path)) {
            self->name = spif_str_dup(self->path);
        } else {
            self->name = spif_str_substr(self->path, idx, 0);
        }
    }

    self->module_handle = dlopen(SPIF_STR_STR(self->path), RTLD_LAZY | RTLD_GLOBAL);
    if (SPIF_PTR_ISNULL(self->module_handle)) {
        libast_print_error("Unable to dlopen() \"%s\" -- %s\n", SPIF_STR_STR(self->path), dlerror());
        return FALSE;
    }
    fp = spif_module_getsym(self, "init");
    if (fp) {
        return (((fp)(self)) ? (TRUE) : (FALSE));
    } else {
        return TRUE;
    }
}

spif_bool_t
spif_module_run(spif_module_t self)
{
    return ((spif_module_call(self, "run", NULL)) ? (TRUE) : (FALSE));
}

spif_bool_t
spif_module_unload(spif_module_t self)
{
    spif_func_t fp;

    ASSERT_RVAL(!SPIF_MODULE_ISNULL(self), FALSE);
    REQUIRE_RVAL(!SPIF_PTR_ISNULL(self->module_handle), FALSE);

    fp = spif_module_getsym(self, "done");
    if (fp) {
        if (!(fp)()) {
            return FALSE;
        }
    }

    dlerror();
    if (dlclose(self->module_handle)) {
        libast_print_warning("Unable to dlclose() \"%s\" -- %s\n", SPIF_STR_STR(self->path), dlerror());
        return FALSE;
    }
    self->module_handle = (spif_ptr_t) NULL;
    return TRUE;
}

SPIF_DEFINE_PROPERTY_FUNC(module, str, name);
SPIF_DEFINE_PROPERTY_FUNC(module, str, path);
SPIF_DEFINE_PROPERTY_FUNC(module, ptr, module_handle);
SPIF_DEFINE_PROPERTY_FUNC(module, ptr, main_handle);
