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

#ifndef _LIBAST_STR_H_
#define _LIBAST_STR_H_

#define SPIF_STR(obj)                ((spif_str_t) (obj))
#define SPIF_OBJ_IS_STR(obj)         (SPIF_OBJ_IS_TYPE(obj, str))
#define SPIF_STR_ISNULL(s)           SPIF_OBJ_ISNULL(SPIF_OBJ(s))

#define SPIF_STR_NEW(type)           SPIF_STR((SPIF_CLASS(SPIF_CLASS_VAR(type)))->noo())
#define SPIF_STR_INIT(obj)           SPIF_OBJ_INIT(obj)
#define SPIF_STR_DONE(obj)           SPIF_OBJ_DONE(obj)
#define SPIF_STR_DEL(obj)            SPIF_OBJ_DEL(obj)
#define SPIF_STR_SHOW(obj, b, i)     SPIF_OBJ_SHOW(obj, b, i)
#define SPIF_STR_COMP(o1, o2)        SPIF_OBJ_COMP(o1, o2)
#define SPIF_STR_DUP(obj)            SPIF_OBJ_DUP(obj)
#define SPIF_STR_TYPE(obj)           SPIF_OBJ_TYPE(obj)

#define SPIF_STR_STR(obj)            (SPIF_CONST_CAST(charptr) ((SPIF_STR_ISNULL(obj)) \
                                                                ? (SPIF_CAST(charptr) "") \
                                                                : (SPIF_STR(obj)->s)))
typedef size_t spif_stridx_t;

SPIF_DECL_OBJ(str) {
    SPIF_DECL_PARENT_TYPE(obj);
    spif_charptr_t s;
    SPIF_DECL_PROPERTY_C(spif_stridx_t, size);
    SPIF_DECL_PROPERTY_C(spif_stridx_t, len);
};

extern spif_class_t SPIF_CLASS_VAR(str);
extern spif_str_t spif_str_new(void);
extern spif_str_t spif_str_new_from_ptr(spif_charptr_t);
extern spif_str_t spif_str_new_from_buff(spif_charptr_t, spif_stridx_t);
extern spif_str_t spif_str_new_from_fp(FILE *);
extern spif_str_t spif_str_new_from_fd(int);
extern spif_str_t spif_str_new_from_num(long);
extern spif_bool_t spif_str_del(spif_str_t);
extern spif_bool_t spif_str_init(spif_str_t);
extern spif_bool_t spif_str_init_from_ptr(spif_str_t, spif_charptr_t);
extern spif_bool_t spif_str_init_from_buff(spif_str_t, spif_charptr_t, spif_stridx_t);
extern spif_bool_t spif_str_init_from_fp(spif_str_t, FILE *);
extern spif_bool_t spif_str_init_from_fd(spif_str_t, int);
extern spif_bool_t spif_str_init_from_num(spif_str_t, long);
extern spif_bool_t spif_str_done(spif_str_t);
extern spif_str_t spif_str_show(spif_str_t, spif_charptr_t, spif_str_t, size_t);
extern spif_cmp_t spif_str_comp(spif_str_t, spif_str_t);
extern spif_str_t spif_str_dup(spif_str_t);
extern spif_classname_t spif_str_type(spif_str_t);
extern spif_bool_t spif_str_append(spif_str_t, spif_str_t);
extern spif_bool_t spif_str_append_char(spif_str_t, spif_char_t);
extern spif_bool_t spif_str_append_from_ptr(spif_str_t, spif_charptr_t);
extern spif_cmp_t spif_str_casecmp(spif_str_t, spif_str_t);
extern spif_cmp_t spif_str_casecmp_with_ptr(spif_str_t, spif_charptr_t);
extern spif_bool_t spif_str_clear(spif_str_t, spif_char_t);
extern spif_cmp_t spif_str_cmp(spif_str_t, spif_str_t);
extern spif_cmp_t spif_str_cmp_with_ptr(spif_str_t, spif_charptr_t);
extern spif_stridx_t spif_str_find(spif_str_t, spif_str_t);
extern spif_stridx_t spif_str_find_from_ptr(spif_str_t, spif_charptr_t);
extern spif_stridx_t spif_str_index(spif_str_t, spif_char_t);
extern spif_cmp_t spif_str_ncasecmp(spif_str_t, spif_str_t, spif_stridx_t);
extern spif_cmp_t spif_str_ncasecmp_with_ptr(spif_str_t, spif_charptr_t, spif_stridx_t);
extern spif_cmp_t spif_str_ncmp(spif_str_t, spif_str_t, spif_stridx_t);
extern spif_cmp_t spif_str_ncmp_with_ptr(spif_str_t, spif_charptr_t, spif_stridx_t);
extern spif_bool_t spif_str_reverse(spif_str_t);
extern spif_stridx_t spif_str_rindex(spif_str_t, spif_char_t);
extern spif_bool_t spif_str_splice(spif_str_t, spif_stridx_t, spif_stridx_t, spif_str_t);
extern spif_bool_t spif_str_splice_from_ptr(spif_str_t, spif_stridx_t, spif_stridx_t, spif_charptr_t);
extern spif_str_t spif_str_substr(spif_str_t, spif_int32_t, spif_int32_t);
extern spif_charptr_t spif_str_substr_to_ptr(spif_str_t, spif_int32_t, spif_int32_t);
extern double spif_str_to_float(spif_str_t);
extern size_t spif_str_to_num(spif_str_t, int);
extern spif_bool_t spif_str_trim(spif_str_t);
SPIF_DECL_PROPERTY_FUNC_C(str, spif_stridx_t, size);
SPIF_DECL_PROPERTY_FUNC_C(str, spif_stridx_t, len);

#endif /* _LIBAST_STR_H_ */
