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

static spif_array_t spif_array_new(void);
static spif_bool_t spif_array_init(spif_array_t);
static spif_bool_t spif_array_done(spif_array_t);
static spif_bool_t spif_array_del(spif_array_t);
static spif_str_t spif_array_show(spif_array_t, spif_charptr_t, spif_str_t, size_t);
static spif_cmp_t spif_array_comp(spif_array_t, spif_array_t);
static spif_array_t spif_array_dup(spif_array_t);
static spif_classname_t spif_array_type(spif_array_t);
static spif_bool_t spif_array_append(spif_array_t, spif_obj_t);
static spif_bool_t spif_array_contains(spif_array_t, spif_obj_t);
static size_t spif_array_count(spif_array_t);
static spif_obj_t spif_array_get(spif_array_t, size_t);
static size_t spif_array_index(spif_array_t, spif_obj_t);
static spif_bool_t spif_array_insert(spif_array_t, spif_obj_t);
static spif_bool_t spif_array_insert_at(spif_array_t, spif_obj_t, size_t);
static spif_bool_t spif_array_iterator(spif_array_t);
static spif_obj_t spif_array_next(spif_array_t);
static spif_bool_t spif_array_prepend(spif_array_t, spif_obj_t);
static spif_obj_t spif_array_remove(spif_array_t, spif_obj_t);
static spif_obj_t spif_array_remove_at(spif_array_t, size_t);
static spif_bool_t spif_array_reverse(spif_array_t);
static spif_obj_t *spif_array_to_array(spif_array_t);

/* *INDENT-OFF* */
static spif_const_listclass_t a_class = {
    {
        SPIF_DECL_CLASSNAME(array),
        (spif_func_t) spif_array_new,
        (spif_func_t) spif_array_init,
        (spif_func_t) spif_array_done,
        (spif_func_t) spif_array_del,
        (spif_func_t) spif_array_show,
        (spif_func_t) spif_array_comp,
        (spif_func_t) spif_array_dup,
        (spif_func_t) spif_array_type
    },
    (spif_func_t) spif_array_append,
    (spif_func_t) spif_array_contains,
    (spif_func_t) spif_array_count,
    (spif_func_t) spif_array_get,
    (spif_func_t) spif_array_index,
    (spif_func_t) spif_array_insert,
    (spif_func_t) spif_array_insert_at,
    (spif_func_t) spif_array_iterator,
    (spif_func_t) spif_array_next,
    (spif_func_t) spif_array_prepend,
    (spif_func_t) spif_array_remove,
    (spif_func_t) spif_array_remove_at,
    (spif_func_t) spif_array_reverse,
    (spif_func_t) spif_array_to_array
};
spif_listclass_t SPIF_CLASS_VAR(array) = &a_class;
/* *INDENT-ON* */

static spif_array_t
spif_array_new(void)
{
    spif_array_t self;

    self = SPIF_ALLOC(array);
    spif_array_init(self);
    return self;
}

static spif_bool_t
spif_array_init(spif_array_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_CLASS_VAR(array)));
    self->len = 0;
    self->items = SPIF_NULL_CTYPE(spif_obj_t *);
    return TRUE;
}

static spif_bool_t
spif_array_done(spif_array_t self)
{
    size_t i;

    for (i = 0; i < self->len; i++) {
        if (!SPIF_OBJ_ISNULL(self->items[i])) {
            SPIF_OBJ_DEL(self->items[i]);
        }
    }
    self->len = 0;
    FREE(self->items);
    return TRUE;
}

static spif_bool_t
spif_array_del(spif_array_t self)
{
    spif_array_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

static spif_str_t
spif_array_show(spif_array_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];
    size_t i;

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_array_t) %s:  {\n", name);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }

    if (SPIF_ARRAY_ISNULL(self->items)) {
        spif_str_append_from_ptr(buff, SPIF_NULLSTR_CTYPE(spif_obj_t *));
    } else {
        for (i = 0; i < self->len; i++) {
            spif_obj_t o = self->items[i];
            sprintf(tmp, "item %d", i);
            buff = SPIF_OBJ_CALL_METHOD(o, show)(o, tmp, buff, indent + 2);
        }
    }

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

static spif_cmp_t
spif_array_comp(spif_array_t self, spif_array_t other)
{
    size_t i;

    for (i = 0; i < self->len; i++) {
        if (!SPIF_CMP_IS_EQUAL(SPIF_OBJ_COMP(self->items[i], other->items[i]))) {
            return FALSE;
        }
    }
    return TRUE;
}

static spif_array_t
spif_array_dup(spif_array_t self)
{
    spif_array_t tmp;
    size_t i;

    tmp = spif_array_new();
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(array));
    tmp->items = SPIF_CAST_C(spif_obj_t *) MALLOC(SPIF_SIZEOF_TYPE(obj) * self->len);
    for (i = 0; i < self->len; i++) {
        tmp->items[i] = SPIF_CAST(obj) SPIF_OBJ_DUP(SPIF_OBJ(self->items[i]));
    }
    return tmp;
}

static spif_classname_t
spif_array_type(spif_array_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}

static spif_bool_t
spif_array_append(spif_array_t self, spif_obj_t obj)
{
    self->len++;
    if (self->items) {
        self->items = SPIF_CAST_C(spif_obj_t *) REALLOC(self->items, SPIF_SIZEOF_TYPE(obj) * self->len);
    } else {
        self->items = SPIF_CAST_C(spif_obj_t *) MALLOC(SPIF_SIZEOF_TYPE(obj) * self->len);
    }
    self->items[self->len - 1] = obj;
    return TRUE;
}

static spif_bool_t
spif_array_contains(spif_array_t self, spif_obj_t obj)
{
    size_t i;

    for (i = 0; i < self->len; i++) {
        if (SPIF_CMP_IS_EQUAL(SPIF_OBJ_COMP(self->items[i], obj))) {
            return TRUE;
        }
    }
    return FALSE;
}

static size_t
spif_array_count(spif_array_t self)
{
    return self->len;
}

static spif_obj_t
spif_array_get(spif_array_t self, size_t idx)
{
    return ((idx < self->len) ? (self->items[idx]) : (SPIF_NULL_TYPE(obj)));
}

static size_t
spif_array_index(spif_array_t self, spif_obj_t obj)
{
    size_t i;

    for (i = 0; i < self->len; i++) {
        if (SPIF_CMP_IS_EQUAL(SPIF_OBJ_COMP(self->items[i], obj))) {
            return i;
        }
    }
    return SPIF_CAST_C(size_t) (-1);
}

static spif_bool_t
spif_array_insert(spif_array_t self, spif_obj_t obj)
{
    size_t i, left;

    if (self->items) {
        self->items = SPIF_CAST_C(spif_obj_t *) REALLOC(self->items, SPIF_SIZEOF_TYPE(obj) * (self->len + 1));
    } else {
        self->items = SPIF_CAST_C(spif_obj_t *) MALLOC(SPIF_SIZEOF_TYPE(obj) * (self->len + 1));
    }

    for (i = 0; i < self->len && SPIF_CMP_IS_GREATER(SPIF_OBJ_COMP(obj, self->items[i])); i++);
    left = self->len - i;
    if (left) {
        memmove(self->items + i + 1, self->items + i, SPIF_SIZEOF_TYPE(obj) * left);
    }
    self->items[i] = obj;
    self->len++;
    return TRUE;
}

static spif_bool_t
spif_array_insert_at(spif_array_t self, spif_obj_t obj, size_t idx)
{
    size_t left;

    if (self->items) {
        self->items = SPIF_CAST_C(spif_obj_t *) REALLOC(self->items, SPIF_SIZEOF_TYPE(obj) * (self->len + 1));
    } else {
        self->items = SPIF_CAST_C(spif_obj_t *) MALLOC(SPIF_SIZEOF_TYPE(obj) * (self->len + 1));
    }

    left = self->len - idx;
    if (left) {
        memmove(self->items + idx + 1, self->items + idx, SPIF_SIZEOF_TYPE(obj) * left);
    }
    self->items[idx] = obj;
    self->len++;
    return TRUE;
}

static spif_bool_t
spif_array_iterator(spif_array_t self)
{
    USE_VAR(self);
    return FALSE;
}

static spif_obj_t
spif_array_next(spif_array_t self)
{
    USE_VAR(self);
    return FALSE;
}

static spif_bool_t
spif_array_prepend(spif_array_t self, spif_obj_t obj)
{
    if (self->items) {
        self->items = SPIF_CAST_C(spif_obj_t *) REALLOC(self->items, SPIF_SIZEOF_TYPE(obj) * (self->len + 1));
    } else {
        self->items = SPIF_CAST_C(spif_obj_t *) MALLOC(SPIF_SIZEOF_TYPE(obj) * (self->len + 1));
    }

    memmove(self->items + 1, self->items, SPIF_SIZEOF_TYPE(obj) * self->len);
    self->items[0] = obj;
    self->len++;
    return TRUE;
}

static spif_obj_t
spif_array_remove(spif_array_t self, spif_obj_t item)
{
    spif_obj_t tmp;
    size_t i, left;

    for (i = 0; i < self->len && !SPIF_CMP_IS_EQUAL(SPIF_OBJ_COMP(item, self->items[i])); i++);
    if (i == self->len) {
        return SPIF_NULL_TYPE(obj);
    }

    left = self->len - i - 1;
    tmp = self->items[i];
    memmove(self->items + i, self->items + i + 1, SPIF_SIZEOF_TYPE(obj) * left);
    self->items = SPIF_CAST_C(spif_obj_t *) REALLOC(self->items, SPIF_SIZEOF_TYPE(obj) * (--(self->len)));

    return tmp;
}

static spif_obj_t
spif_array_remove_at(spif_array_t self, size_t idx)
{
    spif_obj_t tmp;
    size_t left;

    if (idx >= self->len) {
        return SPIF_NULL_TYPE(obj);
    }

    left = self->len - idx - 1;
    tmp = self->items[idx];
    memmove(self->items + idx, self->items + idx + 1, SPIF_SIZEOF_TYPE(obj) * left);
    self->items = SPIF_CAST_C(spif_obj_t *) REALLOC(self->items, SPIF_SIZEOF_TYPE(obj) * (--(self->len)));

    return tmp;
}

static spif_bool_t
spif_array_reverse(spif_array_t self)
{

}

static spif_obj_t *
spif_array_to_array(spif_array_t self)
{

}
