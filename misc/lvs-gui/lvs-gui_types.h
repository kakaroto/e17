/**********************************************************************
 * ipvs-gui_types.h
 *
 * Common types, defines and includes for GUI components
 *
 * Copyright (C) 1999 Carsten Haitzler and Simon Horman
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies of the Software, its documentation and
 * marketing & publicity materials, and acknowledgment shall be given
 * in the documentation, materials and software packages that this
 * Software was used.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************/

#ifndef IPVS_GUI_TYPES_FLIM
#define IPVS_GUI_TYPES_FLIM

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#define CONNECT(ob, sig, func, dat) \
gtk_signal_connect(GTK_OBJECT(ob), sig, GTK_SIGNAL_FUNC(func), dat);

#define METHOD_WLC 0
#define METHOD_WRR 1
#define METHOD_RR  2
#define METHOD_LC  3

#define PROTO_TCP          0
#define PROTO_TCP_PERSIST  1
#define PROTO_UDP          2

#define IP_ADDR_NEXT      "0.0.0.0.next"
#define IP_ADDR_CURRENT   "0.0.0.0.current"
#define IP_DEFAULT        "127.0.0.1"
#define PORT_CURRENT      "0"
#define PORT_DEFAULT      "80"
#define WEIGHT_CURRENT    0
#define WEIGHT_DEFAULT    1


/* Encapsulation of a single advertised service */

typedef struct {
   gchar *address;
   gchar *port;
   GList *back_end_servers;
   GList *ipvs_servers;
   gint   method;
   gint   protocol;
} advertised_service;


/* Encapsulation of a single IPVS server*/

typedef struct {
   gchar *address;
} ipvs_server;


/* Encapsulation of a single back-end server*/

typedef struct {
   gchar *address;
   gchar *port;
   gint   weight;
} back_end_server;

#endif
