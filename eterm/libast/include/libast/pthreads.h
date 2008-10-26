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

#ifndef _LIBAST_PTHREADS_H_
#define _LIBAST_PTHREADS_H_

#define SPIF_PTHREADS(obj)                    ((spif_pthreads_t) (obj))
#define SPIF_OBJ_IS_PTHREADS(o)               (SPIF_OBJ_IS_TYPE(o, pthreads))
#define SPIF_PTHREADS_ISNULL(s)               SPIF_OBJ_ISNULL(SPIF_OBJ(s))

SPIF_DECL_OBJ(pthreads) {
    SPIF_DECL_PARENT_TYPE(obj);
    SPIF_DECL_PROPERTY_C(pthread_t, handle);
    SPIF_DECL_PROPERTY_C(pthread_t, creator);
    SPIF_DECL_PROPERTY_C(pthread_attr_t, attr);
    SPIF_DECL_PROPERTY(thread_func, main_func);
    SPIF_DECL_PROPERTY(thread_data, data);
    SPIF_DECL_PROPERTY(list, tls_keys);
};

#define SPIF_PTHREADS_MUTEX(obj)              ((spif_pthreads_mutex_t) (obj))
#define SPIF_OBJ_IS_PTHREADS_MUTEX(o)         (SPIF_OBJ_IS_TYPE(o, pthreads_mutex))
#define SPIF_PTHREADS_MUTEX_ISNULL(s)         SPIF_OBJ_ISNULL(SPIF_OBJ(s))

SPIF_DECL_OBJ(pthreads_mutex) {
    SPIF_DECL_PARENT_TYPE(obj);
    SPIF_DECL_PROPERTY(thread, creator);
    SPIF_DECL_PROPERTY_C(pthread_mutex_t, mutex);
};

#define SPIF_PTHREADS_CONDITION(obj)          ((spif_pthreads_condition_t) (obj))
#define SPIF_OBJ_IS_PTHREADS_CONDITION(o)     (SPIF_OBJ_IS_TYPE(o, pthreads_condition))
#define SPIF_PTHREADS_CONDITION_ISNULL(s)     SPIF_OBJ_ISNULL(SPIF_OBJ(s))

SPIF_DECL_OBJ(pthreads_condition) {
    SPIF_DECL_PARENT_TYPE(pthreads_mutex);
    SPIF_DECL_PROPERTY_C(pthread_cond_t, cond);
};

extern SPIF_TYPE(class) SPIF_CLASS_VAR(pthreads);
extern SPIF_TYPE(threadclass) SPIF_THREADCLASS_VAR(pthreads);
extern spif_pthreads_t spif_pthreads_new(void);
extern spif_pthreads_t spif_pthreads_new_with_func(spif_thread_func_t, spif_thread_data_t);
extern spif_bool_t spif_pthreads_init(spif_pthreads_t);
extern spif_bool_t spif_pthreads_init_with_func(spif_pthreads_t, spif_thread_func_t, spif_thread_data_t);
extern spif_bool_t spif_pthreads_done(spif_pthreads_t);
extern spif_bool_t spif_pthreads_del(spif_pthreads_t);
extern spif_str_t spif_pthreads_show(spif_pthreads_t, spif_charptr_t, spif_str_t, size_t);
extern spif_cmp_t spif_pthreads_comp(spif_pthreads_t, spif_pthreads_t);
extern spif_pthreads_t spif_pthreads_dup(spif_pthreads_t);
extern spif_classname_t spif_pthreads_type(spif_pthreads_t);
extern spif_bool_t spif_pthreads_detach(spif_pthreads_t self);
extern spif_condition_t spif_pthreads_get_condition(spif_pthreads_t self);
extern spif_mutex_t spif_pthreads_get_mutex(spif_pthreads_t self);
extern spif_bool_t spif_pthreads_kill(spif_pthreads_t self, int sig);
extern spif_bool_t spif_pthreads_run(spif_pthreads_t self);
extern spif_tls_handle_t spif_pthreads_tls_calloc(spif_pthreads_t self, size_t count, size_t size);
extern spif_bool_t spif_pthreads_tls_free(spif_pthreads_t self, spif_tls_handle_t handle);
extern spif_ptr_t spif_pthreads_tls_get(spif_pthreads_t self, spif_tls_handle_t handle);
extern spif_tls_handle_t spif_pthreads_tls_malloc(spif_pthreads_t self, spif_memidx_t size);
extern spif_bool_t spif_pthreads_tls_realloc(spif_pthreads_t self, spif_tls_handle_t handle, spif_memidx_t size);
extern spif_bool_t spif_pthreads_wait(spif_pthreads_t self, spif_condition_t cond);
extern spif_bool_t spif_pthreads_wait_for(spif_pthreads_t self, spif_pthreads_t other);
SPIF_DECL_PROPERTY_FUNC_C(pthreads, pthread_t, handle);
SPIF_DECL_PROPERTY_FUNC_C(pthreads, pthread_t, creator);
SPIF_DECL_PROPERTY_FUNC_C(pthreads, pthread_attr_t, attr);
SPIF_DECL_PROPERTY_FUNC(pthreads, thread_func, main_func);
SPIF_DECL_PROPERTY_FUNC(pthreads, thread_data, data);
SPIF_DECL_PROPERTY_FUNC(pthreads, list, tls_keys);

extern SPIF_TYPE(mutexclass) SPIF_MUTEXCLASS_VAR(pthreads_mutex);
extern spif_pthreads_mutex_t spif_pthreads_mutex_new(void);
extern spif_bool_t spif_pthreads_mutex_init(spif_pthreads_mutex_t);
extern spif_bool_t spif_pthreads_mutex_done(spif_pthreads_mutex_t);
extern spif_bool_t spif_pthreads_mutex_del(spif_pthreads_mutex_t);
extern spif_str_t spif_pthreads_mutex_show(spif_pthreads_mutex_t, spif_charptr_t, spif_str_t, size_t);
extern spif_cmp_t spif_pthreads_mutex_comp(spif_pthreads_mutex_t, spif_pthreads_mutex_t);
extern spif_pthreads_mutex_t spif_pthreads_mutex_dup(spif_pthreads_mutex_t);
extern spif_classname_t spif_pthreads_mutex_type(spif_pthreads_mutex_t);
extern spif_bool_t spif_pthreads_mutex_lock(spif_pthreads_mutex_t);
extern spif_bool_t spif_pthreads_mutex_lock_nowait(spif_pthreads_mutex_t);
extern spif_bool_t spif_pthreads_mutex_unlock(spif_pthreads_mutex_t);
SPIF_DECL_PROPERTY_FUNC(pthreads_mutex, thread, creator);
SPIF_DECL_PROPERTY_FUNC_C(pthreads_mutex, pthread_mutex_t, mutex);

extern SPIF_TYPE(conditionclass) SPIF_CONDITIONCLASS_VAR(pthreads_condition);
extern spif_pthreads_condition_t spif_pthreads_condition_new(void);
extern spif_bool_t spif_pthreads_condition_init(spif_pthreads_condition_t);
extern spif_bool_t spif_pthreads_condition_done(spif_pthreads_condition_t);
extern spif_bool_t spif_pthreads_condition_del(spif_pthreads_condition_t);
extern spif_str_t spif_pthreads_condition_show(spif_pthreads_condition_t, spif_charptr_t, spif_str_t, size_t);
extern spif_cmp_t spif_pthreads_condition_comp(spif_pthreads_condition_t, spif_pthreads_condition_t);
extern spif_pthreads_condition_t spif_pthreads_condition_dup(spif_pthreads_condition_t);
extern spif_classname_t spif_pthreads_condition_type(spif_pthreads_condition_t);
extern spif_bool_t spif_pthreads_condition_broadcast(spif_pthreads_condition_t);
extern spif_bool_t spif_pthreads_condition_signal(spif_pthreads_condition_t);
extern spif_bool_t spif_pthreads_condition_wait(spif_pthreads_condition_t);
extern spif_bool_t spif_pthreads_condition_wait_timed(spif_pthreads_condition_t, spif_int32_t);
SPIF_DECL_PROPERTY_FUNC_C(pthreads_condition, pthread_cond_t, cond);

#endif /* _LIBAST_PTHREADS_H_ */
