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

#ifndef _LIBAST_USTR_H_
#define _LIBAST_USTR_H_

#define SPIF_UTF8_CHAR_LEN(s)         (((((spif_uchar_t) (*(s))) & 0x80) == 0x00) \
                                       ? (1) \
                                       : (((((spif_uchar_t) (*(s))) & 0xc0) == 0x80) \
                                          ? (2) \
                                          : (((((spif_uchar_t) (*(s))) & 0xe0) == 0xc0) \
                                             ? (3) \
                                             : (((((spif_uchar_t) (*(s))) & 0xf0) == 0xe0) \
                                                ? (4) \
                                                : (((((spif_uchar_t) (*(s))) & 0xf8) == 0xf0) \
                                                   ? (5) \
                                                   : (((((spif_uchar_t) (*(s))) & 0xfc) == 0xf8) \
                                                      ? (6) \
                                                      : (0)))))))

#define SPIF_USTR(obj)                ((spif_ustr_t) (obj))
#define SPIF_OBJ_IS_USTR(obj)         (SPIF_OBJ_IS_TYPE(obj, ustr))
#define SPIF_USTR_ISNULL(s)           SPIF_OBJ_ISNULL(SPIF_OBJ(s))

#define SPIF_USTR_NEW(type)           SPIF_USTR((SPIF_CLASS(SPIF_CLASS_VAR(type)))->noo())
#define SPIF_USTR_INIT(obj)           SPIF_OBJ_INIT(obj)
#define SPIF_USTR_DONE(obj)           SPIF_OBJ_DONE(obj)
#define SPIF_USTR_DEL(obj)            SPIF_OBJ_DEL(obj)
#define SPIF_USTR_SHOW(obj, b, i)     SPIF_OBJ_SHOW(obj, b, i)
#define SPIF_USTR_COMP(o1, o2)        SPIF_OBJ_COMP(o1, o2)
#define SPIF_USTR_DUP(obj)            SPIF_OBJ_DUP(obj)
#define SPIF_USTR_TYPE(obj)           SPIF_OBJ_TYPE(obj)

#define SPIF_USTR_NEW_FROM_PTR(type, p)          SPIF_USTR((SPIF_CLASS(SPIF_CLASS_VAR(type)))->new_from_ptr)(p))
#define SPIF_USTR_NEW_FROM_BUFF(type, b, s)      SPIF_USTR((SPIF_CLASS(SPIF_CLASS_VAR(type)))->new_from_buff)(b, s))
#define SPIF_USTR_NEW_FROM_FP(type, fp)          SPIF_USTR((SPIF_CLASS(SPIF_CLASS_VAR(type)))->new_from_fp)(fp))
#define SPIF_USTR_NEW_FROM_FD(type, fd)          SPIF_USTR((SPIF_CLASS(SPIF_CLASS_VAR(type)))->new_from_fd)(fd))
#define SPIF_USTR_NEW_FROM_NUM(type, num)        SPIF_USTR((SPIF_CLASS(SPIF_CLASS_VAR(type)))->new_from_num)(num))
#define SPIF_USTR_INIT_FROM_PTR(o, p)            (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), init_from_ptr)(o, p))
#define SPIF_USTR_INIT_FROM_BUFF(o, b, s)        (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), init_from_buff)(o, b, s))
#define SPIF_USTR_INIT_FROM_FP(o, fp)            (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), init_from_fp)(o, fp))
#define SPIF_USTR_INIT_FROM_FD(o, fd)            (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), init_from_fd)(o, fd))
#define SPIF_USTR_INIT_FROM_NUM(o, num)          (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), init_from_num)(o, num))
#define SPIF_USTR_APPEND(o, x)                   (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), append)(o, x))
#define SPIF_USTR_APPEND_CHAR(o, x)              (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), append_char)(o, x))
#define SPIF_USTR_APPEND_FROM_PTR(o, x)          (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), append_from_ptr)(o, x))
#define SPIF_USTR_CASECMP(o, x)                  (spif_cmp_t) (SPIF_OBJ_CALL_METHOD((o), casecmp)(o, x))
#define SPIF_USTR_CASECMP_WITH_PTR(o, x)         (spif_cmp_t) (SPIF_OBJ_CALL_METHOD((o), casecmp_with_ptr)(o, x))
#define SPIF_USTR_CLEAR(o, x)                    (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), clear)(o, x))
#define SPIF_USTR_CMP(o, x)                      (spif_cmp_t) (SPIF_OBJ_CALL_METHOD((o), cmp)(o, x))
#define SPIF_USTR_CMP_WITH_PTR(o, x)             (spif_cmp_t) (SPIF_OBJ_CALL_METHOD((o), cmp_with_ptr)(o, x))
#define SPIF_USTR_DOWNCASE(o)                    (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), downcase)(o))
#define SPIF_USTR_FIND(o, x)                     (spif_stridx_t) (SPIF_OBJ_CALL_METHOD((o), find)(o, x))
#define SPIF_USTR_FIND_FROM_PTR(o, x)            (spif_stridx_t) (SPIF_OBJ_CALL_METHOD((o), find_from_ptr)(o, x))
#define SPIF_USTR_INDEX(o, x)                    (spif_stridx_t) (SPIF_OBJ_CALL_METHOD((o), index)(o, x))
#define SPIF_USTR_NCASECMP(o, x, n)              (spif_cmp_t) (SPIF_OBJ_CALL_METHOD((o), ncasecmp)(o, x, n))
#define SPIF_USTR_NCASECMP_WITH_PTR(o, x, n)     (spif_cmp_t) (SPIF_OBJ_CALL_METHOD((o), ncasecmp_with_ptr)(o, x, n))
#define SPIF_USTR_NCMP(o, x, n)                  (spif_cmp_t) (SPIF_OBJ_CALL_METHOD((o), ncmp)(o, x, n))
#define SPIF_USTR_NCMP_WITH_PTR(o, x, n)         (spif_cmp_t) (SPIF_OBJ_CALL_METHOD((o), ncmp_with_ptr)(o, x, n))
#define SPIF_USTR_PREPEND(o, x)                  (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), prepend)(o, x))
#define SPIF_USTR_PREPEND_CHAR(o, x)             (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), prepend_char)(o, x))
#define SPIF_USTR_PREPEND_FROM_PTR(o, x)         (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), prepend_from_ptr)(o, x))
#define SPIF_USTR_REVERSE(o)                     (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), reverse)(o))
#define SPIF_USTR_RINDEX(o, x)                   (spif_stridx_t) (SPIF_OBJ_CALL_METHOD((o), rindex)(o, x))
#define SPIF_USTR_SPLICE(o, n1, n2, x)           (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), splice)(o, n1, n2, x))
#define SPIF_USTR_SPLICE_FROM_PTR(o, n1, n2, x)  (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), splice_from_ptr)(o, n1, n2, x))
#define SPIF_USTR_SPRINTF(x)                     (spif_bool_t) (SPIF_STR_CALL_METHOD((o), sprintf) x)
#define SPIF_USTR_SUBSTR(o, n1, n2)              (spif_ustr_t) (SPIF_OBJ_CALL_METHOD((o), substr)(o, n1, n2))
#define SPIF_USTR_SUBSTR_TO_PTR(o, n1, n2)       (spif_charptr_t) (SPIF_OBJ_CALL_METHOD((o), substr_to_ptr)(o, n1, n2))
#define SPIF_USTR_TO_FLOAT(o)                    (double) (SPIF_OBJ_CALL_METHOD((o), to_float)(o))
#define SPIF_USTR_TO_NUM(o, x)                   (size_t) (SPIF_OBJ_CALL_METHOD((o), to_num)(o, x))
#define SPIF_USTR_TRIM(o)                        (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), trim)(o))
#define SPIF_USTR_UPCASE(o)                      (spif_bool_t) (SPIF_OBJ_CALL_METHOD((o), upcase)(o))

#define SPIF_USTR_STR(obj)  ((const spif_charptr_t) ((SPIF_USTR_ISNULL(obj)) \
                                                       ? ((spif_charptr_t) "") \
                                                       : (SPIF_USTR(obj)->s)))

typedef spif_int64_t spif_ustridx_t;

SPIF_DECL_OBJ(ustr) {
    SPIF_DECL_PARENT_TYPE(obj);
    spif_charptr_t s;
    SPIF_DECL_PROPERTY_C(spif_ustridx_t, size);
    SPIF_DECL_PROPERTY_C(spif_ustridx_t, len);
};

extern spif_class_t SPIF_CLASS_VAR(ustr);
extern spif_strclass_t SPIF_STRCLASS_VAR(ustr);
extern spif_ustr_t spif_ustr_new(void);
extern spif_ustr_t spif_ustr_new_from_ptr(spif_charptr_t);
extern spif_ustr_t spif_ustr_new_from_buff(spif_charptr_t, spif_ustridx_t);
extern spif_ustr_t spif_ustr_new_from_fp(FILE *);
extern spif_ustr_t spif_ustr_new_from_fd(int);
extern spif_ustr_t spif_ustr_new_from_num(long);
extern spif_bool_t spif_ustr_del(spif_ustr_t);
extern spif_bool_t spif_ustr_init(spif_ustr_t);
extern spif_bool_t spif_ustr_init_from_ptr(spif_ustr_t, spif_charptr_t);
extern spif_bool_t spif_ustr_init_from_buff(spif_ustr_t, spif_charptr_t, spif_ustridx_t);
extern spif_bool_t spif_ustr_init_from_fp(spif_ustr_t, FILE *);
extern spif_bool_t spif_ustr_init_from_fd(spif_ustr_t, int);
extern spif_bool_t spif_ustr_init_from_num(spif_ustr_t, long);
extern spif_bool_t spif_ustr_done(spif_ustr_t);
extern spif_str_t spif_ustr_show(spif_ustr_t, spif_charptr_t, spif_str_t, size_t);
extern spif_cmp_t spif_ustr_comp(spif_ustr_t, spif_ustr_t);
extern spif_ustr_t spif_ustr_dup(spif_ustr_t);
extern spif_classname_t spif_ustr_type(spif_ustr_t);

extern spif_bool_t spif_ustr_append(spif_ustr_t, spif_ustr_t);
extern spif_bool_t spif_ustr_append_char(spif_ustr_t, spif_char_t);
extern spif_bool_t spif_ustr_append_from_ptr(spif_ustr_t, spif_charptr_t);
extern spif_cmp_t spif_ustr_casecmp(spif_ustr_t, spif_ustr_t);
extern spif_cmp_t spif_ustr_casecmp_with_ptr(spif_ustr_t, spif_charptr_t);
extern spif_bool_t spif_ustr_clear(spif_ustr_t, spif_char_t);
extern spif_cmp_t spif_ustr_cmp(spif_ustr_t, spif_ustr_t);
extern spif_cmp_t spif_ustr_cmp_with_ptr(spif_ustr_t, spif_charptr_t);
extern spif_bool_t spif_ustr_downcase(spif_ustr_t);
extern spif_ustridx_t spif_ustr_find(spif_ustr_t, spif_ustr_t);
extern spif_ustridx_t spif_ustr_find_from_ptr(spif_ustr_t, spif_charptr_t);
extern spif_ustridx_t spif_ustr_index(spif_ustr_t, spif_char_t);
extern spif_cmp_t spif_ustr_ncasecmp(spif_ustr_t, spif_ustr_t, spif_ustridx_t);
extern spif_cmp_t spif_ustr_ncasecmp_with_ptr(spif_ustr_t, spif_charptr_t, spif_ustridx_t);
extern spif_cmp_t spif_ustr_ncmp(spif_ustr_t, spif_ustr_t, spif_ustridx_t);
extern spif_cmp_t spif_ustr_ncmp_with_ptr(spif_ustr_t, spif_charptr_t, spif_ustridx_t);
extern spif_bool_t spif_ustr_prepend(spif_ustr_t, spif_ustr_t);
extern spif_bool_t spif_ustr_prepend_char(spif_ustr_t, spif_char_t);
extern spif_bool_t spif_ustr_prepend_from_ptr(spif_ustr_t, spif_charptr_t);
extern spif_bool_t spif_ustr_reverse(spif_ustr_t);
extern spif_ustridx_t spif_ustr_rindex(spif_ustr_t, spif_char_t);
extern spif_bool_t spif_ustr_splice(spif_ustr_t, spif_ustridx_t, spif_ustridx_t, spif_ustr_t);
extern spif_bool_t spif_ustr_splice_from_ptr(spif_ustr_t, spif_ustridx_t, spif_ustridx_t, spif_charptr_t);
extern spif_bool_t spif_ustr_sprintf(spif_ustr_t, spif_charptr_t, ...);
extern spif_ustr_t spif_ustr_substr(spif_ustr_t, spif_ustridx_t, spif_ustridx_t);
extern spif_charptr_t spif_ustr_substr_to_ptr(spif_ustr_t, spif_ustridx_t, spif_ustridx_t);
extern double spif_ustr_to_float(spif_ustr_t);
extern size_t spif_ustr_to_num(spif_ustr_t, int);
extern spif_bool_t spif_ustr_trim(spif_ustr_t);
extern spif_bool_t spif_ustr_upcase(spif_ustr_t);
SPIF_DECL_PROPERTY_FUNC_C(ustr, spif_ustridx_t, size);
SPIF_DECL_PROPERTY_FUNC_C(ustr, spif_ustridx_t, len);

#endif /* _LIBAST_USTR_H_ */
