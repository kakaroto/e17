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

static spif_ipsockaddr_t spif_url_get_ipaddr(spif_url_t);
static spif_unixsockaddr_t spif_url_get_unixaddr(spif_url_t);
static spif_sockport_t spif_url_get_portnum(spif_url_t);
static spif_bool_t spif_socket_get_proto(spif_socket_t);

/* *INDENT-OFF* */
static SPIF_CONST_TYPE(class) o_class = {
    SPIF_DECL_CLASSNAME(socket),
    (spif_func_t) spif_socket_new,
    (spif_func_t) spif_socket_init,
    (spif_func_t) spif_socket_done,
    (spif_func_t) spif_socket_del,
    (spif_func_t) spif_socket_show,
    (spif_func_t) spif_socket_comp,
    (spif_func_t) spif_socket_dup,
    (spif_func_t) spif_socket_type
};
SPIF_TYPE(class) SPIF_CLASS_VAR(socket) = &o_class;
/* *INDENT-ON* */

spif_socket_t
spif_socket_new(void)
{
    spif_socket_t self;

    self = SPIF_ALLOC(socket);
    spif_socket_init(self);
    return self;
}

spif_socket_t
spif_socket_new_from_url(spif_url_t url)
{
    spif_socket_t self;

    self = SPIF_ALLOC(socket);
    spif_socket_init_from_url(self, url);
    return self;
}

spif_bool_t
spif_socket_del(spif_socket_t self)
{
    spif_socket_done(self);
    SPIF_DEALLOC(self);
    return TRUE;
}

spif_bool_t
spif_socket_init(spif_socket_t self)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(socket));
    return TRUE;
}

spif_bool_t
spif_socket_init_from_url(spif_socket_t self, spif_url_t url)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(socket));
    return TRUE;
}

spif_bool_t
spif_socket_done(spif_socket_t self)
{
    USE_VAR(self);
    return TRUE;
}

spif_str_t
spif_socket_show(spif_socket_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];

    if (SPIF_SOCKET_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL("socket", name, buff, indent);
        return buff;
    }
        
    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_socket_t) %s:  {\n", name);
    if (SPIF_STR_ISNULL(buff)) {
        buff = spif_str_new_from_ptr(tmp);
    } else {
        spif_str_append_from_ptr(buff, tmp);
    }

    indent += 2;
    memset(tmp, ' ', indent);
    snprintf(tmp + indent, sizeof(tmp) - indent, "(int) fd:  %d\n", self->fd);
    spif_str_append_from_ptr(buff, tmp);

    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_uint32_t) flags:  %lu\n", (unsigned long) self->flags);
    spif_str_append_from_ptr(buff, tmp);

    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_sockaddr_t) addr:  %8p\n", self->addr);
    spif_str_append_from_ptr(buff, tmp);

    spif_url_show(self->src_url, "src_url", buff, indent);
    spif_url_show(self->dest_url, "dest_url", buff, indent);
    spif_str_show(self->input, "input", buff, indent);
    spif_str_show(self->output, "output", buff, indent);

    indent -= 2;
    snprintf(tmp + indent, sizeof(tmp) - indent, "}\n");
    spif_str_append_from_ptr(buff, tmp);

    return buff;
}

spif_cmp_t
spif_socket_comp(spif_socket_t self, spif_socket_t other)
{
    return (self->fd == other->fd);
}

spif_socket_t
spif_socket_dup(spif_socket_t self)
{
    spif_socket_t tmp;

    tmp = spif_socket_new();
    memcpy(tmp, self, SPIF_SIZEOF_TYPE(socket));
    return tmp;
}

spif_classname_t
spif_socket_type(spif_socket_t self)
{
    return SPIF_OBJ_CLASSNAME(self);
}

spif_bool_t
spif_socket_open(spif_socket_t self)
{
    REQUIRE_RVAL(!SPIF_SOCKET_ISNULL(self), 0);

    if (!(self->addr)) {
        spif_socket_get_proto(self);

        if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_INET)) {
            self->fam = AF_INET;
            self->addr = SPIF_CAST(sockaddr) spif_url_get_ipaddr(self->dest_url);
        } else if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_UNIX)) {
            self->fam = AF_UNIX;
            self->addr = SPIF_CAST(sockaddr) spif_url_get_unixaddr(self->dest_url);
        } else {
            D_OBJ(("Unknown socket family 0x%08x!\n", SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_FAMILY)));
            ASSERT_NOTREACHED_RVAL(FALSE);
        }
    }

    if (self->fd < 0) {
        if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_TYPE_STREAM)) {
            self->type = SOCK_STREAM;
        } else if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_TYPE_DGRAM)) {
            self->type = SOCK_DGRAM;
        } else if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_TYPE_RAW)) {
            self->type = SOCK_RAW;
        } else {
            D_OBJ(("Unknown socket type 0x%08x!\n", SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_TYPE)));
            ASSERT_NOTREACHED_RVAL(FALSE);
        }

        self->fd = SPIF_CAST(sockfd) socket(self->fam, self->type, self->proto);
        if (self->fd < 0) {
            print_error("Unable to create socket(%d, %d, %d) -- %s\n", (int) self->fam,
                        (int) self->type, (int) self->proto, strerror(errno));
            return FALSE;
        }

        if (!SPIF_URL_ISNULL(self->src_url)) {
            if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_INET)) {
                spif_ipsockaddr_t addr;

                addr = spif_url_get_ipaddr(self->src_url);
                addr->sin_port = spif_url_get_portnum(self->src_url);

                if (bind(self->fd, SPIF_CAST(sockaddr) addr, SPIF_SIZEOF_TYPE(ipsockaddr))) {
                    print_error("Unable to bind socket %d to %s -- %s\n", (int) self->fd,
                                SPIF_STR_STR(self->src_url), strerror(errno));
                    FREE(addr);
                    return FALSE;
                }
                FREE(addr);
            }
        }
    }

    return TRUE;
}

static spif_ipsockaddr_t
spif_url_get_ipaddr(spif_url_t self)
{
    char *ipaddr_str;
    spif_uint8_t tries;
    spif_hostinfo_t hinfo;
    spif_ipsockaddr_t addr;
    spif_str_t hostname;

    REQUIRE_RVAL(!SPIF_URL_ISNULL(self), 0);

    hostname = SPIF_STR(spif_url_get_host(self));
    REQUIRE_RVAL(!SPIF_STR_ISNULL(hostname), 0);

    h_errno = 0;
    tries = 0;
    do {
        tries++;
        hinfo = gethostbyname(SPIF_STR_STR(hostname));
    } while ((tries <= 3) && (hinfo == NULL) && (h_errno == TRY_AGAIN));
    if (hinfo == NULL) {
        print_error("Unable to resolve hostname \"%s\" -- %s\n", SPIF_STR_STR(hostname), hstrerror(h_errno));
        return 0;
    }

    ipaddr_str = hinfo->h_addr_list[0];
    addr = SPIF_ALLOC(ipsockaddr);
    addr->sin_family = AF_INET;
    addr->sin_port = 0;
    if ((inet_aton(ipaddr_str, &(addr->sin_addr))) == 0) {
        print_error("Invalid address \"%s\" returned by gethostbyname()\n", NONULL(ipaddr_str));
        return 0;
    }
    return addr;
}

static spif_unixsockaddr_t
spif_url_get_unixaddr(spif_url_t self)
{
    spif_unixsockaddr_t addr;

    addr = SPIF_ALLOC(unixsockaddr);
    addr->sun_family = AF_UNIX;
    addr->sun_path[0] = 0;
    strncat(addr->sun_path, SPIF_STR_STR(spif_url_get_path(self)), sizeof(addr->sun_path));
    return addr;
}

static spif_sockport_t
spif_url_get_portnum(spif_url_t self)
{
    spif_str_t port_str;

    port_str = spif_url_get_port(self);
    if (!SPIF_STR_ISNULL(port_str)) {
        return SPIF_CAST(sockport) spif_str_to_num(port_str, 10);
    }

    return SPIF_CAST(sockport) 0;
}

static spif_bool_t
spif_socket_get_proto(spif_socket_t self)
{
    spif_url_t url;
    spif_protoinfo_t proto;
    spif_str_t proto_str;
    spif_servinfo_t serv;

    url = ((SPIF_URL_ISNULL(self->dest_url)) ? (self->src_url) : (self->dest_url));

    proto_str = spif_url_get_proto(url);
    if (!SPIF_STR_ISNULL(proto_str)) {
        if (SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(proto_str, "raw"))) {
            spif_str_t target;

            SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_TYPE_RAW);

            target = spif_url_get_host(url);
            if (SPIF_STR_ISNULL(target)) {
                target = spif_url_get_path(url);
                if (!SPIF_STR_ISNULL(target)) {
                    SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_UNIX);
                }
            } else {
                SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_INET);
            }
        } else {
            proto = getprotobyname(SPIF_STR_STR(proto_str));
            if (proto == NULL) {
                /* If it's not a protocol, it's probably a service. */
                serv = getservbyname(SPIF_STR_STR(proto_str), "tcp");
                if (serv == NULL) {
                    serv = getservbyname(SPIF_STR_STR(proto_str), "udp");
                }
                if (serv != NULL) {
                    proto = getprotobyname(serv->s_proto);
                    REQUIRE_RVAL(proto != NULL, FALSE);
                    self->port = serv->s_port;
                }
            }
            self->proto = proto->p_proto;
            if (!strcmp((char *) proto, "unix")) {
                SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_UNIX);
            } else {
                SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_INET);
                if (!strcmp((char *) proto, "tcp")) {
                    SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_TYPE_STREAM);
                } else if (!strcmp((char *) proto, "udp")) {
                    SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_TYPE_DGRAM);
                }
            }
        }
    }

    return TRUE;
}
