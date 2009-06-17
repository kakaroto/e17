/**********************************************************************
 * util.h
 *
 * Utility GUI funtions
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

#ifndef IPVS_GUI_UTIL_FLIM
#define IPVS_GUI_UTIL_FLIM

#include <string.h>
#include "lvs-gui_types.h"

void del_advertised_service(advertised_service *a);
void del_ipvs_server(advertised_service *t, ipvs_server *t2);
void del_back_end_server(advertised_service *t, back_end_server *t2);

advertised_service *add_advertised_service(
  char *addr, 
  char *port, 
  int method, 
  int protocol
);
void add_back_end_server(
  advertised_service *a, 
  char *addr, 
  char *port, 
  int weight
);
void add_ipvs_server(advertised_service *a, char *addr);

void fill_back_end_server_clist(advertised_service *a);
void fill_ipvs_server_clist(advertised_service *a);
void fill_clist(advertised_service *a);

void update_advertised_service(advertised_service *a);
void update_back_end_server(advertised_service *a, back_end_server *be);
void update_ipvs_server(advertised_service *a, ipvs_server *ipvs);

gchar *ip_addr_next(gchar *addr);

#endif
