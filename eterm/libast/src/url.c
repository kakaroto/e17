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

/* *INDENT-OFF* */
static SPIF_CONST_TYPE(class) u_class = {
    SPIF_DECL_CLASSNAME(url),
    (spif_func_t) spif_url_new,
    (spif_func_t) spif_url_init,
    (spif_func_t) spif_url_done,
    (spif_func_t) spif_url_del,
    (spif_func_t) spif_url_show,
    (spif_func_t) spif_url_comp,
    (spif_func_t) spif_url_dup,
    (spif_func_t) spif_url_type
};
SPIF_TYPE(class) SPIF_CLASS_VAR(url) = &u_class;
/* *INDENT-ON* */

static spif_bool_t spif_url_parse(spif_url_t, spif_str_t);

spif_url_t
spif_url_new(void)
{
    spif_url_t self;

    self = SPIF_ALLOC(url);
    spif_url_init(self);
    return self;
}

spif_url_t
spif_url_new_from_str(spif_str_t other)
{
    spif_url_t self;

    self = SPIF_ALLOC(url);
    spif_url_init_from_str(self, other);
    return self;
}

spif_url_t
spif_url_new_from_ptr(spif_charptr_t other)
{
    spif_url_t self;

    self = SPIF_ALLOC(url);
    spif_url_init_from_ptr(self, other);
    return self;
}

spif_bool_t
spif_url_del(spif_url_t self)
{
    spif_url_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_bool_t
spif_url_init(spif_url_t self)
{
    spif_str_init(SPIF_STR(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(url));
    self->proto = SPIF_NULL_TYPE(str);
    self->user = SPIF_NULL_TYPE(str);
    self->passwd = SPIF_NULL_TYPE(str);
    self->host = SPIF_NULL_TYPE(str);
    self->port = SPIF_NULL_TYPE(str);
    self->path = SPIF_NULL_TYPE(str);
    self->query = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_url_init_from_str(spif_url_t self, spif_str_t other)
{
    spif_str_init_from_ptr(SPIF_STR(self), SPIF_STR_STR(other));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(url));
    self->proto = SPIF_NULL_TYPE(str);
    self->user = SPIF_NULL_TYPE(str);
    self->passwd = SPIF_NULL_TYPE(str);
    self->host = SPIF_NULL_TYPE(str);
    self->port = SPIF_NULL_TYPE(str);
    self->path = SPIF_NULL_TYPE(str);
    self->query = SPIF_NULL_TYPE(str);
    spif_url_parse(self, SPIF_STR(self));
    return TRUE;
}

spif_bool_t
spif_url_init_from_ptr(spif_url_t self, spif_charptr_t other)
{
    spif_str_init_from_ptr(SPIF_STR(self), other);
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(url));
    self->proto = SPIF_NULL_TYPE(str);
    self->user = SPIF_NULL_TYPE(str);
    self->passwd = SPIF_NULL_TYPE(str);
    self->host = SPIF_NULL_TYPE(str);
    self->port = SPIF_NULL_TYPE(str);
    self->path = SPIF_NULL_TYPE(str);
    self->query = SPIF_NULL_TYPE(str);
    spif_url_parse(self, SPIF_STR(self));
    return TRUE;
}

spif_bool_t
spif_url_done(spif_url_t self)
{
    if (!SPIF_STR_ISNULL(self->proto)) {
        spif_str_del(self->proto);
        self->proto = SPIF_NULL_TYPE(str);
    }
    if (!SPIF_STR_ISNULL(self->user)) {
        spif_str_del(self->user);
        self->user = SPIF_NULL_TYPE(str);
    }
    if (!SPIF_STR_ISNULL(self->passwd)) {
        spif_str_del(self->passwd);
        self->passwd = SPIF_NULL_TYPE(str);
    }
    if (!SPIF_STR_ISNULL(self->host)) {
        spif_str_del(self->host);
        self->host = SPIF_NULL_TYPE(str);
    }
    if (!SPIF_STR_ISNULL(self->port)) {
        spif_str_del(self->port);
        self->port = SPIF_NULL_TYPE(str);
    }
    if (!SPIF_STR_ISNULL(self->path)) {
        spif_str_del(self->path);
        self->path = SPIF_NULL_TYPE(str);
    }
    if (!SPIF_STR_ISNULL(self->query)) {
        spif_str_del(self->query);
        self->query = SPIF_NULL_TYPE(str);
    }
    spif_str_done(SPIF_STR(self));
    return TRUE;
}

spif_str_t
spif_url_show(spif_url_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];

    if (SPIF_URL_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL("url", name, buff, indent);
        return buff;
    }

    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_url_t) %s:  {\n", name);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }

    buff = spif_str_show(self->proto, "proto", buff, indent + 2);
    buff = spif_str_show(self->user, "user", buff, indent + 2);
    buff = spif_str_show(self->passwd, "passwd", buff, indent + 2);
    buff = spif_str_show(self->host, "host", buff, indent + 2);
    buff = spif_str_show(self->port, "port", buff, indent + 2);
    buff = spif_str_show(self->path, "path", buff, indent + 2);
    buff = spif_str_show(self->query, "query", buff, indent + 2);

    snprintf(tmp + indent, sizeof(tmp) - indent, "}\n");
    spif_str_append_from_ptr(buff, tmp);
    return buff;
}

spif_cmp_t
spif_url_comp(spif_url_t self, spif_url_t other)
{
    return (self == other);
}

spif_url_t
spif_url_dup(spif_url_t self)
{
    spif_url_t tmp;

    tmp = spif_url_new_from_str(SPIF_STR(self));
    tmp->proto = spif_str_dup(self->proto);
    tmp->user = spif_str_dup(self->user);
    tmp->passwd = spif_str_dup(self->passwd);
    tmp->host = spif_str_dup(self->host);
    tmp->port = spif_str_dup(self->port);
    tmp->path = spif_str_dup(self->path);
    tmp->query = spif_str_dup(self->query);
    return tmp;
}

spif_classname_t
spif_url_type(spif_url_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}

spif_str_t
spif_url_get_proto(spif_url_t self)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), SPIF_NULL_TYPE(str));
    return self->proto;
}

spif_bool_t
spif_url_set_proto(spif_url_t self, spif_str_t newproto)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), FALSE);
    if (!SPIF_STR_ISNULL(self->proto)) {
        spif_str_done(self->proto);
    }
    self->proto = spif_str_dup(newproto);
    return TRUE;
}

spif_str_t
spif_url_get_user(spif_url_t self)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), SPIF_NULL_TYPE(str));
    return self->user;
}

spif_bool_t
spif_url_set_user(spif_url_t self, spif_str_t newuser)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), FALSE);
    if (!SPIF_STR_ISNULL(self->user)) {
        spif_str_done(self->user);
    }
    self->user = spif_str_dup(newuser);
    return TRUE;
}

spif_str_t
spif_url_get_passwd(spif_url_t self)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), SPIF_NULL_TYPE(str));
    return self->passwd;
}

spif_bool_t
spif_url_set_passwd(spif_url_t self, spif_str_t newpasswd)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), FALSE);
    if (!SPIF_STR_ISNULL(self->passwd)) {
        spif_str_done(self->passwd);
    }
    self->passwd = spif_str_dup(newpasswd);
    return TRUE;
}

spif_str_t
spif_url_get_host(spif_url_t self)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), SPIF_NULL_TYPE(str));
    return self->host;
}

spif_bool_t
spif_url_set_host(spif_url_t self, spif_str_t newhost)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), FALSE);
    if (!SPIF_STR_ISNULL(self->host)) {
        spif_str_done(self->host);
    }
    self->host = spif_str_dup(newhost);
    return TRUE;
}

spif_str_t
spif_url_get_port(spif_url_t self)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), SPIF_NULL_TYPE(str));
    return self->port;
}

spif_bool_t
spif_url_set_port(spif_url_t self, spif_str_t newport)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), FALSE);
    if (!SPIF_STR_ISNULL(self->port)) {
        spif_str_done(self->port);
    }
    self->port = spif_str_dup(newport);
    return TRUE;
}

spif_str_t
spif_url_get_path(spif_url_t self)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), SPIF_NULL_TYPE(str));
    return self->path;
}

spif_bool_t
spif_url_set_path(spif_url_t self, spif_str_t newpath)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), FALSE);
    if (!SPIF_STR_ISNULL(self->path)) {
        spif_str_done(self->path);
    }
    self->path = spif_str_dup(newpath);
    return TRUE;
}

spif_str_t
spif_url_get_query(spif_url_t self)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), SPIF_NULL_TYPE(str));
    return self->query;
}

spif_bool_t
spif_url_set_query(spif_url_t self, spif_str_t newquery)
{
    REQUIRE_RVAL(SPIF_OBJ_IS_URL(self), FALSE);
    if (!SPIF_STR_ISNULL(self->query)) {
        spif_str_done(self->query);
    }
    self->query = spif_str_dup(newquery);
    return TRUE;
}

static spif_bool_t
spif_url_parse(spif_url_t self, spif_str_t url_str)
{
    const char *s = SPIF_STR_STR(url_str);
    const char *pstr, *pend, *ptmp;

    pstr = s;

    /* Check for "proto:" at the beginning. */
    pend = strchr(s, ':');
    if (pend != NULL) {
        for (; pstr < pend; pstr++) {
            if (!isalnum(*pstr)) {
                break;
            }
        }
        if (pstr == pend) {
            /* Got one. */
            self->proto = spif_str_new_from_buff(SPIF_CAST(charptr) s, pend - s);
            pstr++;
        } else {
            /* Nope, reset. */
            pstr = s;
        }
    }

    if ((*pstr == '/') && (pstr[1] == '/')) {
        pstr += 2;
    }

    /* Knock out the path and query if they're there. */
    pend = strchr(pstr, '/');
    if (pend != NULL) {
        char *tmp = strchr(pend, '?');

        if (tmp != NULL) {
            self->query = spif_str_new_from_ptr(SPIF_CAST(charptr) (tmp + 1));
            self->path = spif_str_new_from_buff(SPIF_CAST(charptr) pend, tmp - pend);
        } else {
          self->path = spif_str_new_from_ptr(SPIF_CAST(charptr) pend);
        }
    } else if ((pend = strchr(pstr, '?')) != NULL) {
        self->query = spif_str_new_from_ptr(SPIF_CAST(charptr) (pend + 1));
    } else {
        for (pend = pstr; *pend; pend++);
    }
    /* At this point, pend *must* point to the end of the user/pass/host/port part. */

    /* Check for an @ sign, which would mean we have auth info. */
    ptmp = strchr(pstr, '@');
    if ((ptmp != NULL) && (ptmp < pend)) {
        char *tmp = strchr(pstr, ':');

        if ((tmp != NULL) && (tmp < ptmp)) {
            /* Both username and password. */
            self->user = spif_str_new_from_buff(SPIF_CAST(charptr) pstr, tmp - pstr);
            self->passwd = spif_str_new_from_buff(SPIF_CAST(charptr) (tmp + 1), ptmp - tmp - 1);
        } else {
            self->user = spif_str_new_from_buff(SPIF_CAST(charptr) pstr, ptmp - pstr);
        }
        pstr = ptmp + 1;
    }

    /* All that remains now between pstr and pend is host and maybe port. */
    ptmp = strchr(pstr, ':');
    if ((ptmp != NULL) && (ptmp < pend)) {
        self->host = spif_str_new_from_buff(SPIF_CAST(charptr) pstr, ptmp - pstr);
        self->port = spif_str_new_from_buff(SPIF_CAST(charptr) (ptmp + 1), pend - ptmp - 1);
    } else if (pstr != pend) {
        self->host = spif_str_new_from_buff(SPIF_CAST(charptr) pstr, pend - pstr);
    }
    return TRUE;
}
