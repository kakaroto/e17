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
#include <pthread.h>

/* *INDENT-OFF* */
static SPIF_CONST_TYPE(threadclass) pt_class = {
    {
        SPIF_DECL_CLASSNAME(pthreads),
        (spif_func_t) spif_pthreads_new,
        (spif_func_t) spif_pthreads_init,
        (spif_func_t) spif_pthreads_done,
        (spif_func_t) spif_pthreads_del,
        (spif_func_t) spif_pthreads_show,
        (spif_func_t) spif_pthreads_comp,
        (spif_func_t) spif_pthreads_dup,
        (spif_func_t) spif_pthreads_type
    },
    (spif_func_t) spif_pthreads_new_with_func,
    (spif_func_t) spif_pthreads_init_with_func,
    (spif_func_t) spif_pthreads_detach,
    (spif_func_t) spif_pthreads_get_condition,
    (spif_func_t) spif_pthreads_get_mutex,
    (spif_func_t) spif_pthreads_kill,
    (spif_func_t) spif_pthreads_run,
    (spif_func_t) spif_pthreads_tls_calloc,
    (spif_func_t) spif_pthreads_tls_free,
    (spif_func_t) spif_pthreads_tls_get,
    (spif_func_t) spif_pthreads_tls_malloc,
    (spif_func_t) spif_pthreads_tls_realloc,
    (spif_func_t) spif_pthreads_wait,
    (spif_func_t) spif_pthreads_wait_for
};
SPIF_TYPE(class) SPIF_CLASS_VAR(pthreads) = (spif_class_t) &pt_class;
SPIF_TYPE(threadclass) SPIF_THREADCLASS_VAR(pthreads) = &pt_class;

static SPIF_CONST_TYPE(mutexclass) ptm_class = {
    {
        SPIF_DECL_CLASSNAME(pthreads_mutex),
        (spif_func_t) spif_pthreads_mutex_new,
        (spif_func_t) spif_pthreads_mutex_init,
        (spif_func_t) spif_pthreads_mutex_done,
        (spif_func_t) spif_pthreads_mutex_del,
        (spif_func_t) spif_pthreads_mutex_show,
        (spif_func_t) spif_pthreads_mutex_comp,
        (spif_func_t) spif_pthreads_mutex_dup,
        (spif_func_t) spif_pthreads_mutex_type
    },
    (spif_func_t) spif_pthreads_mutex_lock,
    (spif_func_t) spif_pthreads_mutex_lock_nowait,
    (spif_func_t) spif_pthreads_mutex_unlock
};
SPIF_TYPE(class) SPIF_CLASS_VAR(pthreads_mutex) = (spif_class_t) &ptm_class;
SPIF_TYPE(mutexclass) SPIF_MUTEXCLASS_VAR(pthreads_mutex) = &ptm_class;

static SPIF_CONST_TYPE(conditionclass) ptc_class = {
    {
        SPIF_DECL_CLASSNAME(pthreads_condition),
        (spif_func_t) spif_pthreads_condition_new,
        (spif_func_t) spif_pthreads_condition_init,
        (spif_func_t) spif_pthreads_condition_done,
        (spif_func_t) spif_pthreads_condition_del,
        (spif_func_t) spif_pthreads_condition_show,
        (spif_func_t) spif_pthreads_condition_comp,
        (spif_func_t) spif_pthreads_condition_dup,
        (spif_func_t) spif_pthreads_condition_type
    },
    (spif_func_t) spif_pthreads_condition_broadcast,
    (spif_func_t) spif_pthreads_condition_signal,
    (spif_func_t) spif_pthreads_condition_wait,
    (spif_func_t) spif_pthreads_condition_wait_timed
};
SPIF_TYPE(class) SPIF_CLASS_VAR(pthreads_condition) = (spif_class_t) &ptc_class;
SPIF_TYPE(conditionclass) SPIF_CONDITIONCLASS_VAR(pthreads_condition) = &ptc_class;
/* *INDENT-ON* */

static void spif_pthreads_tls_destructor(void *ptr);

spif_pthreads_t
spif_pthreads_new(void)
{
    spif_pthreads_t self;

    self = SPIF_ALLOC(pthreads);
    if (!spif_pthreads_init(self)) {
        SPIF_DEALLOC(self);
        self = (spif_pthreads_t) NULL;
    }
    return self;
}

spif_pthreads_t
spif_pthreads_new_with_func(spif_thread_func_t func, spif_thread_data_t data)
{
    spif_pthreads_t self;

    self = SPIF_ALLOC(pthreads);
    if (!spif_pthreads_init_with_func(self, func, data)) {
        SPIF_DEALLOC(self);
        self = (spif_pthreads_t) NULL;
    }
    return self;
}

spif_bool_t
spif_pthreads_init(spif_pthreads_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_THREADCLASS_VAR(pthreads)));
    self->handle = (pthread_t) 0;
    self->creator = (pthread_t) 0;
    pthread_attr_init(&self->attr);
    self->main_func = (spif_thread_func_t) NULL;
    self->data = (spif_thread_data_t) NULL;
    self->tls_keys = (spif_list_t) NULL;
    return TRUE;
}

spif_bool_t
spif_pthreads_init_with_func(spif_pthreads_t self, spif_thread_func_t func, spif_thread_data_t data)
{
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_THREADCLASS_VAR(pthreads)));
    self->handle = (pthread_t) 0;
    self->creator = (pthread_t) 0;
    pthread_attr_init(&self->attr);
    self->main_func = func;
    self->data = data;
    self->tls_keys = (spif_list_t) NULL;
    return TRUE;
}

spif_bool_t
spif_pthreads_done(spif_pthreads_t self)
{
    spif_bool_t ret = TRUE;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    if (self->handle) {
        spif_pthreads_kill(self, SIGTERM);
        self->handle = (pthread_t) 0;
    }
    self->creator = pthread_self();
    pthread_attr_destroy(&self->attr);
    pthread_attr_init(&self->attr);
    self->main_func = (spif_thread_func_t) NULL;
    if (self->tls_keys) {
        SPIF_LIST_DEL(self->tls_keys);
        self->tls_keys = (spif_list_t) NULL;
    }
    return ret;
}

spif_bool_t
spif_pthreads_del(spif_pthreads_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    spif_pthreads_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_str_t
spif_pthreads_show(spif_pthreads_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    spif_char_t tmp[4096];

    if (SPIF_PTHREADS_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(pthreads, name, buff, indent, tmp);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf((char *) tmp + indent, sizeof(tmp) - indent,
             "(spif_pthreads_t) %s:  %10p { \"",
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
    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(pthread_t) handle:  %ld\n", (long) self->handle);
    spif_str_append_from_ptr(buff, tmp);

    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(pthread_t) creator:  %ld\n", (long) self->creator);
    spif_str_append_from_ptr(buff, tmp);

    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(pthread_attr_t) attr:  %10p {...}\n", &self->attr);
    spif_str_append_from_ptr(buff, tmp);

    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(spif_thread_func_t) main_func:  %10p\n", self->main_func);
    spif_str_append_from_ptr(buff, tmp);

    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(spif_thread_data_t) data:  %10p\n", self->data);
    spif_str_append_from_ptr(buff, tmp);

    if (SPIF_LIST_ISNULL(self->tls_keys)) {
        SPIF_OBJ_SHOW_NULL(list, "tls_keys", buff, indent, tmp);
    } else {
        buff = SPIF_LIST_SHOW(self->tls_keys, buff, indent);
    }

    snprintf((char *) tmp, sizeof(tmp), "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

spif_cmp_t
spif_pthreads_comp(spif_pthreads_t self, spif_pthreads_t other)
{
    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    if (pthread_equal(self->handle, other->handle)) {
        return SPIF_CMP_EQUAL;
    }
    return SPIF_OBJ_COMP(self, other);
}

spif_pthreads_t
spif_pthreads_dup(spif_pthreads_t self)
{
    spif_pthreads_t tmp;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), (spif_pthreads_t) NULL);
    tmp = SPIF_ALLOC(pthreads);
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(pthreads));
    tmp->tls_keys = SPIF_LIST_DUP(self->tls_keys);
    return tmp;
}

spif_classname_t
spif_pthreads_type(spif_pthreads_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), (spif_classname_t) SPIF_NULLSTR_TYPE(classname));
    return SPIF_OBJ_CLASSNAME(self);
}

spif_bool_t
spif_pthreads_detach(spif_pthreads_t self)
{
    int ret;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    REQUIRE_RVAL(self->handle, FALSE);

    ret = pthread_detach(self->handle);
    if (!ret) {
        return TRUE;
    } else if ((errno == EINVAL) || (ret == EINVAL)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

spif_condition_t
spif_pthreads_get_condition(spif_pthreads_t self)
{
    spif_pthreads_condition_t cond;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), (spif_condition_t) NULL);
    cond = spif_pthreads_condition_new();
    spif_pthreads_mutex_set_creator(SPIF_PTHREADS_MUTEX(cond), SPIF_THREAD(self));
    return (spif_condition_t) cond;
}

spif_mutex_t
spif_pthreads_get_mutex(spif_pthreads_t self)
{
    spif_pthreads_mutex_t mutex;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), (spif_mutex_t) NULL);
    mutex = spif_pthreads_mutex_new();
    spif_pthreads_mutex_set_creator(mutex, SPIF_THREAD(self));
    return (spif_mutex_t) mutex;
}

spif_bool_t
spif_pthreads_kill(spif_pthreads_t self, int sig)
{
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    REQUIRE_RVAL(self->handle, FALSE);

    return ((pthread_kill(self->handle, sig)) ? (FALSE) : (TRUE));
}

spif_bool_t
spif_pthreads_run(spif_pthreads_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    REQUIRE_RVAL(self->main_func != (spif_thread_func_t) NULL, FALSE);
    REQUIRE_RVAL(self->handle == (pthread_t) 0, FALSE);

    if (!pthread_create(&self->handle, &self->attr, self->main_func, self)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

spif_tls_handle_t
spif_pthreads_tls_calloc(spif_pthreads_t self, size_t count, size_t size)
{
    spif_tls_handle_t handle;
    spif_ptr_t data;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), (spif_tls_handle_t) -1);
    REQUIRE_RVAL(count, (spif_tls_handle_t) -1);
    REQUIRE_RVAL(size, (spif_tls_handle_t) -1);

    handle = spif_pthreads_tls_malloc(self, count * size);
    data = spif_pthreads_tls_get(self, handle);
    MEMSET(data, 0, count * size);
    return handle;
}

spif_bool_t
spif_pthreads_tls_free(spif_pthreads_t self, spif_tls_handle_t handle)
{
    spif_mbuff_t key;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    REQUIRE_RVAL(!SPIF_LIST_ISNULL(self->tls_keys), FALSE);

    key = (spif_mbuff_t) SPIF_LIST_REMOVE_AT(self->tls_keys, handle);
    if (handle != SPIF_LIST_COUNT(self->tls_keys)) {
        /* If this isn't the last item in our list, insert an empty placeholder
           so that numeric indexes which have already been given out will remain valid. */
        SPIF_LIST_INSERT_AT(self->tls_keys, (spif_obj_t) NULL, handle);
    }
    if (!SPIF_MBUFF_ISNULL(key)) {
        spif_ptr_t ptr;

        ptr = (spif_ptr_t) pthread_getspecific(*((pthread_key_t *) SPIF_MBUFF_BUFF(key)));
        if (ptr && !pthread_key_delete(*((pthread_key_t *) SPIF_MBUFF_BUFF(key)))) {
            FREE(ptr);
            return TRUE;
        }
    }
    return FALSE;
}

spif_ptr_t
spif_pthreads_tls_get(spif_pthreads_t self, spif_tls_handle_t handle)
{
    spif_mbuff_t key;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), (spif_ptr_t) NULL);
    REQUIRE_RVAL(!SPIF_LIST_ISNULL(self->tls_keys), FALSE);

    key = (spif_mbuff_t) SPIF_LIST_GET(self->tls_keys, handle);
    if (!SPIF_MBUFF_ISNULL(key)) {
        return ((spif_ptr_t) pthread_getspecific(*((pthread_key_t *) SPIF_MBUFF_BUFF(key))));
    }
    return (spif_ptr_t) NULL;
}

spif_tls_handle_t
spif_pthreads_tls_malloc(spif_pthreads_t self, spif_memidx_t size)
{
    pthread_key_t key;
    spif_ptr_t ptr;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), (spif_tls_handle_t) -1);

    if (SPIF_LIST_ISNULL(self->tls_keys)) {
        self->tls_keys = SPIF_LIST_NEW(array);
    }
    ptr = MALLOC(size);

    if (!pthread_key_create(&key, spif_pthreads_tls_destructor)) {
        spif_mbuff_t tls_key;

        /* Store the pointer. */
        pthread_setspecific(key, ptr);

        /* The key is actually copied into the mbuff buffer. */
        tls_key = spif_mbuff_new_from_ptr((spif_byteptr_t) &key, sizeof(pthread_key_t));

        /* Since the pointer returned by MALLOC() is only stored locally in
           this function and as keyed in the TLS data store, it cannot be
           retrieved by any other thread.  Thus, thread-local storage.  Yay! */
        if (SPIF_LIST_APPEND(self->tls_keys, tls_key)) {
            return (SPIF_LIST_COUNT(self->tls_keys) - 1);
        }
    }
    return ((spif_tls_handle_t) -1);
}

static void
spif_pthreads_tls_destructor(void *ptr)
{
    FREE(ptr);
}

spif_bool_t
spif_pthreads_tls_realloc(spif_pthreads_t self, spif_tls_handle_t handle, spif_memidx_t size)
{
    spif_mbuff_t key;

    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    REQUIRE_RVAL(!SPIF_LIST_ISNULL(self->tls_keys), FALSE);

    key = (spif_mbuff_t) SPIF_LIST_GET(self->tls_keys, handle);
    if (!SPIF_MBUFF_ISNULL(key)) {
        spif_ptr_t ptr;

        ptr = (spif_ptr_t) pthread_getspecific(*((pthread_key_t *) SPIF_MBUFF_BUFF(key)));
        if (ptr) {
            ptr = REALLOC(ptr, size);
            if (ptr) {
                pthread_setspecific(*((pthread_key_t *) SPIF_MBUFF_BUFF(key)), ptr);
                return TRUE;
            }
        }
    }
    return FALSE;
}

spif_bool_t
spif_pthreads_wait(spif_pthreads_t self, spif_condition_t cond)
{
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    ASSERT_RVAL(!SPIF_PTHREADS_CONDITION_ISNULL(SPIF_PTHREADS_CONDITION(cond)), FALSE);
}

spif_bool_t
spif_pthreads_wait_for(spif_pthreads_t self, spif_pthreads_t other)
{
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(self), FALSE);
    ASSERT_RVAL(!SPIF_PTHREADS_ISNULL(other), FALSE);
}

SPIF_DEFINE_PROPERTY_FUNC_C(pthreads, pthread_t, handle);
SPIF_DEFINE_PROPERTY_FUNC_C(pthreads, pthread_t, creator);
SPIF_DEFINE_PROPERTY_FUNC_C(pthreads, pthread_attr_t, attr);
SPIF_DEFINE_PROPERTY_FUNC(pthreads, thread_func, main_func);
SPIF_DEFINE_PROPERTY_FUNC(pthreads, thread_data, data);
SPIF_DEFINE_PROPERTY_FUNC(pthreads, list, tls_keys);

spif_pthreads_mutex_t
spif_pthreads_mutex_new(void)
{
    spif_pthreads_mutex_t self;

    self = SPIF_ALLOC(pthreads_mutex);
    if (!spif_pthreads_mutex_init(self)) {
        SPIF_DEALLOC(self);
        self = (spif_pthreads_mutex_t) NULL;
    }
    return self;
}

spif_bool_t
spif_pthreads_mutex_init(spif_pthreads_mutex_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_MUTEX_ISNULL(self), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_MUTEXCLASS_VAR(pthreads_mutex)));
    self->creator = (spif_thread_t) NULL;
    pthread_mutex_init(&self->mutex, NULL);
    return TRUE;
}

spif_bool_t
spif_pthreads_mutex_done(spif_pthreads_mutex_t self)
{
    spif_bool_t ret = TRUE;

    ASSERT_RVAL(!SPIF_PTHREADS_MUTEX_ISNULL(self), FALSE);
    self->creator = (spif_thread_t) NULL;
    if (!pthread_mutex_unlock(&self->mutex)) {
        /* It was locked.  Destroy it. */
        pthread_mutex_destroy(&self->mutex);
    }
    pthread_mutex_init(&self->mutex, NULL);
    return ret;
}

spif_bool_t
spif_pthreads_mutex_del(spif_pthreads_mutex_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_MUTEX_ISNULL(self), FALSE);
    spif_pthreads_mutex_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_str_t
spif_pthreads_mutex_show(spif_pthreads_mutex_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    spif_char_t tmp[4096];

    if (SPIF_PTHREADS_MUTEX_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(pthreads_mutex, name, buff, indent, tmp);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf((char *) tmp + indent, sizeof(tmp) - indent,
             "(spif_pthreads_mutex_t) %s:  %10p { \"",
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
    if (SPIF_THREAD_ISNULL(self->creator)) {
        SPIF_OBJ_SHOW_NULL(thread, "creator", buff, indent, tmp);
    } else {
        buff = SPIF_THREAD_SHOW(self->creator, buff, indent);
    }

    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(pthread_mutex_t) mutex:  %10p {...}\n", &self->mutex);
    spif_str_append_from_ptr(buff, tmp);

    snprintf((char *) tmp, sizeof(tmp), "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

spif_cmp_t
spif_pthreads_mutex_comp(spif_pthreads_mutex_t self, spif_pthreads_mutex_t other)
{
    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    return SPIF_OBJ_COMP(self, other);
}

spif_pthreads_mutex_t
spif_pthreads_mutex_dup(spif_pthreads_mutex_t self)
{
    spif_pthreads_mutex_t tmp;

    ASSERT_RVAL(!SPIF_PTHREADS_MUTEX_ISNULL(self), (spif_pthreads_mutex_t) NULL);
    tmp = SPIF_ALLOC(pthreads_mutex);
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(pthreads_mutex));
    return tmp;
}

spif_classname_t
spif_pthreads_mutex_type(spif_pthreads_mutex_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_MUTEX_ISNULL(self), (spif_classname_t) SPIF_NULLSTR_TYPE(classname));
    return SPIF_OBJ_CLASSNAME(self);
}

spif_bool_t
spif_pthreads_mutex_lock(spif_pthreads_mutex_t self)
{

}

spif_bool_t
spif_pthreads_mutex_lock_nowait(spif_pthreads_mutex_t self)
{

}

spif_bool_t
spif_pthreads_mutex_unlock(spif_pthreads_mutex_t self)
{

}

SPIF_DEFINE_PROPERTY_FUNC(pthreads_mutex, thread, creator);
SPIF_DEFINE_PROPERTY_FUNC_C(pthreads_mutex, pthread_mutex_t, mutex);

spif_pthreads_condition_t
spif_pthreads_condition_new(void)
{
    spif_pthreads_condition_t self;

    self = SPIF_ALLOC(pthreads_condition);
    if (!spif_pthreads_condition_init(self)) {
        SPIF_DEALLOC(self);
        self = (spif_pthreads_condition_t) NULL;
    }
    return self;
}

spif_bool_t
spif_pthreads_condition_init(spif_pthreads_condition_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_CONDITION_ISNULL(self), FALSE);
    spif_pthreads_mutex_init(SPIF_PTHREADS_MUTEX(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS(SPIF_CONDITIONCLASS_VAR(pthreads_condition)));
    pthread_cond_init(&self->cond, NULL);
    return TRUE;
}

spif_bool_t
spif_pthreads_condition_done(spif_pthreads_condition_t self)
{
    spif_bool_t ret = TRUE;

    ASSERT_RVAL(!SPIF_PTHREADS_CONDITION_ISNULL(self), FALSE);
    ret = spif_pthreads_mutex_done(SPIF_PTHREADS_MUTEX(self));
    pthread_cond_destroy(&self->cond);
    pthread_cond_init(&self->cond, NULL);
    return ret;
}

spif_bool_t
spif_pthreads_condition_del(spif_pthreads_condition_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_CONDITION_ISNULL(self), FALSE);
    spif_pthreads_condition_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_str_t
spif_pthreads_condition_show(spif_pthreads_condition_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    spif_char_t tmp[4096];

    if (SPIF_PTHREADS_CONDITION_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(pthreads_condition, name, buff, indent, tmp);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf((char *) tmp + indent, sizeof(tmp) - indent,
             "(spif_pthreads_condition_t) %s:  %10p { \"",
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
    buff = spif_pthreads_mutex_show(SPIF_PTHREADS_MUTEX(self), "self", buff, indent);

    snprintf((char *) tmp + indent, sizeof(tmp) - indent, "(pthread_cond_t) cond:  %10p {...}\n", &self->cond);
    spif_str_append_from_ptr(buff, tmp);

    snprintf((char *) tmp, sizeof(tmp), "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

spif_cmp_t
spif_pthreads_condition_comp(spif_pthreads_condition_t self, spif_pthreads_condition_t other)
{
    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    return SPIF_OBJ_COMP(self, other);
}

spif_pthreads_condition_t
spif_pthreads_condition_dup(spif_pthreads_condition_t self)
{
    spif_pthreads_condition_t tmp;

    ASSERT_RVAL(!SPIF_PTHREADS_CONDITION_ISNULL(self), (spif_pthreads_condition_t) NULL);
    tmp = SPIF_ALLOC(pthreads_condition);
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(pthreads_condition));
    return tmp;
}

spif_classname_t
spif_pthreads_condition_type(spif_pthreads_condition_t self)
{
    ASSERT_RVAL(!SPIF_PTHREADS_CONDITION_ISNULL(self), (spif_classname_t) SPIF_NULLSTR_TYPE(classname));
    return SPIF_OBJ_CLASSNAME(self);
}

spif_bool_t
spif_pthreads_condition_broadcast(spif_pthreads_condition_t self)
{

}

spif_bool_t
spif_pthreads_condition_signal(spif_pthreads_condition_t self)
{

}

spif_bool_t
spif_pthreads_condition_wait(spif_pthreads_condition_t self)
{

}

spif_bool_t
spif_pthreads_condition_wait_timed(spif_pthreads_condition_t self, spif_int32_t delay)
{

}

SPIF_DEFINE_PROPERTY_FUNC_C(pthreads_condition, pthread_cond_t, cond);
