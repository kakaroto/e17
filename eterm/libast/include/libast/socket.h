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

#ifndef _LIBAST_SOCKET_H_
#define _LIBAST_SOCKET_H_

/* Cast an arbitrary object pointer to a socket. */
#define SPIF_SOCKET(o)                      ((spif_socket_t) (socket))

/* Check to see if a pointer references an socket. */
#define SPIF_OBJ_IS_SOCKET(o)               (SPIF_OBJ_IS_TYPE(o, socket))

/* Used for testing the NULL-ness of sockets. */
#define SPIF_SOCKET_ISNULL(o)               (SPIF_SOCKET(o) == SPIF_NULL_TYPE(socket))

/* Calls to the basic functions. */
#define SPIF_SOCKET_NEW()                   SPIF_CAST(socket) (SPIF_CLASS(SPIF_CLASS_VAR(socket)))->(noo)()
#define SPIF_SOCKET_INIT(o)                 SPIF_OBJ_INIT(o)
#define SPIF_SOCKET_DONE(o)                 SPIF_OBJ_DONE(o)
#define SPIF_SOCKET_DEL(o)                  SPIF_OBJ_DEL(o)
#define SPIF_SOCKET_SHOW(o, b, i)           SPIF_OBJ_SHOW(o, b, i)
#define SPIF_SOCKET_COMP(o1, o2)            SPIF_OBJ_COMP(o)
#define SPIF_SOCKET_DUP(o)                  SPIF_OBJ_DUP(o)
#define SPIF_SOCKET_TYPE(o)                 SPIF_OBJ_TYPE(o)

typedef struct spif_socket_t_struct *spif_socket_t;
typedef struct spif_socket_t_struct spif_const_socket_t;

struct spif_socket_t_struct {
    spif_const_obj_t parent;
    int fd;
    int flags;
    spif_str_t input, output;
};

extern spif_class_t SPIF_CLASS_VAR(socket);
extern spif_socket_t spif_socket_new(void);
extern spif_bool_t spif_socket_del(spif_socket_t);
extern spif_bool_t spif_socket_init(spif_socket_t);
extern spif_bool_t spif_socket_done(spif_socket_t);
extern spif_str_t spif_socket_show(spif_socket_t, spif_charptr_t, spif_str_t, size_t);
extern spif_cmp_t spif_socket_comp(spif_socket_t, spif_socket_t);
extern spif_socket_t spif_socket_dup(spif_socket_t);
extern spif_classname_t spif_socket_type(spif_socket_t);

#endif /* _LIBAST_SOCKET_H_ */
