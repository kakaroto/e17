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
static SPIF_CONST_TYPE(class) s_class = {
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
SPIF_TYPE(class) SPIF_CLASS_VAR(socket) = &s_class;
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
spif_socket_new_from_urls(spif_url_t surl, spif_url_t durl)
{
    spif_socket_t self;

    self = SPIF_ALLOC(socket);
    spif_socket_init_from_urls(self, surl, durl);
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
    self->fd = -1;
    self->fam = AF_INET;
    self->type = SOCK_STREAM;
    self->proto = 0;
    self->addr = SPIF_NULL_TYPE(sockaddr);
    self->len = 0;
    self->flags = 0;
    self->src_url = SPIF_NULL_TYPE(url);
    self->dest_url = SPIF_NULL_TYPE(url);
    self->input = SPIF_NULL_TYPE(str);
    self->output = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_socket_init_from_urls(spif_socket_t self, spif_url_t surl, spif_url_t durl)
{
    spif_obj_init(SPIF_OBJ(self));
    spif_obj_set_class(SPIF_OBJ(self), SPIF_CLASS_VAR(socket));
    self->fd = -1;
    self->fam = AF_INET;
    self->type = SOCK_STREAM;
    self->proto = 0;
    self->addr = SPIF_NULL_TYPE(sockaddr);
    self->len = 0;
    self->flags = 0;
    if (!SPIF_URL_ISNULL(surl)) {
        self->src_url = spif_url_dup(surl);
    } else {
        self->src_url = SPIF_NULL_TYPE(url);
    }
    if (!SPIF_URL_ISNULL(durl)) {
        self->dest_url = spif_url_dup(durl);
    } else {
        self->dest_url = SPIF_NULL_TYPE(url);
    }
    self->input = SPIF_NULL_TYPE(str);
    self->output = SPIF_NULL_TYPE(str);
    return TRUE;
}

spif_bool_t
spif_socket_done(spif_socket_t self)
{
    if (self->fd >= 0) {
        if ((close(self->fd)) < 0) {
            print_error("Unable to close socket %d -- %s\n", self->fd, strerror(errno));
        }
        self->fd = -1;
    }
    self->fam = AF_INET;
    self->type = SOCK_STREAM;
    self->proto = 0;
    if (self->addr != SPIF_NULL_TYPE(sockaddr)) {
        SPIF_DEALLOC(self->addr);
        self->addr = SPIF_NULL_TYPE(sockaddr);
    }
    self->len = 0;
    self->flags = 0;
    if (!SPIF_URL_ISNULL(self->src_url)) {
        spif_url_del(self->src_url);
        self->src_url = SPIF_NULL_TYPE(url);
    }
    if (!SPIF_URL_ISNULL(self->dest_url)) {
        spif_url_del(self->dest_url);
        self->dest_url = SPIF_NULL_TYPE(url);
    }
    if (!SPIF_STR_ISNULL(self->input)) {
        spif_str_del(self->input);
        self->input = SPIF_NULL_TYPE(str);
    }
    if (!SPIF_STR_ISNULL(self->output)) {
        spif_str_del(self->output);
        self->output = SPIF_NULL_TYPE(str);
    }
    return TRUE;
}

spif_str_t
spif_socket_show(spif_socket_t self, spif_charptr_t name, spif_str_t buff, size_t indent)
{
    char tmp[4096];

    if (SPIF_SOCKET_ISNULL(self)) {
        SPIF_OBJ_SHOW_NULL(socket, name, buff, indent);
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
    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_sockfd_t) fd:  %d\n", self->fd);
    spif_str_append_from_ptr(buff, tmp);

    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_sockfamily_t) fam:  %d\n", (int) self->fam);
    spif_str_append_from_ptr(buff, tmp);

    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_socktype_t) type:  %d\n", (int) self->type);
    spif_str_append_from_ptr(buff, tmp);

    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_sockproto_t) proto:  %d\n", (int) self->proto);
    spif_str_append_from_ptr(buff, tmp);

    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_sockaddr_t) addr:  %8p\n", self->addr);
    spif_str_append_from_ptr(buff, tmp);

    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_sockaddr_len_t) len:  %lu\n", (unsigned long) self->len);
    spif_str_append_from_ptr(buff, tmp);

    snprintf(tmp + indent, sizeof(tmp) - indent, "(spif_uint32_t) flags:  0x%08x\n", (unsigned) self->flags);
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
    if (self->fd >= 0) {
        tmp->fd = dup(self->fd);
    }
    tmp->fam = self->fam;
    tmp->type = self->type;
    tmp->proto = self->proto;
    tmp->len = self->len;
    if (self->addr != SPIF_NULL_TYPE(sockaddr)) {
        tmp->addr = SPIF_CAST(sockaddr) MALLOC(tmp->len);
        memcpy(tmp->addr, self->addr, tmp->len);
    }
    tmp->flags = self->flags;
    if (!SPIF_URL_ISNULL(self->src_url)) {
        tmp->src_url = spif_url_dup(self->src_url);
    }
    if (!SPIF_URL_ISNULL(self->dest_url)) {
        tmp->dest_url = spif_url_dup(self->dest_url);
    }
    tmp->input = SPIF_NULL_TYPE(str);
    tmp->output = SPIF_NULL_TYPE(str);
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
    REQUIRE_RVAL(!SPIF_SOCKET_ISNULL(self), FALSE);

    if (!(self->addr)) {
        /* Set family, protocol, and type flags. */
        spif_socket_get_proto(self);

        /* Get destination address, if we have one. */
        if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_INET)) {
            self->fam = AF_INET;
            self->addr = SPIF_CAST(sockaddr) spif_url_get_ipaddr(self->dest_url);
            self->len = SPIF_SIZEOF_TYPE(ipsockaddr);
        } else if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_UNIX)) {
            self->fam = AF_UNIX;
            self->addr = SPIF_CAST(sockaddr) spif_url_get_unixaddr(self->dest_url);
            self->len = SPIF_SIZEOF_TYPE(unixsockaddr);
        } else {
            D_OBJ(("Unknown socket family 0x%08x!\n", SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_FAMILY)));
            ASSERT_NOTREACHED_RVAL(FALSE);
        }
    }

    /* If the socket isn't open yet, open it and get a file descriptor. */
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

        /* If we have a source URL, bind it to the appropriate address and port. */
        if (!SPIF_URL_ISNULL(self->src_url)) {
            if (SPIF_SOCKET_FLAGS_IS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_INET)) {
                spif_ipsockaddr_t addr;

                addr = spif_url_get_ipaddr(self->src_url);
                addr->sin_port = htonl(spif_url_get_portnum(self->src_url));

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

    /* Connect if we have a destination.  If not, open a listening socket. */
    if (!SPIF_URL_ISNULL(self->dest_url)) {
        spif_socket_clear_nbio(self);
        if ((connect(self->fd, self->addr, self->len)) < 0) {
            print_error("Unable to connect socket %d to %s -- %s\n", (int) self->fd,
                        SPIF_STR_STR(self->dest_url), strerror(errno));
            return FALSE;
        }
    } else if (!SPIF_URL_ISNULL(self->src_url)) {
        if ((listen(self->fd, 5)) < 0) {
            print_error("Unable to listen at %s on socket %d -- %s\n", 
                        SPIF_STR_STR(self->src_url), (int) self->fd, strerror(errno));
            return FALSE;
        }
    }

    return TRUE;
}

spif_bool_t
spif_socket_check_io(spif_socket_t self)
{
    static struct timeval tv = { 0, 0 };
    fd_set read_fds, write_fds;

    REQUIRE_RVAL(!SPIF_SOCKET_ISNULL(self), FALSE);
    REQUIRE_RVAL(self->fd >= 0, FALSE);

    FD_ZERO(&read_fds);
    FD_SET(self->fd, &read_fds);
    FD_ZERO(&write_fds);
    FD_SET(self->fd, &write_fds);
    if ((select(self->fd + 1, &read_fds, &write_fds, NULL, &tv)) < 0) {
        print_error("Unable to select() on %d -- %s\n", self->fd, strerror(errno));
        return FALSE;
    }

    if (FD_ISSET(self->fd, &read_fds)) {
        SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_HAVE_INPUT);
    } else {
        SPIF_SOCKET_FLAGS_CLEAR(self, SPIF_SOCKET_FLAGS_HAVE_INPUT);
    }
    if (FD_ISSET(self->fd, &write_fds)) {
        SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_CAN_OUTPUT);
    } else {
        SPIF_SOCKET_FLAGS_CLEAR(self, SPIF_SOCKET_FLAGS_CAN_OUTPUT);
    }
    return TRUE;
}

spif_bool_t
spif_socket_send(spif_socket_t self, spif_str_t data)
{
    size_t len;
    int num_written;
    struct timeval tv = { 0, 0 };

    REQUIRE_RVAL(!SPIF_SOCKET_ISNULL(self), FALSE);
    REQUIRE_RVAL(!SPIF_STR_ISNULL(data), FALSE);

    len = spif_str_get_len(data);
    REQUIRE_RVAL(len > 0, FALSE);

    num_written = write(self->fd, SPIF_STR_STR(data), len);
    for (; (num_written < 0) && ((errno == EAGAIN) || (errno == EINTR)); ) {
        tv.tv_usec += 10000;
        if (tv.tv_usec == 1000000) {
            tv.tv_usec = 0;
            tv.tv_sec++;
        }
        select(0, NULL, NULL, NULL, &tv);
        num_written = write(self->fd, SPIF_STR_STR(data), len);
    }
    if (num_written < 0) {
        D_OBJ(("Unable to write to socket %d -- %s\n", self->fd, strerror(errno)));
        switch (errno) {
        case EFBIG:
            {
                spif_bool_t b;
                spif_str_t tmp_buf;
                char *s;
                long left;

                for (left = len, s = SPIF_STR_STR(data); left > 0; s += 1024, left -= 1024) {
                    tmp_buf = spif_str_new_from_buff(s, 1024);
                    b = spif_socket_send(self, tmp_buf);
                    if (b == FALSE) {
                        spif_str_del(tmp_buf);
                        return b;
                    }
                }
            }
            break;
        case EIO:
        case EPIPE:
            close(self->fd);
            /* Drop */
        case EBADF:
        case EINVAL:
        default:
            self->fd = -1;
            return FALSE;
            break;

        }
    }
    return TRUE;
}

spif_str_t
spif_socket_recv(spif_socket_t self)
{

}

spif_bool_t
spif_socket_set_nbio(spif_socket_t self)
{
    int flags;

    REQUIRE_RVAL(self->fd >= 0, FALSE);

    /* Set the socket options for non-blocking I/O. */
#if defined(O_NDELAY)
    flags = fcntl(self->fd, F_GETFL, 0);
    if (flags < 0) {
        flags = O_NDELAY;
    } else {
        flags |= O_NDELAY;
    }
    if ((fcntl(self->fd, F_SETFL, flags)) != 0) {
        return FALSE;
    } else {
        return TRUE;
    }
#elif defined(O_NONBLOCK)
    flags = fcntl(self->fd, F_GETFL, 0);
    if (flags < 0) {
        flags = O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    if ((fcntl(self->fd, F_SETFL, flags)) != 0) {
        return FALSE;
    } else {
        return TRUE;
    }
#else
    flags = 1;
    if ((ioctl(fd, FIONBIO, &flags)) != 0) {
        return FALSE;
    } else {
        return TRUE;
    }
#endif
}     

spif_bool_t
spif_socket_clear_nbio(spif_socket_t self)
{
    int flags;

    REQUIRE_RVAL(self->fd >= 0, FALSE);

    /* Set the socket options for blocking I/O. */
#if defined(O_NDELAY)
    flags = fcntl(self->fd, F_GETFL, 0);
    if (flags < 0) {
        flags = 0;
    } else {
        flags &= ~(O_NDELAY);
    }
    if ((fcntl(self->fd, F_SETFL, flags)) != 0) {
        return FALSE;
    } else {
        return TRUE;
    }
#elif defined(O_NONBLOCK)
    flags = fcntl(self->fd, F_GETFL, 0);
    if (flags < 0) {
        flags = 0;
    } else {
        flags &= ~(O_NONBLOCK);
    }
    if ((fcntl(self->fd, F_SETFL, flags)) != 0) {
        return FALSE;
    } else {
        return TRUE;
    }
#else
    flags = 0;
    if ((ioctl(fd, FIONBIO, &flags)) != 0) {
        return FALSE;
    } else {
        return TRUE;
    }
#endif
}     

/**************** Static internal functions ****************/

static spif_ipsockaddr_t
spif_url_get_ipaddr(spif_url_t self)
{
    spif_uint8_t tries;
    spif_hostinfo_t hinfo;
    spif_ipsockaddr_t addr;
    spif_str_t hostname;

    REQUIRE_RVAL(!SPIF_URL_ISNULL(self), 0);

    /* We need a hostname of some type to connect to. */
    hostname = SPIF_STR(spif_url_get_host(self));
    REQUIRE_RVAL(!SPIF_STR_ISNULL(hostname), 0);

    /* Try up to 3 times to resolve the hostname. */
    h_errno = 0;
    tries = 0;
    do {
        tries++;
        hinfo = gethostbyname(SPIF_STR_STR(hostname));
    } while ((tries <= 3) && (hinfo == NULL) && (h_errno == TRY_AGAIN));
    if (hinfo == NULL) {
        print_error("Unable to resolve hostname \"%s\" -- %s\n", SPIF_STR_STR(hostname), hstrerror(h_errno));
        return SPIF_NULL_TYPE(ipsockaddr);
    }

    if (hinfo->h_addr_list == NULL) {
        print_error("Invalid address list returned by gethostbyname()\n");
        return SPIF_NULL_TYPE(ipsockaddr);
    }

    /* Grab the host IP address and port number, both in network byte order. */
    addr = SPIF_ALLOC(ipsockaddr);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(spif_url_get_portnum(self));
    memcpy(&(addr->sin_addr), (void *) (hinfo->h_addr_list[0]), sizeof(addr->sin_addr));
    D_OBJ(("Got address 0x%08x and port %d for name \"%s\"\n", *((int *) (&addr->sin_addr)),
           (int) ntohs(addr->sin_port), SPIF_STR_STR(hostname)));
    return addr;
}

static spif_unixsockaddr_t
spif_url_get_unixaddr(spif_url_t self)
{
    spif_unixsockaddr_t addr;

    /* No address to look up, just a file path. */
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

    /* Return the integer form of the port number for a URL */
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

    /* If we have a destination URL, use it.  Otherwise, use the source. */
    url = ((SPIF_URL_ISNULL(self->dest_url)) ? (self->src_url) : (self->dest_url));
    REQUIRE_RVAL(!SPIF_URL_ISNULL(url), FALSE);

    proto_str = spif_url_get_proto(url);
    if (!SPIF_STR_ISNULL(proto_str)) {
        if (SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(proto_str, "raw"))) {
            spif_str_t target;

            /* Raw socket.  Could be raw UNIX or raw IP. */
            SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_TYPE_RAW);

            /* If we have a hostname, it's IP.  If we have a path only, it's UNIX. */
            target = spif_url_get_host(url);
            if (SPIF_STR_ISNULL(target)) {
                target = spif_url_get_path(url);
                if (!SPIF_STR_ISNULL(target)) {
                    SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_UNIX);
                }
            } else {
                SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_INET);
            }
        } else if (SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(proto_str, "unix"))) {
            /* UNIX socket.  Assume stream-based. */
            SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_UNIX);
            SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_TYPE_STREAM);
        } else {
            /* IP socket.  See if they gave us a protocol name. */
            SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_INET);
            proto = getprotobyname(SPIF_STR_STR(proto_str));
            if (proto == NULL) {
                /* If it's not a protocol, it's probably a service. */
                serv = getservbyname(SPIF_STR_STR(proto_str), "tcp");
                if (serv == NULL) {
                    serv = getservbyname(SPIF_STR_STR(proto_str), "udp");
                }
                if (serv != NULL) {
                    /* If we found one, get the protocol info too. */
                    proto = getprotobyname(serv->s_proto);
                    REQUIRE_RVAL(proto != NULL, FALSE);
                }
            }
            if (proto != NULL) {
                /* Bingo.  Set the flags appropriately. */
                self->proto = proto->p_proto;
                if (!strcmp(proto->p_name, "tcp")) {
                    SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_TYPE_STREAM);
                } else if (!strcmp(proto->p_name, "udp")) {
                    SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_TYPE_DGRAM);
                }
            }
        }
    } else {
        /* No protocol?  Assume a UNIX socket. */
        SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_FAMILY_UNIX);
        SPIF_SOCKET_FLAGS_SET(self, SPIF_SOCKET_FLAGS_TYPE_STREAM);
    }
    return TRUE;
}

