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

/* Declaration for the spif_tok_t classname variable. */
SPIF_DECL_CLASSNAME(tok);

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
    spif_str_init(SPIF_STR(self));
    spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(tok));
    self->count = 0;
    self->token = ((spif_str_t *) (NULL));
    self->sep = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_tok_init_from_ptr(spif_tok_t self, spif_charptr_t old)
{
    spif_str_init_from_ptr(SPIF_STR(self), old);
    spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(tok));
    self->count = 0;
    self->token = ((spif_str_t *) (NULL));
    self->sep = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_tok_init_from_fp(spif_tok_t self, FILE * fp)
{
    spif_str_init_from_fp(SPIF_STR(self), fp);
    spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(tok));
    self->count = 0;
    self->token = ((spif_str_t *) (NULL));
    self->sep = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_tok_init_from_fd(spif_tok_t self, int fd)
{
    spif_str_init_from_fd(SPIF_STR(self), fd);
    spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(tok));
    self->count = 0;
    self->token = ((spif_str_t *) (NULL));
    self->sep = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_tok_done(spif_tok_t self)
{
    if (self->token) {
        size_t i;

        for (i = 0; i < self->count; i++) {
            spif_str_done(SPIF_STR(self->token[i]));
        }
        FREE(self->token);
        self->token = ((spif_str_t *) (NULL));
        self->count = 0;
    }
    if (!SPIF_OBJ_ISNULL(self->sep)) {
        spif_str_done(SPIF_STR(self->sep));
        self->sep = SPIF_NULL_TYPE(str);
    }
    spif_str_done(SPIF_STR(self));
    spif_str_init(SPIF_STR(self));
    return TRUE;
}

#define IS_DELIM(c)  ((delim != NULL) ? (strchr(delim, (c)) != NULL) : (isspace(c)))
#define IS_QUOTE(c)  (quote && quote == (c))

spif_bool_t
spif_tok_eval(spif_tok_t self)
{
#if 0
    char **slist;
    register const char *pstr;
    register char *pdest;
    char quote = 0;
    unsigned short cnt = 0;
    unsigned long len;

    REQUIRE_RVAL(str != NULL, (char **) NULL);

    if ((slist = (char **) MALLOC(sizeof(char *))) == NULL) {
        print_error("split():  Unable to allocate memory -- %s\n", strerror(errno));
        return ((char **) NULL);
    }

    /* Before we do anything, skip leading "whitespace." */
    for (pstr = str; *pstr && IS_DELIM(*pstr); pstr++);

    /* The outermost for loop is where we traverse the string.  Each new
       word brings us back to the top where we resize our string list. */
    for (; *pstr; cnt++) {
        /* First, resize the list to two bigger than our count.  Why two?
           One for the string we're about to do, and one for a trailing NULL. */
        if ((slist = (char **) REALLOC(slist, sizeof(char *) * (cnt + 2))) == NULL) {
            print_error("split():  Unable to allocate memory -- %s\n", strerror(errno));
            return ((char **) NULL);
        }

        /* The string we're about to create can't possibly be larger than the remainder
           of the string we have yet to parse, so allocate that much space to start. */
        len = strlen(pstr) + 1;
        if ((slist[cnt] = (char *) MALLOC(len)) == NULL) {
            print_error("split():  Unable to allocate memory -- %s.\n", strerror(errno));
            return ((char **) NULL);
        }
        pdest = slist[cnt];

        /* This for loop is where we process each character. */
        for (; *pstr && (quote || !IS_DELIM(*pstr));) {
            if (*pstr == '\"' || *pstr == '\'') {
                /* It's a quote character, so set or reset the quote variable. */
                if (quote) {
                    if (quote == *pstr) {
                        quote = 0;
                    } else {
                        /* It's a single quote inside double quotes, or vice versa.  Leave it alone. */
                        *pdest++ = *pstr++;
                    }
                } else {
                    quote = *pstr;
                }
                pstr++;
            } else {
                /* Handle any backslashes that are escaping delimiters or quotes. */
                if ((*pstr == '\\') && (IS_DELIM(*(pstr + 1)) || IS_QUOTE(*(pstr + 1)))) {
                    /* Incrementing pstr here moves us past the backslash so that the line
                       below will copy the next character to the new token, no questions asked. */
                    pstr++;
                }
                *pdest++ = *pstr++;
            }
        }
        /* Add the trailing \0 to terminate the new string. */
        *pdest = 0;

        /* Reallocate the new string to be just the right size. */
        len = strlen(slist[cnt]) + 1;
        slist[cnt] = (char *) REALLOC(slist[cnt], len);

        /* Move past any trailing "whitespace." */
        for (; *pstr && IS_DELIM(*pstr); pstr++);
    }
    if (cnt == 0) {
        return NULL;
    } else {
        /* The last element of slist[] should be NULL. */
        slist[cnt] = 0;
        return slist;
    }
#endif
    USE_VAR(self);
    return TRUE;
}

spif_bool_t
spif_tok_show(spif_tok_t self, spif_charptr_t name)
{
    USE_VAR(self);
    USE_VAR(name);
    return TRUE;
}
