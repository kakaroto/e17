/**********************************************************************
 * callback.h
 *
 * Callback funtions for GUI
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
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************/

#ifndef CALLBACK_FLIM
#define CALLBACK_FLIM

#include "lvs-gui_types.h"

void cb_main_close(GtkWidget *w, void *data);
void cb_close(GtkWidget * w, void *data);
void cb_external_service_addr_change(GtkEntry *entry, gpointer user_data);
void cb_external_service_port_change(GtkEntry *entry, gpointer user_data);
void cb_back_end_server_addr_change(GtkEntry *entry, gpointer user_data);
void cb_back_end_server_port_change(GtkEntry *entry, gpointer user_data);
void cb_back_end_server_weight_change(GtkEntry *entry, gpointer user_data);
void cb_ipvs_server_addr_change(GtkEntry *entry, gpointer user_data);
void cb_sched_wlc_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data);
void cb_sched_wrr_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data);
void cb_sched_lc_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data);
void cb_sched_rr_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data);
void cb_proto_tcp_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data);
void cb_proto_persistent_tcp_radio_toggle(
  GtkRadioButton *radiobutton, 
  gpointer user_data
);
void cb_proto_udp_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data);
void cb_advertised_service_select(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
);
void cb_advertised_service_unselect(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
);
void cb_back_end_server_select(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
);
void cb_back_end_server_unselect(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
);
void cb_ipvs_server_select(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
);
void cb_ipvs_server_unselect(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
);
void cb_add_advertised_service(GtkButton *button, gpointer user_data);
void cb_del_advertised_service(GtkButton *button, gpointer user_data);
void cb_add_back_end_server(GtkButton *button, gpointer user_data);
void cb_del_back_end_server(GtkButton *button, gpointer user_data);
void cb_add_ipvs_server(GtkButton *button, gpointer user_data);
void cb_del_ipvs_server(GtkButton *button, gpointer user_data);
void cb_apply(GtkButton *button, gpointer user_data);
void cb_ok(GtkButton *button, gpointer user_data);
void cb_cancel(GtkButton *button, gpointer user_data);
void cb_reload(GtkButton *button, gpointer user_data);
void cb_options_change(GtkEntry *entry, gpointer user_data);
void cb_options_close(GtkButton *button, gpointer user_data);
void cb_about(GtkButton *button, gpointer user_data);
void cb_options(GtkButton *button, gpointer user_data);

#endif
