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

static const char cvs_ident[] = "$Id$";

#if defined(HAVE_CONFIG_H) && (HAVE_CONFIG_H != 0)
# include <config.h>
#endif

#include <libast_internal.h>

/* *INDENT-OFF* */
SPIF_DECL_OBJ(dlinked_list_iterator) {
    SPIF_DECL_PARENT_TYPE(obj);
    spif_dlinked_list_t subject;
    spif_dlinked_list_item_t current;
};
/* *INDENT-ON* */

static spif_dlinked_list_item_t spif_dlinked_list_item_new(void);
static spif_bool_t spif_dlinked_list_item_init(spif_dlinked_list_item_t);
static spif_bool_t spif_dlinked_list_item_done(spif_dlinked_list_item_t);
static spif_bool_t spif_dlinked_list_item_del(spif_dlinked_list_item_t);
static spif_str_t spif_dlinked_list_item_show(spif_dlinked_list_item_t, spif_charptr_t, spif_str_t, size_t);
static spif_cmp_t spif_dlinked_list_item_comp(spif_dlinked_list_item_t, spif_dlinked_list_item_t);
static spif_dlinked_list_item_t spif_dlinked_list_item_dup(spif_dlinked_list_item_t);
static spif_classname_t spif_dlinked_list_item_type(spif_dlinked_list_item_t);
static spif_obj_t spif_dlinked_list_item_get_data(spif_dlinked_list_item_t);
static spif_bool_t spif_dlinked_list_item_set_data(spif_dlinked_list_item_t, spif_obj_t);

static spif_dlinked_list_t spif_dlinked_list_new(void);
static spif_dlinked_list_t spif_dlinked_list_vector_new(void);
static spif_bool_t spif_dlinked_list_init(spif_dlinked_list_t);
static spif_bool_t spif_dlinked_list_vector_init(spif_dlinked_list_t);
static spif_bool_t spif_dlinked_list_done(spif_dlinked_list_t);
static spif_bool_t spif_dlinked_list_del(spif_dlinked_list_t);
static spif_str_t spif_dlinked_list_show(spif_dlinked_list_t, spif_charptr_t, spif_str_t, size_t);
static spif_cmp_t spif_dlinked_list_comp(spif_dlinked_list_t, spif_dlinked_list_t);
static spif_dlinked_list_t spif_dlinked_list_dup(spif_dlinked_list_t);
static spif_dlinked_list_t spif_dlinked_list_vector_dup(spif_dlinked_list_t);
static spif_classname_t spif_dlinked_list_type(spif_dlinked_list_t);
static spif_bool_t spif_dlinked_list_append(spif_dlinked_list_t, spif_obj_t);
static spif_bool_t spif_dlinked_list_contains(spif_dlinked_list_t, spif_obj_t);
static spif_bool_t spif_dlinked_list_vector_contains(spif_dlinked_list_t, spif_obj_t);
static size_t spif_dlinked_list_count(spif_dlinked_list_t);
static spif_obj_t spif_dlinked_list_find(spif_dlinked_list_t, spif_obj_t);
static spif_obj_t spif_dlinked_list_vector_find(spif_dlinked_list_t, spif_obj_t);
static spif_obj_t spif_dlinked_list_get(spif_dlinked_list_t, size_t);
static size_t spif_dlinked_list_index(spif_dlinked_list_t, spif_obj_t);
static spif_bool_t spif_dlinked_list_insert(spif_dlinked_list_t, spif_obj_t);
static spif_bool_t spif_dlinked_list_insert_at(spif_dlinked_list_t, spif_obj_t, size_t);
static spif_iterator_t spif_dlinked_list_iterator(spif_dlinked_list_t);
static spif_bool_t spif_dlinked_list_prepend(spif_dlinked_list_t, spif_obj_t);
static spif_obj_t spif_dlinked_list_remove(spif_dlinked_list_t, spif_obj_t);
static spif_obj_t spif_dlinked_list_remove_at(spif_dlinked_list_t, size_t);
static spif_bool_t spif_dlinked_list_reverse(spif_dlinked_list_t);
static spif_obj_t *spif_dlinked_list_to_array(spif_dlinked_list_t);
static spif_dlinked_list_iterator_t spif_dlinked_list_iterator_new(spif_dlinked_list_t subject);
static spif_bool_t spif_dlinked_list_iterator_init(spif_dlinked_list_iterator_t self, spif_dlinked_list_t subject);
static spif_bool_t spif_dlinked_list_iterator_done(spif_dlinked_list_iterator_t self);
static spif_bool_t spif_dlinked_list_iterator_del(spif_dlinked_list_iterator_t self);
static spif_str_t spif_dlinked_list_iterator_show(spif_dlinked_list_iterator_t self, spif_charptr_t name, spif_str_t buff, size_t indent);
static spif_cmp_t spif_dlinked_list_iterator_comp(spif_dlinked_list_iterator_t self, spif_dlinked_list_iterator_t other);
static spif_dlinked_list_iterator_t spif_dlinked_list_iterator_dup(spif_dlinked_list_iterator_t self);
static spif_classname_t spif_dlinked_list_iterator_type(spif_dlinked_list_iterator_t self);
static spif_bool_t spif_dlinked_list_iterator_has_next(spif_dlinked_list_iterator_t self);
static spif_obj_t spif_dlinked_list_iterator_next(spif_dlinked_list_iterator_t self);

/* *INDENT-OFF* */
static SPIF_CONST_TYPE(class) dli_class = {
    SPIF_DECL_CLASSNAME(dlinked_list_item),
    (spif_func_t) spif_dlinked_list_item_new,
    (spif_func_t) spif_dlinked_list_item_init,
    (spif_func_t) spif_dlinked_list_item_done,
    (spif_func_t) spif_dlinked_list_item_del,
    (spif_func_t) spif_dlinked_list_item_show,
    (spif_func_t) spif_dlinked_list_item_comp,
    (spif_func_t) spif_dlinked_list_item_dup,
    (spif_func_t) spif_dlinked_list_item_type
};
SPIF_TYPE(class) SPIF_CLASS_VAR(dlinked_list_item) = &dli_class;

static spif_const_listclass_t dl_class = {
    {
        SPIF_DECL_CLASSNAME(dlinked_list),
        (spif_func_t) spif_dlinked_list_new,
        (spif_func_t) spif_dlinked_list_init,
        (spif_func_t) spif_dlinked_list_done,
        (spif_func_t) spif_dlinked_list_del,
        (spif_func_t) spif_dlinked_list_show,
        (spif_func_t) spif_dlinked_list_comp,
        (spif_func_t) spif_dlinked_list_dup,
        (spif_func_t) spif_dlinked_list_type
    },
    (spif_func_t) spif_dlinked_list_append,
    (spif_func_t) spif_dlinked_list_contains,
    (spif_func_t) spif_dlinked_list_count,
    (spif_func_t) spif_dlinked_list_find,
    (spif_func_t) spif_dlinked_list_get,
    (spif_func_t) spif_dlinked_list_index,
    (spif_func_t) spif_dlinked_list_insert,
    (spif_func_t) spif_dlinked_list_insert_at,
    (spif_func_t) spif_dlinked_list_iterator,
    (spif_func_t) spif_dlinked_list_prepend,
    (spif_func_t) spif_dlinked_list_remove,
    (spif_func_t) spif_dlinked_list_remove_at,
    (spif_func_t) spif_dlinked_list_reverse,
    (spif_func_t) spif_dlinked_list_to_array
};
spif_listclass_t SPIF_LISTCLASS_VAR(dlinked_list) = &dl_class;

static spif_const_vectorclass_t dlv_class = {
    {
        SPIF_DECL_CLASSNAME(dlinked_list),
        (spif_func_t) spif_dlinked_list_vector_new,
        (spif_func_t) spif_dlinked_list_vector_init,
        (spif_func_t) spif_dlinked_list_done,
        (spif_func_t) spif_dlinked_list_del,
        (spif_func_t) spif_dlinked_list_show,
        (spif_func_t) spif_dlinked_list_comp,
        (spif_func_t) spif_dlinked_list_vector_dup,
        (spif_func_t) spif_dlinked_list_type
    },
    (spif_func_t) spif_dlinked_list_vector_contains,
    (spif_func_t) spif_dlinked_list_count,
    (spif_func_t) spif_dlinked_list_vector_find,
    (spif_func_t) spif_dlinked_list_insert,
    (spif_func_t) spif_dlinked_list_iterator,
    (spif_func_t) spif_dlinked_list_remove,
    (spif_func_t) spif_dlinked_list_to_array
};
spif_vectorclass_t SPIF_VECTORCLASS_VAR(dlinked_list) = &dlv_class;

static spif_const_iteratorclass_t dlli_class = {
    {
        SPIF_DECL_CLASSNAME(dlinked_list),
        (spif_func_t) spif_dlinked_list_iterator_new,
        (spif_func_t) spif_dlinked_list_iterator_init,
        (spif_func_t) spif_dlinked_list_iterator_done,
        (spif_func_t) spif_dlinked_list_iterator_del,
        (spif_func_t) spif_dlinked_list_iterator_show,
        (spif_func_t) spif_dlinked_list_iterator_comp,
        (spif_func_t) spif_dlinked_list_iterator_dup,
        (spif_func_t) spif_dlinked_list_iterator_type
    },
    (spif_func_t) spif_dlinked_list_iterator_has_next,
    (spif_func_t) spif_dlinked_list_iterator_next
};
spif_iteratorclass_t SPIF_ITERATORCLASS_VAR(dlinked_list) = &dlli_class;
/* *INDENT-ON* */

static spif_dlinked_list_item_t
spif_dlinked_list_item_new(void)
{
    spif_dlinked_list_item_t self;

    self = SPIF_ALLOC(dlinked_list_item);
    spif_dlinked_list_item_init(self);
    return self;
}

static spif_bool_t
spif_dlinked_list_item_init(spif_dlinked_list_item_t self)
{
    self->data = SPIF_NULL_TYPE(obj);
    self->prev = SPIF_NULL_TYPE(dlinked_list_item);
    self->next = SPIF_NULL_TYPE(dlinked_list_item);
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_item_done(spif_dlinked_list_item_t self)
{
    if (self->data != SPIF_NULL_TYPE(obj)) {
        SPIF_OBJ_DEL(self->data);
    }
    self->data = SPIF_NULL_TYPE(obj);
    self->prev = SPIF_NULL_TYPE(dlinked_list_item);
    self->next = SPIF_NULL_TYPE(dlinked_list_item);
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_item_del(spif_dlinked_list_item_t self)
{
    spif_dlinked_list_item_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

static spif_str_t
spif_dlinked_list_item_show(spif_dlinked_list_item_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_dlinked_list_item_t) %s (%9p <- %9p -> %9p):  ",
             name, self->prev, self, self->next);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }
    if (SPIF_DLINKED_LIST_ITEM_ISNULL(self->data)) {
        spif_str_append_from_ptr(buff, SPIF_NULLSTR_TYPE(obj));
    } else {
        buff = SPIF_OBJ_SHOW(self->data, buff, 0);
    }
    return buff;
}

static spif_cmp_t
spif_dlinked_list_item_comp(spif_dlinked_list_item_t self, spif_dlinked_list_item_t other)
{
    return (SPIF_CAST(cmp) SPIF_OBJ_COMP(SPIF_OBJ(self->data), SPIF_OBJ(other->data)));
}

static spif_dlinked_list_item_t
spif_dlinked_list_item_dup(spif_dlinked_list_item_t self)
{
    spif_dlinked_list_item_t tmp;

    tmp = spif_dlinked_list_item_new();
    tmp->data = SPIF_OBJ_DUP(self->data);
    return tmp;
}

static spif_classname_t
spif_dlinked_list_item_type(spif_dlinked_list_item_t self)
{
    USE_VAR(self);
    return SPIF_CLASS_VAR(dlinked_list_item)->classname;
}

static spif_obj_t
spif_dlinked_list_item_get_data(spif_dlinked_list_item_t self)
{
    return SPIF_OBJ(self->data);
}

static spif_bool_t
spif_dlinked_list_item_set_data(spif_dlinked_list_item_t self, spif_obj_t obj)
{
    self->data = obj;
    return TRUE;
}

static spif_dlinked_list_t
spif_dlinked_list_new(void)
{
    spif_dlinked_list_t self;

    self = SPIF_ALLOC(dlinked_list);
    spif_dlinked_list_init(self);
    return self;
}

static spif_dlinked_list_t
spif_dlinked_list_vector_new(void)
{
    spif_dlinked_list_t self;

    self = SPIF_ALLOC(dlinked_list);
    spif_dlinked_list_vector_init(self);
    return self;
}

static spif_bool_t
spif_dlinked_list_init(spif_dlinked_list_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_LISTCLASS_VAR(dlinked_list)));
    self->len = 0;
    self->head = SPIF_NULL_TYPE(dlinked_list_item);
    self->tail = SPIF_NULL_TYPE(dlinked_list_item);
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_vector_init(spif_dlinked_list_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_VECTORCLASS_VAR(dlinked_list)));
    self->len = 0;
    self->head = SPIF_NULL_TYPE(dlinked_list_item);
    self->tail = SPIF_NULL_TYPE(dlinked_list_item);
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_done(spif_dlinked_list_t self)
{
    spif_dlinked_list_item_t current;

    if (self->len) {
        for (current = self->head; current;) {
            spif_dlinked_list_item_t tmp;

            tmp = current;
            current = current->next;
            spif_dlinked_list_item_del(tmp);
        }
        self->len = 0;
        self->head = SPIF_NULL_TYPE(dlinked_list_item);
        self->tail = SPIF_NULL_TYPE(dlinked_list_item);
    }
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_del(spif_dlinked_list_t self)
{
    spif_dlinked_list_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

static spif_str_t
spif_dlinked_list_show(spif_dlinked_list_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];
    spif_dlinked_list_item_t current;
    size_t i;

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_dlinked_list_t) %s:  %010p {\n", name, self);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }

    if (SPIF_DLINKED_LIST_ITEM_ISNULL(self->head)) {
        spif_str_append_from_ptr(buff, SPIF_NULLSTR_TYPE(obj));
    } else {
        for (current = self->head, i = 0; current; current = current->next, i++) {
            sprintf(tmp, "item %d", i);
            buff = spif_dlinked_list_item_show(current, tmp, buff, indent + 2);
        }
    }

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

static spif_cmp_t
spif_dlinked_list_comp(spif_dlinked_list_t self, spif_dlinked_list_t other)
{
    return (SPIF_OBJ_COMP(SPIF_OBJ(self), SPIF_OBJ(other)));
}

static spif_dlinked_list_t
spif_dlinked_list_dup(spif_dlinked_list_t self)
{
    spif_dlinked_list_t tmp;
    spif_dlinked_list_item_t src, dest, prev;

    tmp = spif_dlinked_list_new();
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(dlinked_list));
    tmp->head = spif_dlinked_list_item_dup(self->head);
    for (src = self->head, dest = tmp->head, prev = SPIF_NULL_TYPE(dlinked_list_item);
         src->next;
         src = src->next, prev = dest, dest = dest->next) {
        dest->next = spif_dlinked_list_item_dup(src->next);
        dest->prev = prev;
    }
    dest->next = SPIF_NULL_TYPE(dlinked_list_item);
    tmp->tail = prev;
    return tmp;
}

static spif_dlinked_list_t
spif_dlinked_list_vector_dup(spif_dlinked_list_t self)
{
    spif_dlinked_list_t tmp;
    spif_dlinked_list_item_t src, dest, prev;

    tmp = spif_dlinked_list_vector_new();
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(dlinked_list));
    tmp->head = spif_dlinked_list_item_dup(self->head);
    for (src = self->head, dest = tmp->head, prev = SPIF_NULL_TYPE(dlinked_list_item);
         src->next;
         src = src->next, prev = dest, dest = dest->next) {
        dest->next = spif_dlinked_list_item_dup(src->next);
        dest->prev = prev;
    }
    dest->next = SPIF_NULL_TYPE(dlinked_list_item);
    tmp->tail = prev;
    return tmp;
}

static spif_classname_t
spif_dlinked_list_type(spif_dlinked_list_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}

static spif_bool_t
spif_dlinked_list_append(spif_dlinked_list_t self, spif_obj_t obj)
{
    spif_dlinked_list_item_t item;

    /* Create list member object "item" */
    item = spif_dlinked_list_item_new();
    spif_dlinked_list_item_set_data(item, obj);

    /* Append "item" to the end of the list. */
    if (self->tail) {
        item->prev = self->tail;
        item->prev->next = item;
        self->tail = item;
    } else {
        self->tail = self->head = item;
        item->prev = SPIF_NULL_TYPE(dlinked_list_item);
    }
    item->next = SPIF_NULL_TYPE(dlinked_list_item);
    self->len++;
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_contains(spif_dlinked_list_t self, spif_obj_t obj)
{
    return ((SPIF_LIST_ISNULL(spif_dlinked_list_find(self, obj))) ? (FALSE) : (TRUE));
}

static spif_bool_t
spif_dlinked_list_vector_contains(spif_dlinked_list_t self, spif_obj_t obj)
{
    return ((SPIF_LIST_ISNULL(spif_dlinked_list_vector_find(self, obj))) ? (FALSE) : (TRUE));
}

static size_t
spif_dlinked_list_count(spif_dlinked_list_t self)
{
    return self->len;
}

static spif_obj_t
spif_dlinked_list_find(spif_dlinked_list_t self, spif_obj_t obj)
{
    spif_dlinked_list_item_t current;

    for (current = self->head; current; current = current->next) {
        if (SPIF_CMP_IS_EQUAL(SPIF_OBJ_COMP(current->data, obj))) {
            return current->data;
        }
    }
    return SPIF_NULL_TYPE(obj);
}

static spif_obj_t
spif_dlinked_list_vector_find(spif_dlinked_list_t self, spif_obj_t obj)
{
    spif_dlinked_list_item_t current;

    for (current = self->head; current; current = current->next) {
        spif_cmp_t c;

        c = SPIF_OBJ_COMP(current->data, obj);
        if (SPIF_CMP_IS_EQUAL(c)) {
            return current->data;
        } else if (SPIF_CMP_IS_GREATER(c)) {
            break;
        }
    }
    return SPIF_NULL_TYPE(obj);
}

static spif_obj_t
spif_dlinked_list_get(spif_dlinked_list_t self, size_t idx)
{
    size_t i;
    spif_dlinked_list_item_t current;

    if (idx >= self->len) {
        return SPIF_NULL_TYPE(obj);
    } else if (idx > (self->len / 2)) {
        for (current = self->tail, i = self->len - 1; current && i > idx; i--, current = current->prev);
        return (current ? (current->data) : SPIF_NULL_TYPE(obj));
    } else {
        for (current = self->head, i = 0; current && i < idx; i++, current = current->next);
        return (current ? (current->data) : SPIF_NULL_TYPE(obj));
    }
}

static size_t
spif_dlinked_list_index(spif_dlinked_list_t self, spif_obj_t obj)
{
    size_t i;
    spif_dlinked_list_item_t current;

    for (current = self->head, i = 0; current && !SPIF_CMP_IS_EQUAL(SPIF_OBJ_COMP(current->data, obj)); i++, current = current->next);
    return (current ? i : ((size_t) (-1)));
}

static spif_bool_t
spif_dlinked_list_insert(spif_dlinked_list_t self, spif_obj_t obj)
{
    spif_dlinked_list_item_t item, current;

    item = spif_dlinked_list_item_new();
    spif_dlinked_list_item_set_data(item, obj);

    if (SPIF_DLINKED_LIST_ITEM_ISNULL(self->head)) {
        self->head = self->tail = item;
    } else if (SPIF_CMP_IS_LESS(spif_dlinked_list_item_comp(item, self->head))) {
        item->next = self->head;
        self->head->prev = item;
        self->head = item;
    } else if (SPIF_CMP_IS_GREATER(spif_dlinked_list_item_comp(item, self->tail))) {
        item->prev = self->tail;
        self->tail->next = item;
        self->tail = item;
    } else {
        for (current = self->head;
             current->next && SPIF_CMP_IS_GREATER(spif_dlinked_list_item_comp(item, current->next));
             current = current->next);
        item->next = current->next;
        item->prev = current;
        current->next->prev = item;
        current->next = item;
    }
    self->len++;
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_insert_at(spif_dlinked_list_t self, spif_obj_t obj, size_t idx)
{
    size_t i;
    spif_dlinked_list_item_t item, current;

    if (idx == 0 || SPIF_DLINKED_LIST_ITEM_ISNULL(self->head)) {
        return spif_dlinked_list_prepend(self, obj);
    } else if (idx == (self->len - 1) || SPIF_DLINKED_LIST_ITEM_ISNULL(self->tail)) {
        return spif_dlinked_list_append(self, obj);
    } else if (idx > (self->len / 2)) {
        for (current = self->tail, i = self->len - 1; current->prev && i > idx; i--, current = current->prev);
        if (i != idx) {
            return FALSE;
        }
    } else {
        for (current = self->head, i = 1; current->next && i < idx; i++, current = current->next);
        if (i != idx) {
            return FALSE;
        }
    }
    item = spif_dlinked_list_item_new();
    spif_dlinked_list_item_set_data(item, obj);

    item->next = current->next;
    item->prev = current;
    current->next->prev = item;
    current->next = item;
    self->len++;
    return TRUE;
}

static spif_iterator_t
spif_dlinked_list_iterator(spif_dlinked_list_t self)
{
    return SPIF_CAST(iterator) spif_dlinked_list_iterator_new(self);
}

static spif_bool_t
spif_dlinked_list_prepend(spif_dlinked_list_t self, spif_obj_t obj)
{
    spif_dlinked_list_item_t item, current;

    /* Create list member object "item" */
    item = spif_dlinked_list_item_new();
    spif_dlinked_list_item_set_data(item, obj);

    /* Set "item" to be at the front of the list. */
    if (self->head) {
        current = self->head;
        self->head = item;
        current->prev = item;
        item->next = current;
    } else {
        self->head = self->tail = item;
    }
    self->len++;
    return TRUE;
}

static spif_obj_t
spif_dlinked_list_remove(spif_dlinked_list_t self, spif_obj_t item)
{
    spif_dlinked_list_item_t current, tmp;

    if (SPIF_DLINKED_LIST_ITEM_ISNULL(self->head)) {
        return SPIF_NULL_TYPE(obj);
    }

    for (current = self->head; current && !SPIF_CMP_IS_EQUAL(SPIF_OBJ_COMP(item, current->data)); current = current->next);
    if (SPIF_DLINKED_LIST_ITEM_ISNULL(current)) {
        return SPIF_NULL_TYPE(obj);
    }
    tmp = current;
    if (!SPIF_DLINKED_LIST_ITEM_ISNULL(tmp->prev)) {
        tmp->prev->next = tmp->next;
    }
    if (!SPIF_DLINKED_LIST_ITEM_ISNULL(tmp->next)) {
        tmp->next->prev = tmp->prev;
    }
    if (tmp == self->head) {
        self->head = tmp->next;
    }
    if (tmp == self->tail) {
        self->tail = tmp->prev;
    }
    item = tmp->data;
    tmp->data = SPIF_NULL_TYPE(obj);
    spif_dlinked_list_item_del(tmp);

    self->len--;
    return item;
}

static spif_obj_t
spif_dlinked_list_remove_at(spif_dlinked_list_t self, size_t idx)
{
    size_t i;
    spif_dlinked_list_item_t current;
    spif_obj_t tmp;

    if (SPIF_DLINKED_LIST_ITEM_ISNULL(self->head)) {
        return SPIF_NULL_TYPE(obj);
    }

    if (idx > (self->len / 2)) {
        for (current = self->tail, i = self->len - 1; current && i > idx; i--, current = current->prev);
    } else {
        for (current = self->head, i = 0; current && i < idx; i++, current = current->next);
    }

    if (SPIF_DLINKED_LIST_ITEM_ISNULL(current)) {
        return SPIF_NULL_TYPE(obj);
    }
    if (!SPIF_DLINKED_LIST_ITEM_ISNULL(current->prev)) {
        current->prev->next = current->next;
    }
    if (!SPIF_DLINKED_LIST_ITEM_ISNULL(current->next)) {
        current->next->prev = current->prev;
    }
    if (current == self->head) {
        self->head = current->next;
    }
    if (current == self->tail) {
        self->tail = current->prev;
    }

    tmp = spif_dlinked_list_item_get_data(current);
    spif_dlinked_list_item_set_data(current, SPIF_NULL_TYPE(obj));
    spif_dlinked_list_item_del(current);

    self->len--;
    return tmp;
}

static spif_bool_t
spif_dlinked_list_reverse(spif_dlinked_list_t self)
{
    spif_dlinked_list_item_t current, tmp;

    for (current = self->head; current; ) {
        tmp = current;
        current = current->next;
        BINSWAP(tmp->prev, tmp->next);
    }
    self->head = tmp;
    return TRUE;
}

static spif_obj_t *
spif_dlinked_list_to_array(spif_dlinked_list_t self)
{
    spif_obj_t *tmp;
    spif_dlinked_list_item_t current;
    size_t i;

    tmp = SPIF_CAST_C(spif_obj_t *) MALLOC(SPIF_SIZEOF_TYPE(obj) * self->len);
    for (i = 0, current = self->head; i < self->len; current = current->next, i++) {
        tmp[i] = SPIF_CAST(obj) SPIF_OBJ(spif_dlinked_list_item_get_data(current));
    }
    return tmp;
}

static spif_dlinked_list_iterator_t
spif_dlinked_list_iterator_new(spif_dlinked_list_t subject)
{
    spif_dlinked_list_iterator_t self;

    self = SPIF_ALLOC(dlinked_list_iterator);
    spif_dlinked_list_iterator_init(self, subject);
    return self;
}

static spif_bool_t
spif_dlinked_list_iterator_init(spif_dlinked_list_iterator_t self, spif_dlinked_list_t subject)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_ITERATORCLASS_VAR(dlinked_list)));
    self->subject = subject;
    if (SPIF_LIST_ISNULL(self->subject)) {
        self->current = SPIF_NULL_TYPE(dlinked_list_item);
    } else {
        self->current = self->subject->head;
    }
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_iterator_done(spif_dlinked_list_iterator_t self)
{
    self->subject = SPIF_NULL_TYPE(dlinked_list);
    self->current = SPIF_NULL_TYPE(dlinked_list_item);
    return TRUE;
}

static spif_bool_t
spif_dlinked_list_iterator_del(spif_dlinked_list_iterator_t self)
{
    spif_dlinked_list_iterator_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

static spif_str_t
spif_dlinked_list_iterator_show(spif_dlinked_list_iterator_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];

    if (SPIF_ITERATOR_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(iterator, name, buff, indent, tmp);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_dlinked_list_iterator_t) %s:  %010p {\n", name, self);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }

    buff = spif_dlinked_list_show(self->subject, "subject", buff, indent + 2);
    buff = spif_dlinked_list_item_show(self->current, "current", buff, indent + 2);

    snprintf(tmp + indent, sizeof(tmp) - indent, "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

static spif_cmp_t
spif_dlinked_list_iterator_comp(spif_dlinked_list_iterator_t self, spif_dlinked_list_iterator_t other)
{
    return spif_dlinked_list_comp(self->subject, other->subject);
}

static spif_dlinked_list_iterator_t
spif_dlinked_list_iterator_dup(spif_dlinked_list_iterator_t self)
{
    spif_dlinked_list_iterator_t tmp;

    tmp = spif_dlinked_list_iterator_new(self->subject);
    tmp->current = self->current;
    return tmp;
}

static spif_classname_t
spif_dlinked_list_iterator_type(spif_dlinked_list_iterator_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}

static spif_bool_t
spif_dlinked_list_iterator_has_next(spif_dlinked_list_iterator_t self)
{
    spif_dlinked_list_t subject;

    ASSERT_RVAL(!SPIF_ITERATOR_ISNULL(self), FALSE);
    subject = self->subject;
    REQUIRE_RVAL(!SPIF_LIST_ISNULL(subject), FALSE);
    if (self->current) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static spif_obj_t
spif_dlinked_list_iterator_next(spif_dlinked_list_iterator_t self)
{
    spif_obj_t tmp;

    ASSERT_RVAL(!SPIF_ITERATOR_ISNULL(self), SPIF_NULL_TYPE(obj));
    REQUIRE_RVAL(!SPIF_LIST_ISNULL(self->subject), SPIF_NULL_TYPE(obj));
    REQUIRE_RVAL(!SPIF_DLINKED_LIST_ITEM_ISNULL(self->current), SPIF_NULL_TYPE(obj));
    tmp = self->current->data;
    self->current = self->current->next;
    return tmp;
}
