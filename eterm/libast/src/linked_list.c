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

static spif_linked_list_item_t spif_linked_list_item_new(void);
static spif_bool_t spif_linked_list_item_init(spif_linked_list_item_t);
static spif_bool_t spif_linked_list_item_done(spif_linked_list_item_t);
static spif_bool_t spif_linked_list_item_del(spif_linked_list_item_t);
static spif_bool_t spif_linked_list_item_show(spif_linked_list_item_t);
static spif_obj_t spif_linked_list_item_get_data(spif_linked_list_item_t);
static spif_bool_t spif_linked_list_item_set_data(spif_linked_list_item_t, spif_obj_t);

static spif_linked_list_t spif_linked_list_new(void);
static spif_bool_t spif_linked_list_init(spif_linked_list_t);
static spif_bool_t spif_linked_list_done(spif_linked_list_t);
static spif_bool_t spif_linked_list_del(spif_linked_list_t);
static spif_bool_t spif_linked_list_show(spif_linked_list_t);
static spif_bool_t spif_linked_list_append(spif_linked_list_t, spif_obj_t);
static spif_bool_t spif_linked_list_contains(spif_linked_list_t, spif_obj_t);
static size_t spif_linked_list_count(spif_linked_list_t);
static spif_obj_t spif_linked_list_get(spif_linked_list_t, size_t);
static size_t spif_linked_list_index(spif_linked_list_t, spif_obj_t);
static spif_bool_t spif_linked_list_insert(spif_linked_list_t, spif_obj_t);
static spif_bool_t spif_linked_list_insert_at(spif_linked_list_t, spif_obj_t, size_t);
static spif_bool_t spif_linked_list_iterator(spif_linked_list_t);
static spif_obj_t spif_linked_list_next(spif_linked_list_t);
static spif_bool_t spif_linked_list_prepend(spif_linked_list_t, spif_obj_t);
static spif_obj_t spif_linked_list_remove(spif_linked_list_t);
static spif_obj_t spif_linked_list_remove_at(spif_linked_list_t, size_t);

SPIF_DECL_IFCLASS(list, linked_list) = {
    {
SPIF_DECL_CLASSNAME(linked_list),
            (spif_newfunc_t) spif_linked_list_new,
            (spif_memberfunc_t) spif_linked_list_init,
            (spif_memberfunc_t) spif_linked_list_done,
            (spif_memberfunc_t) spif_linked_list_del,
            (spif_func_t) spif_linked_list_show,
            (spif_func_t) spif_linked_list_comp,
            (spif_func_t) spif_linked_list_dup,
            (spif_func_t) spif_linked_list_type},
        (spif_memberfunc_t) spif_linked_list_append,
        (spif_memberfunc_t) spif_linked_list_contains,
        (spif_memberfunc_t) spif_linked_list_count,
        (spif_memberfunc_t) spif_linked_list_get,
        (spif_memberfunc_t) spif_linked_list_index,
        (spif_memberfunc_t) spif_linked_list_insert,
        (spif_memberfunc_t) spif_linked_list_insert_at,
        (spif_memberfunc_t) spif_linked_list_iterator,
        (spif_memberfunc_t) spif_linked_list_next,
        (spif_memberfunc_t) spif_linked_list_prepend,
        (spif_memberfunc_t) spif_linked_list_remove, (spif_memberfunc_t) spif_linked_list_remove_at};

static spif_linked_list_item_t
spif_linked_list_item_new(void)
{
    spif_linked_list_item_t self;

    self = SPIF_ALLOC(linked_list_item);
    spif_linked_list_item_init(self);
    return self;
}

static spif_bool_t
spif_linked_list_item_init(spif_linked_list_item_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(linked_list_item));
    self->data = SPIF_NULL_TYPE(obj);
    self->next = SPIF_NULL_TYPE(linked_list_item);
    return TRUE;
}

static spif_bool_t
spif_linked_list_item_done(spif_linked_list_item_t self)
{
    /* FIXME:  Should we destroy the data objects? */
    self->data = SPIF_NULL_TYPE(obj);
    self->next = SPIF_NULL_TYPE(linked_list_item);
    return TRUE;
}

static spif_bool_t
spif_linked_list_item_del(spif_linked_list_item_t self)
{
    spif_linked_list_item_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

static spif_bool_t
spif_linked_list_item_show(spif_linked_list_item_t self)
{
    USE_VAR(self);
    return TRUE;
}

static spif_obj_t
spif_linked_list_item_get_data(spif_linked_list_item_t self)
{
    return (self->data);
}

static spif_bool_t
spif_linked_list_item_set_data(spif_linked_list_item_t self, spif_obj_t obj)
{
    self->data = obj;
    return TRUE;
}

static spif_linked_list_t
spif_linked_list_new(void)
{
    spif_linked_list_t self;

    self = SPIF_ALLOC(linked_list);
    spif_linked_list_init(self);
    return self;
}

static spif_bool_t
spif_linked_list_init(spif_linked_list_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(linked_list));
    self->len = 0;
    self->head = SPIF_NULL_TYPE(linked_list_item);
    return TRUE;
}

static spif_bool_t
spif_linked_list_done(spif_linked_list_t self)
{
    spif_linked_list_item_t current;

    if (self->len) {
        for (current = self->head; current;) {
            spif_linked_list_item_t tmp;

            tmp = current;
            current = current->next;
            spif_linked_list_item_del(tmp);
        }
        self->len = 0;
        self->head = SPIF_NULL_TYPE(linked_list_item);
    }
    return TRUE;
}

static spif_bool_t
spif_linked_list_del(spif_linked_list_t self)
{
    spif_linked_list_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

static spif_bool_t
spif_linked_list_show(spif_linked_list_t self)
{
    USE_VAR(self);
    return TRUE;
}

static spif_bool_t
spif_linked_list_append(spif_linked_list_t self, spif_obj_t obj)
{
    spif_linked_list_item_t item, current;

    /* Create list member object "item" */
    item = spif_linked_list_item_new();
    spif_linked_list_item_set_data(item, obj);

    /* Append "item" to the end of the list. */
    if (self->head) {
        for (current = self->head; current->next; current = current->next);
        current->next = item;
    } else {
        self->head = item;
    }
    item->next = SPIF_NULL_TYPE(linked_list_item);
    self->len++;
    return TRUE;
}

static spif_bool_t
spif_linked_list_contains(spif_linked_list_t self, spif_obj_t obj)
{
    spif_linked_list_item_t current;

    for (current = self->head; current; current = current->next) {
        if (spif_linked_list_item_get_data(current) == obj) {
            return TRUE;
        }
    }
    return FALSE;
}

static size_t
spif_linked_list_count(spif_linked_list_t self)
{
    return self->len;
}

static spif_obj_t
spif_linked_list_get(spif_linked_list_t self, size_t idx)
{
    size_t i;
    spif_linked_list_item_t current;

    for (current = self->head, i = 0; current && i < idx; i++, current = current->next);
    return (current ? (current->data) : SPIF_NULL_TYPE(obj));
}

static size_t
spif_linked_list_index(spif_linked_list_t self, spif_obj_t obj)
{
    size_t i;
    spif_linked_list_item_t current;

    for (current = self->head, i = 0; current && (current->data != obj); i++, current = current->next);
    return (current ? i : ((size_t) (-1)));
}

static spif_bool_t
spif_linked_list_insert(spif_linked_list_t self, spif_obj_t obj)
{

}

static spif_bool_t
spif_linked_list_insert_at(spif_linked_list_t self, spif_obj_t obj, size_t idx)
{

}

static spif_bool_t
spif_linked_list_iterator(spif_linked_list_t self)
{

}

static spif_obj_t
spif_linked_list_next(spif_linked_list_t self)
{

}

static spif_bool_t
spif_linked_list_prepend(spif_linked_list_t self, spif_obj_t obj)
{

}

static spif_obj_t
spif_linked_list_remove(spif_linked_list_t self)
{

}

static spif_obj_t
spif_linked_list_remove_at(spif_linked_list_t self, size_t idx)
{

}
