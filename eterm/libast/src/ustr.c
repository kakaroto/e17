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

/* *INDENT-OFF* */
static SPIF_CONST_TYPE(strclass) s_class = {
    {
        SPIF_DECL_CLASSNAME(ustr),
        (spif_func_t) spif_ustr_new,
        (spif_func_t) spif_ustr_init,
        (spif_func_t) spif_ustr_done,
        (spif_func_t) spif_ustr_del,
        (spif_func_t) spif_ustr_show,
        (spif_func_t) spif_ustr_comp,
        (spif_func_t) spif_ustr_dup,
        (spif_func_t) spif_ustr_type
    },
    (spif_func_t) spif_ustr_new_from_ptr,
    (spif_func_t) spif_ustr_new_from_buff,
    (spif_func_t) spif_ustr_new_from_fp,
    (spif_func_t) spif_ustr_new_from_fd,
    (spif_func_t) spif_ustr_new_from_num,
    (spif_func_t) spif_ustr_init_from_ptr,
    (spif_func_t) spif_ustr_init_from_buff,
    (spif_func_t) spif_ustr_init_from_fp,
    (spif_func_t) spif_ustr_init_from_fd,
    (spif_func_t) spif_ustr_init_from_num,
    (spif_func_t) spif_ustr_append,
    (spif_func_t) spif_ustr_append_char,
    (spif_func_t) spif_ustr_append_from_ptr,
    (spif_func_t) spif_ustr_casecmp,
    (spif_func_t) spif_ustr_casecmp_with_ptr,
    (spif_func_t) spif_ustr_clear,
    (spif_func_t) spif_ustr_cmp,
    (spif_func_t) spif_ustr_cmp_with_ptr,
    (spif_func_t) spif_ustr_downcase,
    (spif_func_t) spif_ustr_find,
    (spif_func_t) spif_ustr_find_from_ptr,
    (spif_func_t) spif_ustr_index,
    (spif_func_t) spif_ustr_ncasecmp,
    (spif_func_t) spif_ustr_ncasecmp_with_ptr,
    (spif_func_t) spif_ustr_ncmp,
    (spif_func_t) spif_ustr_ncmp_with_ptr,
    (spif_func_t) spif_ustr_prepend,
    (spif_func_t) spif_ustr_prepend_char,
    (spif_func_t) spif_ustr_prepend_from_ptr,
    (spif_func_t) spif_ustr_reverse,
    (spif_func_t) spif_ustr_rindex,
    (spif_func_t) spif_ustr_splice,
    (spif_func_t) spif_ustr_splice_from_ptr,
    (spif_func_t) spif_ustr_sprintf,
    (spif_func_t) spif_ustr_substr,
    (spif_func_t) spif_ustr_substr_to_ptr,
    (spif_func_t) spif_ustr_to_float,
    (spif_func_t) spif_ustr_to_num,
    (spif_func_t) spif_ustr_trim,
    (spif_func_t) spif_ustr_upcase
};
SPIF_TYPE(class) SPIF_CLASS_VAR(ustr) = (spif_class_t) &s_class;
SPIF_TYPE(strclass) SPIF_STRCLASS_VAR(ustr) = &s_class;
/* *INDENT-ON* */

const size_t buff_inc = 4096;

spif_ustr_t
spif_ustr_new(void)
{
    spif_ustr_t self;

    self = SPIF_ALLOC(ustr);
    if (!spif_ustr_init(self)) {
        SPIF_DEALLOC(self);
        self = (spif_ustr_t) NULL;
    }
    return self;
}

spif_ustr_t
spif_ustr_new_from_ptr(spif_charptr_t old)
{
    spif_ustr_t self;

    self = SPIF_ALLOC(ustr);
    if (!spif_ustr_init_from_ptr(self, old)) {
        SPIF_DEALLOC(self);
        self = (spif_ustr_t) NULL;
    }
    return self;
}

spif_ustr_t
spif_ustr_new_from_buff(spif_charptr_t buff, spif_ustridx_t size)
{
    spif_ustr_t self;

    self = SPIF_ALLOC(ustr);
    if (!spif_ustr_init_from_buff(self, buff, size)) {
        SPIF_DEALLOC(self);
        self = (spif_ustr_t) NULL;
    }
    return self;
}

spif_ustr_t
spif_ustr_new_from_fp(FILE * fp)
{
    spif_ustr_t self;

    self = SPIF_ALLOC(ustr);
    if (!spif_ustr_init_from_fp(self, fp)) {
        SPIF_DEALLOC(self);
        self = (spif_ustr_t) NULL;
    }
    return self;
}

spif_ustr_t
spif_ustr_new_from_fd(int fd)
{
    spif_ustr_t self;

    self = SPIF_ALLOC(ustr);
    if (!spif_ustr_init_from_fd(self, fd)) {
        SPIF_DEALLOC(self);
        self = (spif_ustr_t) NULL;
    }
    return self;
}

spif_ustr_t
spif_ustr_new_from_num(long num)
{
    spif_ustr_t self;

    self = SPIF_ALLOC(ustr);
    if (!spif_ustr_init_from_num(self, num)) {
        SPIF_DEALLOC(self);
        self = (spif_ustr_t) NULL;
    }
    return self;
}

spif_bool_t
spif_ustr_init(spif_ustr_t self)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(ustr));
    self->s = (spif_charptr_t) NULL;
    self->len = 0;
    self->size = 0;
    return TRUE;
}

spif_bool_t
spif_ustr_init_from_ptr(spif_ustr_t self, spif_charptr_t old)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    REQUIRE_RVAL((old != (spif_charptr_t) NULL), spif_ustr_init(self));
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(ustr));
    self->len = strlen((const char *) old);
    self->size = self->len + 1;
    self->s = (spif_charptr_t) MALLOC(self->size);
    memcpy(self->s, old, self->size);
    return TRUE;
}

spif_bool_t
spif_ustr_init_from_buff(spif_ustr_t self, spif_charptr_t buff, spif_ustridx_t size)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(ustr));
    self->size = size;
    if (buff != (spif_charptr_t) NULL) {
        self->len = strnlen((const char *) buff, size);
    } else {
        self->len = 0;
    }
    if (self->size == self->len) {
        self->size++;
    }
    self->s = (spif_charptr_t) MALLOC(self->size);
    if (buff != (spif_charptr_t) NULL) {
        memcpy(self->s, buff, self->len);
    }
    self->s[self->len] = 0;
    return TRUE;
}

spif_bool_t
spif_ustr_init_from_fp(spif_ustr_t self, FILE *fp)
{
    spif_charptr_t p, end = NULL;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    ASSERT_RVAL((fp != (FILE *) NULL), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(ustr));
    self->size = buff_inc;
    self->len = 0;
    self->s = (spif_charptr_t) MALLOC(self->size);

    for (p = self->s; fgets((char *)p, buff_inc, fp); p += buff_inc) {
        if ((end = (spif_charptr_t)
             strchr((const char *)p, '\n')) == NULL) {
            self->size += buff_inc;
            self->s = (spif_charptr_t) REALLOC(self->s, self->size);
        } else {
            *end = 0;
            break;
        }
    }
    self->len = (spif_ustridx_t) ((end)
                          ? (end - self->s)
                          : ((int) strlen((const char *)self->s)));
    self->size = self->len + 1;
    self->s = (spif_charptr_t) REALLOC(self->s, self->size);
    return TRUE;
}

spif_bool_t
spif_ustr_init_from_fd(spif_ustr_t self, int fd)
{
    int n;
    spif_charptr_t p;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    ASSERT_RVAL((fd >= 0), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(ustr));
    self->size = buff_inc;
    self->len = 0;
    self->s = (spif_charptr_t) MALLOC(self->size);

    for (p = self->s; ((n = read(fd, p, buff_inc)) > 0) || (errno == EINTR);) {
        self->size += n;
        self->s = (spif_charptr_t) REALLOC(self->s, self->size);
        p += n;
    }
    self->len = self->size - buff_inc;
    self->size = self->len + 1;
    self->s = (spif_charptr_t) REALLOC(self->s, self->size);
    self->s[self->len] = 0;
    return TRUE;
}

spif_bool_t
spif_ustr_init_from_num(spif_ustr_t self, long num)
{
    spif_char_t buff[28];

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    /* ***NOT NEEDED*** spif_obj_init(SPIF_OBJ(self)); */
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(ustr));

    snprintf((char *) buff, sizeof(buff), "%ld", num);
    self->len = strlen((char *) buff);
    self->size = self->len + 1;
    self->s = (spif_charptr_t) MALLOC(self->size);
    strcpy((char *) self->s, (char *) buff);

    return TRUE;
}

spif_bool_t
spif_ustr_done(spif_ustr_t self)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    if (self->size) {
        FREE(self->s);
        self->len = 0;
        self->size = 0;
        self->s = (spif_charptr_t) NULL;
    }
    return TRUE;
}

spif_bool_t
spif_ustr_del(spif_ustr_t self)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    spif_ustr_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_str_t
spif_ustr_show(spif_ustr_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    spif_char_t tmp[4096];

    if (SPIF_USTR_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(ustr, name, buff, indent, tmp);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf((char *) tmp + indent, sizeof(tmp) - indent,
             "(spif_ustr_t) %s:  %10p { \"",
             name, (spif_ptr_t) self);
    if (SPIF_USTR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }

    /*spif_str_append(buff, self);*/

    snprintf((char *) tmp, sizeof(tmp), "\", len %lu, size %lu }\n", (unsigned long) self->len,
             (unsigned long) self->size);
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

spif_cmp_t
spif_ustr_comp(spif_ustr_t self, spif_ustr_t other)
{
    return spif_ustr_cmp(self, other);
}

spif_ustr_t
spif_ustr_dup(spif_ustr_t self)
{
    spif_ustr_t tmp;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), (spif_ustr_t) NULL);
    tmp = SPIF_ALLOC(ustr);
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(ustr));
    tmp->s = (spif_charptr_t) STRDUP((const char *) SPIF_USTR_STR(self));
    tmp->len = self->len;
    tmp->size = self->size;
    return tmp;
}

spif_classname_t
spif_ustr_type(spif_ustr_t self)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), (spif_classname_t) SPIF_NULLSTR_TYPE(classname));
    return SPIF_OBJ_CLASSNAME(self);
}

spif_bool_t
spif_ustr_append(spif_ustr_t self, spif_ustr_t other)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    REQUIRE_RVAL(!SPIF_USTR_ISNULL(other), FALSE);
    if (other->size && other->len) {
        self->size += other->size - 1;
        self->s = (spif_charptr_t) REALLOC(self->s, self->size);
        memcpy(self->s + self->len, SPIF_USTR_STR(other), other->len + 1);
        self->len += other->len;
    }
    return TRUE;
}

spif_bool_t
spif_ustr_append_char(spif_ustr_t self, spif_char_t c)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    self->len++;
    if (self->size <= self->len) {
        self->size++;
        self->s = (spif_charptr_t) REALLOC(self->s, self->size);
    }
    self->s[self->len - 1] = c;
    self->s[self->len] = 0;
    return TRUE;
}

spif_bool_t
spif_ustr_append_from_ptr(spif_ustr_t self, spif_charptr_t other)
{
    spif_ustridx_t len;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    REQUIRE_RVAL((other != (spif_charptr_t) NULL), FALSE);
    len = strlen((const char *) other);
    if (len) {
        self->size += len;
        self->s = (spif_charptr_t) REALLOC(self->s, self->size);
        memcpy(self->s + self->len, other, len + 1);
        self->len += len;
    }
    return TRUE;
}

spif_cmp_t
spif_ustr_casecmp(spif_ustr_t self, spif_ustr_t other)
{
    int c;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    c = strcasecmp((char *) SPIF_USTR_STR(self), (char *) SPIF_USTR_STR(other));
    return SPIF_CMP_FROM_INT(c);
}

spif_cmp_t
spif_ustr_casecmp_with_ptr(spif_ustr_t self, spif_charptr_t other)
{
    int c;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    c = strcasecmp((char *) SPIF_USTR_STR(self), (char *) other);
    return SPIF_CMP_FROM_INT(c);
}

spif_bool_t
spif_ustr_clear(spif_ustr_t self, spif_char_t c)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    memset(self->s, c, self->size);
    self->s[self->len] = 0;
    return TRUE;
}

spif_cmp_t
spif_ustr_cmp(spif_ustr_t self, spif_ustr_t other)
{
    int c;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    c = strcmp((char *) SPIF_USTR_STR(self), (char *) SPIF_USTR_STR(other));
    return SPIF_CMP_FROM_INT(c);
}

spif_cmp_t
spif_ustr_cmp_with_ptr(spif_ustr_t self, spif_charptr_t other)
{
    int c;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    c = strcmp((char *) SPIF_USTR_STR(self), (char *) other);
    return SPIF_CMP_FROM_INT(c);
}

spif_bool_t
spif_ustr_downcase(spif_ustr_t self)
{
    spif_charptr_t tmp;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    for (tmp = self->s; *tmp; tmp++) {
        *tmp = tolower(*tmp);
    }
    return TRUE;
}

spif_ustridx_t
spif_ustr_find(spif_ustr_t self, spif_ustr_t other)
{
    char *tmp;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), ((spif_stridx_t) -1));
    REQUIRE_RVAL(!SPIF_USTR_ISNULL(other), ((spif_stridx_t) -1));
    tmp = strstr((const char *) SPIF_USTR_STR(self),
                 (const char *) SPIF_USTR_STR(other));
    if (tmp) {
        return (spif_stridx_t) ((spif_long_t) tmp - (spif_long_t) (SPIF_USTR_STR(self)));
    } else {
        return (spif_stridx_t) (self->len);
    }
}

spif_ustridx_t
spif_ustr_find_from_ptr(spif_ustr_t self, spif_charptr_t other)
{
    char *tmp;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), ((spif_stridx_t) -1));
    REQUIRE_RVAL((other != (spif_charptr_t) NULL), ((spif_stridx_t) -1));
    tmp = strstr((const char *) SPIF_USTR_STR(self),
                 (const char *) other);
    if (tmp) {
        return (spif_stridx_t) ((spif_long_t) tmp - (spif_long_t) (SPIF_USTR_STR(self)));
    } else {
        return (spif_stridx_t) (self->len);
    }
}

spif_ustridx_t
spif_ustr_index(spif_ustr_t self, spif_char_t c)
{
    char *tmp;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), ((spif_stridx_t) -1));
    tmp = index((const char *) SPIF_USTR_STR(self), c);
    if (tmp) {
        return (spif_stridx_t) ((spif_long_t) tmp - (spif_long_t) (SPIF_USTR_STR(self)));
    } else {
        return (spif_stridx_t) (self->len);
    }
}

spif_cmp_t
spif_ustr_ncasecmp(spif_ustr_t self, spif_ustr_t other, spif_ustridx_t cnt)
{
    int c;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    c = strncasecmp((char *) SPIF_USTR_STR(self), (char *) SPIF_USTR_STR(other), cnt);
    return SPIF_CMP_FROM_INT(c);
}

spif_cmp_t
spif_ustr_ncasecmp_with_ptr(spif_ustr_t self, spif_charptr_t other, spif_ustridx_t cnt)
{
    int c;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    c = strncasecmp((char *) SPIF_USTR_STR(self), (char *) other, cnt);
    return SPIF_CMP_FROM_INT(c);
}

spif_cmp_t
spif_ustr_ncmp(spif_ustr_t self, spif_ustr_t other, spif_ustridx_t cnt)
{
    int c;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    c = strncmp((char *) SPIF_USTR_STR(self), (char *) SPIF_USTR_STR(other), cnt);
    return SPIF_CMP_FROM_INT(c);
}

spif_cmp_t
spif_ustr_ncmp_with_ptr(spif_ustr_t self, spif_charptr_t other, spif_ustridx_t cnt)
{
    int c;

    SPIF_OBJ_COMP_CHECK_NULL(self, other);
    c = strncmp((char *) SPIF_USTR_STR(self), (char *) other, cnt);
    return SPIF_CMP_FROM_INT(c);
}

spif_bool_t
spif_ustr_prepend(spif_ustr_t self, spif_ustr_t other)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    REQUIRE_RVAL(!SPIF_USTR_ISNULL(other), FALSE);
    if (other->size && other->len) {
        self->size += other->size - 1;
        self->s = (spif_charptr_t) REALLOC(self->s, self->size);
        memmove(self->s + other->len, self->s, self->len + 1);
        memcpy(self->s, SPIF_USTR_STR(other), other->len);
        self->len += other->len;
    }
    return TRUE;
}

spif_bool_t
spif_ustr_prepend_char(spif_ustr_t self, spif_char_t c)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    self->len++;
    if (self->size <= self->len) {
        self->size++;
        self->s = (spif_charptr_t) REALLOC(self->s, self->size);
    }
    memmove(self->s + 1, self->s, self->len + 1);
    self->s[0] = (spif_uchar_t) c;
    return TRUE;
}

spif_bool_t
spif_ustr_prepend_from_ptr(spif_ustr_t self, spif_charptr_t other)
{
    spif_ustridx_t len;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    REQUIRE_RVAL((other != (spif_charptr_t) NULL), FALSE);
    len = strlen((const char *) other);
    if (len) {
        self->size += len;
        self->s = (spif_charptr_t) REALLOC(self->s, self->size);
        memmove(self->s + len, self->s, self->len + 1);
        memcpy(self->s, other, len);
        self->len += len;
    }
    return TRUE;
}

spif_bool_t
spif_ustr_reverse(spif_ustr_t self)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    return ((strrev((char *) self->s)) ? TRUE : FALSE);
}

spif_ustridx_t
spif_ustr_rindex(spif_ustr_t self, spif_char_t c)
{
    char *tmp;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), ((spif_stridx_t) -1));
    tmp = rindex((const char *) SPIF_USTR_STR(self), c);
    if (tmp) {
        return (spif_stridx_t) ((spif_long_t) tmp - (spif_long_t) (SPIF_USTR_STR(self)));
    } else {
        return (spif_stridx_t) (self->len);
    }
}

spif_bool_t
spif_ustr_splice(spif_ustr_t self, spif_ustridx_t idx, spif_ustridx_t cnt, spif_ustr_t other)
{
    spif_charptr_t tmp, ptmp;
    spif_ustridx_t newsize;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    if (idx < 0) {
        idx = self->len + idx;
    }
    REQUIRE_RVAL(idx >= 0, FALSE);
    REQUIRE_RVAL(idx < self->len, FALSE);
    if (cnt < 0) {
        cnt = idx + self->len + cnt;
    }
    REQUIRE_RVAL(cnt >= 0, FALSE);
    REQUIRE_RVAL(cnt <= (self->len - idx), FALSE);

    newsize = self->len + ((SPIF_USTR_ISNULL(other)) ? (0) : (other->len)) - cnt + 1;
    ptmp = tmp = (spif_charptr_t) MALLOC(newsize);
    if (idx > 0) {
        memcpy(tmp, self->s, idx);
        ptmp += idx;
    }
    if (!SPIF_OBJ_ISNULL(other)) {
        memcpy(ptmp, other->s, other->len);
        ptmp += other->len;
    }
    memcpy(ptmp, self->s + idx + cnt, self->len - idx - cnt + 1);
    if (self->size < newsize) {
        self->s = (spif_charptr_t) REALLOC(self->s, newsize);
        self->size = newsize;
    }
    self->len = newsize - 1;
    memcpy(self->s, tmp, newsize);
    FREE(tmp);
    return TRUE;
}

spif_bool_t
spif_ustr_splice_from_ptr(spif_ustr_t self, spif_ustridx_t idx, spif_ustridx_t cnt, spif_charptr_t other)
{
    spif_charptr_t tmp, ptmp;
    spif_ustridx_t len, newsize;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    len = (other ? strlen((const char *) other) : 0);
    if (idx < 0) {
        idx = self->len + idx;
    }
    REQUIRE_RVAL(idx >= 0, FALSE);
    REQUIRE_RVAL(idx < self->len, FALSE);
    if (cnt < 0) {
        cnt = idx + self->len + cnt;
    }
    REQUIRE_RVAL(cnt >= 0, FALSE);
    REQUIRE_RVAL(cnt <= (self->len - idx), FALSE);

    newsize = self->len + len - cnt + 1;
    ptmp = tmp = (spif_charptr_t) MALLOC(newsize);
    if (idx > 0) {
        memcpy(tmp, self->s, idx);
        ptmp += idx;
    }
    if (len) {
        memcpy(ptmp, other, len);
        ptmp += len;
    }
    memcpy(ptmp, self->s + idx + cnt, self->len - idx - cnt + 1);
    if (self->size < newsize) {
        self->s = (spif_charptr_t) REALLOC(self->s, newsize);
        self->size = newsize;
    }
    self->len = newsize - 1;
    memcpy(self->s, tmp, newsize);
    FREE(tmp);
    return TRUE;
}

spif_bool_t
spif_ustr_sprintf(spif_ustr_t self, spif_charptr_t format, ...)
{
    va_list ap;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    va_start(ap, format);
    if (self->s != (spif_charptr_t) NULL) {
        spif_ustr_done(self);
    }
    if (*format == 0) {
        return TRUE;
    } else if (*(format + 1) == 0) {
        return spif_ustr_init_from_ptr(self, format);
    } else {
        int c;
        char buff[2];

        c = vsnprintf(buff, sizeof(buff), format, ap);
        if (c <= 0) {
            return TRUE;
        } else {
            self->len = c;
            self->size = c + 1;
            self->s = (spif_charptr_t) MALLOC(self->size);
            c = vsnprintf(self->s, c + 1, format, ap);
        }
        return ((c >= 0) ? (TRUE) : (FALSE));
    }
    ASSERT_NOTREACHED_RVAL(FALSE);
}

spif_ustr_t
spif_ustr_substr(spif_ustr_t self, spif_ustridx_t idx, spif_ustridx_t cnt)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), (spif_ustr_t) NULL);
    if (idx < 0) {
        idx = self->len + idx;
    }
    REQUIRE_RVAL(idx >= 0, (spif_ustr_t) NULL);
    REQUIRE_RVAL(idx < self->len, (spif_ustr_t) NULL);
    if (cnt <= 0) {
        cnt = self->len - idx + cnt;
    }
    REQUIRE_RVAL(cnt >= 0, (spif_ustr_t) NULL);
    UPPER_BOUND(cnt, self->len - idx);
    return spif_ustr_new_from_buff(SPIF_USTR_STR(self) + idx, cnt);
}

spif_charptr_t
spif_ustr_substr_to_ptr(spif_ustr_t self, spif_ustridx_t idx, spif_ustridx_t cnt)
{
    spif_charptr_t newstr;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), (spif_charptr_t) NULL);
    if (idx < 0) {
        idx = self->len + idx;
    }
    REQUIRE_RVAL(idx >= 0, (spif_charptr_t) NULL);
    REQUIRE_RVAL(idx < self->len, (spif_charptr_t) NULL);
    if (cnt <= 0) {
        cnt = self->len - idx + cnt;
    }
    REQUIRE_RVAL(cnt >= 0, (spif_charptr_t) NULL);
    UPPER_BOUND(cnt, self->len - idx);

    newstr = (spif_charptr_t) MALLOC(cnt + 1);
    memcpy(newstr, SPIF_USTR_STR(self) + idx, cnt);
    newstr[cnt] = 0;
    return newstr;
}

double
spif_ustr_to_float(spif_ustr_t self)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), (double) NAN);
    return (double) (strtod((const char *)SPIF_USTR_STR(self), (char **) NULL));
}

size_t
spif_ustr_to_num(spif_ustr_t self, int base)
{
    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), ((size_t) -1));
    return (size_t) (strtoul((const char *) SPIF_USTR_STR(self), (char **) NULL, base));
}

spif_bool_t
spif_ustr_trim(spif_ustr_t self)
{
    spif_charptr_t start, end;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    start = self->s;
    end = self->s + self->len - 1;
    for (; isspace((spif_uchar_t) (*start)) && (start < end); start++);
    for (; isspace((spif_uchar_t) (*end)) && (start < end); end--);
    if (start > end) {
        return spif_ustr_done(self);
    }
    *(++end) = 0;
    self->len = (spif_ustridx_t) (end - start);
    self->size = self->len + 1;
    memmove(self->s, start, self->size);
    self->s = (spif_charptr_t) REALLOC(self->s, self->size);
    return TRUE;
}

spif_bool_t
spif_ustr_upcase(spif_ustr_t self)
{
    spif_charptr_t tmp;

    ASSERT_RVAL(!SPIF_USTR_ISNULL(self), FALSE);
    for (tmp = self->s; *tmp; tmp++) {
        *tmp = toupper(*tmp);
    }
    return TRUE;
}

SPIF_DEFINE_PROPERTY_FUNC_C(ustr, spif_ustridx_t, size)
SPIF_DEFINE_PROPERTY_FUNC_C(ustr, spif_ustridx_t, len)
