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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libast_internal.h>

/* *INDENT-OFF* */
static SPIF_CONST_TYPE(class) t_class = {
    SPIF_DECL_CLASSNAME(tok),
    (spif_func_t) spif_tok_new,
    (spif_func_t) spif_tok_init,
    (spif_func_t) spif_tok_done,
    (spif_func_t) spif_tok_del,
    (spif_func_t) spif_tok_show,
    (spif_func_t) spif_tok_comp,
    (spif_func_t) spif_tok_dup,
    (spif_func_t) spif_tok_type
};
SPIF_TYPE(class) SPIF_CLASS_VAR(tok) = &t_class;
/* *INDENT-ON* */

spif_tok_t
spif_tok_new(void)
{
    spif_tok_t self;

    self = SPIF_ALLOC(tok);
    spif_tok_init(self);
    return self;
}

spif_tok_t
spif_tok_new_from_ptr(spif_charptr_t old)
{
    spif_tok_t self;

    self = SPIF_ALLOC(tok);
    spif_tok_init_from_ptr(self, old);
    return self;
}

spif_tok_t
spif_tok_new_from_fp(FILE * fp)
{
    spif_tok_t self;

    self = SPIF_ALLOC(tok);
    spif_tok_init_from_fp(self, fp);
    return self;
}

spif_tok_t
spif_tok_new_from_fd(int fd)
{
    spif_tok_t self;

    self = SPIF_ALLOC(tok);
    spif_tok_init_from_fd(self, fd);
    return self;
}

spif_bool_t
spif_tok_del(spif_tok_t self)
{
    spif_tok_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_bool_t
spif_tok_init(spif_tok_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(tok));
    self->src = SPIF_NULL_TYPE(str);
    self->quote = '\'';
    self->dquote = '\"';
    self->escape = '\\';
    self->tokens = SPIF_NULL_TYPE(list);
    self->sep = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_tok_init_from_ptr(spif_tok_t self, spif_charptr_t old)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(tok));
    self->src = spif_str_new_from_ptr(old);
    self->quote = '\'';
    self->dquote = '\"';
    self->escape = '\\';
    self->tokens = SPIF_NULL_TYPE(list);
    self->sep = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_tok_init_from_fp(spif_tok_t self, FILE * fp)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(tok));
    self->src = spif_str_new_from_fp(fp);
    self->quote = '\'';
    self->dquote = '\"';
    self->escape = '\\';
    self->tokens = SPIF_NULL_TYPE(list);
    self->sep = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_tok_init_from_fd(spif_tok_t self, int fd)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(tok));
    self->src = spif_str_new_from_fd(fd);
    self->quote = '\'';
    self->dquote = '\"';
    self->escape = '\\';
    self->tokens = SPIF_NULL_TYPE(list);
    self->sep = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_tok_done(spif_tok_t self)
{
    if (!SPIF_LIST_ISNULL(self->tokens)) {
        SPIF_LIST_DEL(self->tokens);
        self->tokens = SPIF_NULL_TYPE(list);
    }
    if (!SPIF_STR_ISNULL(self->src)) {
        spif_str_del(SPIF_STR(self->src));
        self->src = SPIF_NULL_TYPE(str);
    }
    if (!SPIF_STR_ISNULL(self->sep)) {
        spif_str_del(SPIF_STR(self->sep));
        self->sep = SPIF_NULL_TYPE(str);
    }
    self->quote = '\'';
    self->dquote = '\"';
    self->escape = '\\';
    return TRUE;
}

#define IS_DELIM(c)  ((delim != NULL) ? (strchr(delim, (c)) != NULL) : (isspace(c)))
#define IS_QUOTE(c)  (quote && quote == (c))

spif_bool_t
spif_tok_eval(spif_tok_t self)
{
    const char *pstr, *delim = NULL;
    spif_str_t tmp;
    char quote;
    size_t len;

    if (SPIF_STR_ISNULL(self->src)) {
        return FALSE;
    }
    pstr = SPIF_CAST_C(const char *) SPIF_STR_STR(SPIF_STR(self->src));
    len = spif_str_get_len(SPIF_STR(self->src));

    if (!SPIF_STR_ISNULL(self->sep)) {
        delim = SPIF_CAST_C(const char *) SPIF_STR_STR(SPIF_STR(self->sep));
    }

    if (!SPIF_LIST_ISNULL(self->tokens)) {
        SPIF_LIST_DEL(self->tokens);
    }
    self->tokens = SPIF_LIST_NEW(dlinked_list);

    /* Before we do anything, skip leading "whitespace." */
    for (; *pstr && IS_DELIM(*pstr); pstr++);

    /* The outermost for loop is where we traverse the string.  Each new
       word brings us back to the top where we resize our string list. */
    for (quote = 0; *pstr; ) {
        tmp = spif_str_new_from_buff("", len);
        spif_str_clear(tmp, 0);

        /* This for loop is where we process each character. */
        for (; *pstr && (quote || !IS_DELIM(*pstr));) {
            if (*pstr == self->dquote || *pstr == self->quote) {
                /* It's a quote character, so set or reset the quote variable. */
                if (quote) {
                    if (quote == *pstr) {
                        quote = 0;
                    } else {
                        /* It's a single quote inside double quotes, or vice versa.  Leave it alone. */
                        spif_str_append_char(tmp, *pstr);
                    }
                } else {
                    quote = *pstr;
                }
                pstr++;
            } else {
                /* Handle any backslashes that are escaping delimiters or quotes. */
                if ((*pstr == self->escape) && (IS_DELIM(*(pstr + 1)) || IS_QUOTE(*(pstr + 1)))) {
                    /* Incrementing pstr here moves us past the backslash so that the line
                       below will copy the next character to the new token, no questions asked. */
                    pstr++;
                }
                spif_str_append_char(tmp, *pstr++);
            }
        }

        /* Reallocate the new string to be just the right size. */
        spif_str_trim(tmp);
        len -= spif_str_get_len(tmp);

        /* Add it to the list */
        SPIF_LIST_APPEND(self->tokens, tmp);

        /* Move past any trailing "whitespace." */
        for (; *pstr && IS_DELIM(*pstr); pstr++);
    }
    return TRUE;
}

spif_str_t
spif_tok_show(spif_tok_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];

    if (SPIF_TOK_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(tok, name, buff, indent, tmp);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_tok_t) %s:  %010p {\n", name, self);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }
    buff = spif_str_show(SPIF_STR(self->src), "src", buff, indent + 2);
    buff = spif_str_show(SPIF_STR(self->sep), "sep", buff, indent + 2);

    indent += 2;
    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_char_t) quote:  '%c' (0x%02x)\n",
             (char) self->quote, (unsigned int) self->quote);
    spif_str_append_from_ptr(buff, tmp);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_char_t) dquote:  '%c' (0x%02x)\n",
             (char) self->dquote, (unsigned int) self->dquote);
    spif_str_append_from_ptr(buff, tmp);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_char_t) escape:  '%c' (0x%02x)\n",
             (char) self->escape, (unsigned int) self->escape);
    spif_str_append_from_ptr(buff, tmp);

    SPIF_LIST_SHOW(self->tokens, buff, indent);
    indent -= 2;

    snprintf(tmp + indent, sizeof(tmp) - indent, "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

spif_cmp_t
spif_tok_comp(spif_tok_t self, spif_tok_t other)
{
    return spif_str_cmp(SPIF_STR(self), SPIF_STR(other));
}

spif_tok_t
spif_tok_dup(spif_tok_t self)
{
    spif_tok_t tmp;

    tmp = spif_tok_new();
    tmp->src = spif_str_dup(SPIF_STR(self->src));
    tmp->quote = self->quote;
    tmp->dquote = self->dquote;
    tmp->escape = self->escape;
    tmp->tokens = SPIF_LIST_DUP(self->tokens);
    tmp->sep = spif_str_dup(SPIF_STR(self->sep));

    return tmp;
}

spif_classname_t
spif_tok_type(spif_tok_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}

spif_str_t
spif_tok_get_src(spif_tok_t self)
{
    return ((SPIF_OBJ_IS_TOK(self)) ? (SPIF_STR(self->src)) : (SPIF_NULL_TYPE(str)));
}

spif_bool_t
spif_tok_set_src(spif_tok_t self, spif_str_t new_src)
{
    if (SPIF_OBJ_IS_TOK(self) && SPIF_OBJ_IS_STR(new_src)) {
        if (!SPIF_STR_ISNULL(self->src)) {
            spif_str_done(self->src);
        }
        self->src = spif_str_dup(new_src);
        return TRUE;
    } else {
        return FALSE;
    }
}

spif_char_t
spif_tok_get_quote(spif_tok_t self)
{
    return ((SPIF_OBJ_IS_TOK(self)) ? (SPIF_CAST(char) self->quote) : (SPIF_CAST(char) 0));
}

spif_bool_t
spif_tok_set_quote(spif_tok_t self, spif_char_t c)
{
    if (SPIF_OBJ_IS_TOK(self)) {
        self->quote = c;
        return TRUE;
    } else {
        return FALSE;
    }
}

spif_char_t
spif_tok_get_dquote(spif_tok_t self)
{
    return ((SPIF_OBJ_IS_TOK(self)) ? (SPIF_CAST(char) self->dquote) : (SPIF_CAST(char) 0));
}

spif_bool_t
spif_tok_set_dquote(spif_tok_t self, spif_char_t c)
{
    if (SPIF_OBJ_IS_TOK(self)) {
        self->dquote = c;
        return TRUE;
    } else {
        return FALSE;
    }
}

spif_char_t
spif_tok_get_escape(spif_tok_t self)
{
    return ((SPIF_OBJ_IS_TOK(self)) ? (SPIF_CAST(char) self->escape) : (SPIF_CAST(char) 0));
}

spif_bool_t
spif_tok_set_escape(spif_tok_t self, spif_char_t c)
{
    if (SPIF_OBJ_IS_TOK(self)) {
        self->escape = c;
        return TRUE;
    } else {
        return FALSE;
    }
}

spif_str_t
spif_tok_get_sep(spif_tok_t self)
{
    return ((SPIF_OBJ_IS_TOK(self)) ? (SPIF_STR(self->sep)) : (SPIF_NULL_TYPE(str)));
}

spif_bool_t
spif_tok_set_sep(spif_tok_t self, spif_str_t new_sep)
{
    if (SPIF_OBJ_IS_TOK(self) && SPIF_OBJ_IS_STR(new_sep)) {
        if (!SPIF_STR_ISNULL(self->sep)) {
            spif_str_done(self->sep);
        }
        self->sep = spif_str_dup(new_sep);
        return TRUE;
    } else {
        return FALSE;
    }
}

spif_list_t
spif_tok_get_tokens(spif_tok_t self)
{
    if (!SPIF_OBJ_IS_TOK(self)) {
        return SPIF_NULL_TYPE(list);
    }
    if (SPIF_LIST_ISNULL(self->tokens)) {
        spif_tok_eval(self);
    }
    return self->tokens;
}
