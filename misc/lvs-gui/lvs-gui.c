/**********************************************************************
 * ipvs-gui.c
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

#include "lvs-gui.h"
#include "callback.h"

/* globals */
GtkWidget *advertised_address_frame;
GtkWidget *advertised_service_add;
GtkWidget *advertised_service_addr;
GtkWidget *advertised_service_clist;
GtkWidget *advertised_service_del;
GtkWidget *advertised_service_port;
GtkWidget *back_end_server_add;
GtkWidget *back_end_server_addr;
GtkWidget *back_end_server_clist;
GtkWidget *back_end_server_del;
GtkWidget *back_end_server_frame;
GtkWidget *back_end_server_port;
GtkWidget *back_end_server_weight;
GtkWidget *cfg_config;
GtkWidget *cfg_reload;
GtkWidget *ipvs_server_add;
GtkWidget *ipvs_server_addr;
GtkWidget *ipvs_server_clist;
GtkWidget *ipvs_server_del;
GtkWidget *ipvs_server_frame;
GtkWidget *proto_persistent_tcp_radio;
GtkWidget *proto_tcp_radio;
GtkWidget *proto_udp_radio;
GtkWidget *sched_lc_radio;
GtkWidget *sched_rr_radio;
GtkWidget *sched_wlc_radio;
GtkWidget *sched_wrr_radio;
GtkWidget *service_apply;
GtkWidget *service_cancel;
GtkWidget *service_ok;

GList                *service                        = NULL;
ipvs_server          *current_ipvs_server         = NULL;
back_end_server      *current_back_end_server     = NULL;
advertised_service   *current_advertised_service  = NULL;


/* gui setup */
void
gui_tcp(GtkWidget *win)
{
   GtkWidget *hbox, *hbox2, *vbox, *vport, *list, *frame, *align, *button;
   GtkWidget *hbox3, *entry, *radio, *vbox2;
   gchar *advertised_service_clist_titles[] = {
      "External Address", 
      "Port", 
      "Scheduling Method", 
      "Protocol"
   };
   gchar *ipvs_server_clist_titles[] = {"Internal Address"};
   gchar *back_end_server_clist_titles[] = {
     "Internal Address", 
     "Port", 
     "Weight"
   };


   /*External servers gui setup*/
   hbox = gtk_vbox_new(FALSE, 2);
   gtk_container_add(GTK_CONTAINER(win), hbox);

   frame = gtk_frame_new("Externally advertised services - Floating Addresses");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 2);

   hbox2 = gtk_hbox_new(FALSE, 2);
   gtk_container_border_width(GTK_CONTAINER(hbox2), 4);
   gtk_container_add(GTK_CONTAINER(frame), hbox2);

   align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox2), align, FALSE, FALSE, 0);
   vbox = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(align), vbox);

   advertised_service_add = button = gtk_button_new_with_label("Add");
   gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

   advertised_service_del = button = gtk_button_new_with_label("Del");
   gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

   vbox = gtk_vbox_new(FALSE, 0);
   gtk_box_pack_start(GTK_BOX(hbox2), vbox, TRUE, TRUE, 0);

   vport = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(vport), 
        			  GTK_POLICY_AUTOMATIC,
        			  GTK_POLICY_AUTOMATIC);
   gtk_widget_set_usize(vport, 400, 64);
   gtk_box_pack_start(GTK_BOX(vbox), vport, TRUE, TRUE, 0);

   ipvs_server_frame = hbox3 = gtk_hbox_new(FALSE, 2);
   gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 2);

   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox3), align, TRUE, TRUE, 2);
   frame = gtk_frame_new("External address");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_container_add(GTK_CONTAINER(align), frame);
   advertised_service_addr = entry = gtk_entry_new();
   gtk_widget_set_usize(entry, 80, -1);
   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_container_add(GTK_CONTAINER(align), entry);
   gtk_container_add(GTK_CONTAINER(frame), align);

   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox3), align, FALSE, FALSE, 2);
   frame = gtk_frame_new("Port");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_container_add(GTK_CONTAINER(align), frame);
   advertised_service_port = entry = gtk_entry_new();
   gtk_widget_set_usize(entry, 32, -1);
   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_container_add(GTK_CONTAINER(align), entry);
   gtk_container_add(GTK_CONTAINER(frame), align);

   align = gtk_alignment_new(0.0, 0.0, 1.0, 1.0);
   gtk_box_pack_start(GTK_BOX(hbox3), align, FALSE, FALSE, 2);
   frame = gtk_frame_new("Connection Scheduling Method");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_container_add(GTK_CONTAINER(align), frame);
   vbox2 = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(frame), vbox2);

   sched_wlc_radio = radio = gtk_radio_button_new_with_label_from_widget(NULL, 
      "Weighted least connection");
   gtk_box_pack_start(GTK_BOX(vbox2), radio, FALSE, FALSE, 0);

   sched_wrr_radio = radio = gtk_radio_button_new_with_label_from_widget(
      GTK_RADIO_BUTTON(radio), "Weighted round robin");
   gtk_box_pack_start(GTK_BOX(vbox2), radio, FALSE, FALSE, 0);

   sched_lc_radio = radio = gtk_radio_button_new_with_label_from_widget(
      GTK_RADIO_BUTTON(radio), "Round robin");
   gtk_box_pack_start(GTK_BOX(vbox2), radio, FALSE, FALSE, 0);

   sched_rr_radio = radio = gtk_radio_button_new_with_label_from_widget(
      GTK_RADIO_BUTTON(radio), "Least connection");
   gtk_box_pack_start(GTK_BOX(vbox2), radio, FALSE, FALSE, 0);

   align = gtk_alignment_new(0.0, 0.0, 1.0, 1.0);
   gtk_box_pack_start(GTK_BOX(hbox3), align, FALSE, FALSE, 2);
   frame = gtk_frame_new("Protocol");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_container_add(GTK_CONTAINER(align), frame);
   vbox2 = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(frame), vbox2);

   proto_tcp_radio = radio = gtk_radio_button_new_with_label_from_widget(
     NULL, 
     "TCP"
   );
   gtk_box_pack_start(GTK_BOX(vbox2), radio, FALSE, FALSE, 0);

   proto_persistent_tcp_radio = radio = gtk_radio_button_new_with_label_from_widget(
      GTK_RADIO_BUTTON(radio), 
      "Persistant TCP"
   );
   gtk_box_pack_start(GTK_BOX(vbox2), radio, FALSE, FALSE, 0);

   proto_udp_radio = radio = gtk_radio_button_new_with_label_from_widget(
      GTK_RADIO_BUTTON(radio), "UDP");
   gtk_box_pack_start(GTK_BOX(vbox2), radio, FALSE, FALSE, 0);

   advertised_service_clist = list = gtk_clist_new_with_titles(
     4, 
     advertised_service_clist_titles
   );
   gtk_clist_set_column_resizeable(GTK_CLIST(list), 0, TRUE);
   gtk_clist_set_column_resizeable(GTK_CLIST(list), 1, FALSE);
   gtk_clist_set_column_resizeable(GTK_CLIST(list), 2, FALSE);
   gtk_clist_set_column_resizeable(GTK_CLIST(list), 3, FALSE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(list), 0, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(list), 1, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(list), 2, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(list), 3, TRUE);
   gtk_clist_set_selection_mode(GTK_CLIST(list), GTK_SELECTION_BROWSE);

   gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(vport), list);


   /*IPVS servers GUI*/
   advertised_address_frame = frame = gtk_frame_new(
      "Internal Addresses for IPVS Servers for Selected Advertised Service");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 2);

   hbox2 = gtk_hbox_new(FALSE, 2);
   gtk_container_border_width(GTK_CONTAINER(hbox2), 4);
   gtk_container_add(GTK_CONTAINER(frame), hbox2);

   align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox2), align, FALSE, FALSE, 0);
   vbox = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(align), vbox);

   ipvs_server_add = button = gtk_button_new_with_label("Add");
   gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

   ipvs_server_del = button = gtk_button_new_with_label("Del");
   gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

   vbox = gtk_vbox_new(FALSE, 0);
   gtk_box_pack_start(GTK_BOX(hbox2), vbox, TRUE, TRUE, 0);

   vport = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(vport), 
        			  GTK_POLICY_AUTOMATIC,
        			  GTK_POLICY_AUTOMATIC);
   gtk_widget_set_usize(vport, 400, 64);
   gtk_box_pack_start(GTK_BOX(vbox), vport, TRUE, TRUE, 0);

   ipvs_server_frame = hbox3 = gtk_hbox_new(FALSE, 2);
   gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 2);

   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox3), align, TRUE, TRUE, 2);
   frame = gtk_frame_new("Internal Address");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_container_add(GTK_CONTAINER(align), frame);
   ipvs_server_addr = entry = gtk_entry_new();
   gtk_widget_set_usize(entry, 80, -1);
   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_container_add(GTK_CONTAINER(align), entry);
   gtk_container_add(GTK_CONTAINER(frame), align);

   ipvs_server_clist = list = gtk_clist_new_with_titles(
      1, 
      ipvs_server_clist_titles
   );
   gtk_clist_set_column_resizeable(GTK_CLIST(list), 0, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(list), 0, TRUE);
   gtk_clist_set_selection_mode(GTK_CLIST(list), GTK_SELECTION_BROWSE);

   gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(vport), list);


   /*back-end server gui*/
   advertised_address_frame = frame = gtk_frame_new(
      "Internal Back-End Servers for Selected Advertised Service");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 2);

   hbox2 = gtk_hbox_new(FALSE, 2);
   gtk_container_border_width(GTK_CONTAINER(hbox2), 4);
   gtk_container_add(GTK_CONTAINER(frame), hbox2);

   align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox2), align, FALSE, FALSE, 0);
   vbox = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(align), vbox);

   back_end_server_add = button = gtk_button_new_with_label("Add");
   gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

   back_end_server_del = button = gtk_button_new_with_label("Del");
   gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

   vbox = gtk_vbox_new(FALSE, 0);
   gtk_box_pack_start(GTK_BOX(hbox2), vbox, TRUE, TRUE, 0);

   vport = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(vport), 
        			  GTK_POLICY_AUTOMATIC,
        			  GTK_POLICY_AUTOMATIC);
   gtk_widget_set_usize(vport, 400, 64);
   gtk_box_pack_start(GTK_BOX(vbox), vport, TRUE, TRUE, 0);

   back_end_server_frame = hbox3 = gtk_hbox_new(FALSE, 2);
   gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 2);

   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox3), align, TRUE, TRUE, 2);
   frame = gtk_frame_new("Internal Address");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_container_add(GTK_CONTAINER(align), frame);
   back_end_server_addr = entry = gtk_entry_new();
   gtk_widget_set_usize(entry, 80, -1);
   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_container_add(GTK_CONTAINER(align), entry);
   gtk_container_add(GTK_CONTAINER(frame), align);

   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox3), align, FALSE, FALSE, 2);
   frame = gtk_frame_new("Port");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_container_add(GTK_CONTAINER(align), frame);
   back_end_server_port = entry = gtk_entry_new();
   gtk_widget_set_usize(entry, 32, -1);
   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_container_add(GTK_CONTAINER(align), entry);
   gtk_container_add(GTK_CONTAINER(frame), align);

   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_box_pack_start(GTK_BOX(hbox3), align, FALSE, FALSE, 2);
   frame = gtk_frame_new("Weight");
   gtk_container_border_width(GTK_CONTAINER(frame), 2);
   gtk_container_add(GTK_CONTAINER(align), frame);
   back_end_server_weight = entry = gtk_entry_new();
   gtk_widget_set_usize(entry, 32, -1);
   align = gtk_alignment_new(0.0, 0.0, 1.0, 0.0);
   gtk_container_add(GTK_CONTAINER(align), entry);
   gtk_container_add(GTK_CONTAINER(frame), align);

   back_end_server_clist = list = gtk_clist_new_with_titles(
     3, 
     back_end_server_clist_titles
   );
   gtk_clist_set_column_resizeable(GTK_CLIST(list),  0, TRUE);
   gtk_clist_set_column_resizeable(GTK_CLIST(list),  1, FALSE);
   gtk_clist_set_column_resizeable(GTK_CLIST(list),  2, FALSE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(list), 0, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(list), 1, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(list), 2, TRUE);
   gtk_clist_set_selection_mode(GTK_CLIST(list), GTK_SELECTION_BROWSE);

   gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(vport), list);

   gtk_widget_set_sensitive(advertised_address_frame, 0);
   gtk_widget_set_sensitive(ipvs_server_frame, 0);
   gtk_widget_set_sensitive(back_end_server_frame, 0);      

   CONNECT(
      advertised_service_addr,
      "changed",
      cb_external_service_addr_change,
      NULL
   );
   CONNECT(
      advertised_service_port, 
      "changed", 
      cb_external_service_port_change,   
      NULL
   ); 
   CONNECT(
      back_end_server_addr,  
      "changed",      
      cb_back_end_server_addr_change,  
      NULL
   );
   CONNECT(
      ipvs_server_addr,  
      "changed",      
      cb_ipvs_server_addr_change,  
      NULL
   );
   CONNECT(
      back_end_server_port,  
      "changed",      
      cb_back_end_server_port_change,  
      NULL
   );
   CONNECT(
      back_end_server_weight, 
      "changed",      
      cb_back_end_server_weight_change, 
      NULL
   );
   CONNECT(ipvs_server_addr, "changed", cb_ipvs_server_addr_change, NULL);
   CONNECT(
      advertised_service_clist,   
      "select-row",   
      cb_advertised_service_select,       
      NULL
   );
   CONNECT(
      advertised_service_clist,   
      "unselect-row", 
      cb_advertised_service_unselect,     
      NULL
   );
   CONNECT(
      back_end_server_clist,  
      "select-row",   
      cb_back_end_server_select,      
      NULL
   );
   CONNECT(
      back_end_server_clist,  
      "unselect-row", 
      cb_back_end_server_unselect,    
      NULL
   );
   CONNECT(
      ipvs_server_clist,  
      "select-row",   
      cb_ipvs_server_select,      
      NULL
   );
   CONNECT(
      ipvs_server_clist,  
      "unselect-row", 
      cb_ipvs_server_unselect,    
      NULL
   );
   CONNECT(sched_wlc_radio, "toggled", cb_sched_wlc_radio_toggle, NULL);
   CONNECT(sched_wrr_radio, "toggled", cb_sched_wrr_radio_toggle, NULL);
   CONNECT(sched_lc_radio,  "toggled", cb_sched_lc_radio_toggle,  NULL);
   CONNECT(sched_rr_radio,  "toggled", cb_sched_rr_radio_toggle,  NULL);
   CONNECT(proto_tcp_radio, "toggled", cb_proto_tcp_radio_toggle, NULL);
   CONNECT(
      proto_persistent_tcp_radio, 
      "toggled",      
      cb_proto_persistent_tcp_radio_toggle, 
      NULL
   );
   CONNECT(proto_udp_radio, "toggled", cb_proto_udp_radio_toggle, NULL);
   CONNECT(advertised_service_add, "clicked", cb_add_advertised_service, NULL);
   CONNECT(advertised_service_del, "clicked", cb_del_advertised_service, NULL);
   CONNECT(back_end_server_add,    "clicked", cb_add_back_end_server,    NULL);
   CONNECT(back_end_server_del,    "clicked", cb_del_back_end_server,    NULL);
   CONNECT(ipvs_server_add,        "clicked", cb_add_ipvs_server,        NULL);
   CONNECT(ipvs_server_del,        "clicked", cb_del_ipvs_server,        NULL);

}

void
gui(void)
{
   GtkWidget *win, *align, *label, *vbox, *hbox, *button, *hbox2;
#ifdef WITH_LOGO
   GtkWidget *pix;
   GdkPixmap *pm, *mm;
#endif

   win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   CONNECT(win, "delete_event", cb_main_close, win);
   gtk_window_set_wmclass(GTK_WINDOW(win), "IPVS-Gui", "IPVS");
   gtk_window_set_title(GTK_WINDOW(win), "IPVS Configuration");
   gtk_container_border_width(GTK_CONTAINER(win), 2);
   gtk_window_set_policy(GTK_WINDOW(win), 1, 1, 1);

   vbox = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(win), vbox);

   hbox = gtk_hbox_new(FALSE, 2);
   gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);

   label = gtk_label_new(
     "Connection forwarding and balancing configuration options");
   gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 2);

#ifdef WITH_LOGO
   gtk_widget_realize(win);
   pm = gdk_pixmap_create_from_xpm_d(win->window, &mm, NULL, va_xpm);
   pix = gtk_pixmap_new(pm, mm);
   gdk_pixmap_unref(pm);
   gdk_pixmap_unref(mm);

   button = gtk_button_new();
   gtk_container_add(GTK_CONTAINER(button), pix);
#else
   button = gtk_button_new_with_label("About...");
#endif
   gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 2);

   CONNECT(button, "clicked", cb_about, NULL);

   align = gtk_alignment_new(0.5, 0.5, 1.0, 1.0);
   gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

   gui_tcp(align);

   align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
   gtk_box_pack_start(GTK_BOX(vbox), align, FALSE, FALSE, 0);

   hbox2 = gtk_hbox_new(FALSE, 2);
   gtk_container_add(GTK_CONTAINER(align), hbox2);

   hbox = gtk_hbox_new(FALSE, 2);
   gtk_box_pack_start(GTK_BOX(hbox2), hbox, TRUE, TRUE, 2);

   cfg_reload = button = gtk_button_new_with_label("Reload Configuration");
   gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 2);

   cfg_config = button = gtk_button_new_with_label("Change Configuration...");
   gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 2);

   hbox = gtk_hbox_new(TRUE, 2);
   gtk_box_pack_start(GTK_BOX(hbox2), hbox, FALSE, FALSE, 2);

   service_ok = button = gtk_button_new_with_label("OK");
   gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 2);

   service_apply = button = gtk_button_new_with_label("Apply");
   gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 2);

   service_cancel = button = gtk_button_new_with_label("Cancel");
   gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 2);

   CONNECT(service_ok,     "clicked", cb_ok,     NULL);
   CONNECT(service_apply,  "clicked", cb_apply,  NULL);
   CONNECT(service_cancel, "clicked", cb_cancel, NULL);
   CONNECT(cfg_reload, "clicked", cb_reload, NULL);
   CONNECT(cfg_config, "clicked", cb_options, NULL);

   gtk_widget_show_all(win);
}

/* config load/save */
void
load_config(char *file)
{
   advertised_service *t = NULL;
   FILE *f;

   /* delete old stuff */
   while (service)
      del_advertised_service(service->data);

   /* -s wlc = weighted lest connection */
   /* -s wrr = weighted round-robnin */
   /* -s rr =  round robin */
   /* -s lc =  least connection */

   /* -w NUM = weight of server */

   /* IP.ADDRESS.COM.BLAH:PORT */
   f = fopen(file,  "r");
   if (f)
     {
        char buf[1024];

        while (fgets(buf, 1023, f))
          {
             gchar opt1[16], opt2[16], addr[512], port[32];
             gchar meth[32], opt3[16], wgt[12];

             if (buf[0] != '#')
               {
        	  opt1[0] = 0;
        	  opt3[0] = 0;
        	  sscanf(buf, 
        		 "%*s %15s %15s %511[0-9a-zA-Z.-]:%31s %*s %31s %15s ",
        		 opt1, opt2, addr, port, meth, opt3);
        	  if (!strcmp(opt1, "-A"))
        	    {
        	       int method = METHOD_WLC, protocol = PROTO_TCP;
        	       
        	       if (!strcmp(meth, "wlc"))
        		  method = METHOD_WLC;
        	       else if (!strcmp(meth, "wrr"))
        		  method = METHOD_WRR;
        	       else if (!strcmp(meth, "rr"))
        		  method = METHOD_RR;
        	       else if (!strcmp(meth, "lc"))
        		  method = METHOD_LC;
        	       if (!strcmp(opt2, "-u"))
        		  protocol = PROTO_UDP;
        	       else if (!strcmp(opt3, "-p"))
        		  protocol = PROTO_TCP_PERSIST;
        	       t = add_advertised_service(addr, port, method, protocol);
        	    }
        	  else if ((!strcmp(opt1, "-a")) && (t))
        	    {
        	       sscanf(buf, 
        		      "%*s %*s %*s %*s %*s %511[0-9a-zA-Z.-]:%31s %*s %*s %15s ",
        		      addr, port, wgt);
        	       add_back_end_server(t, addr, port, atoi(wgt));
        	    }
               }
          }
        fclose(f);
     }
   gtk_clist_select_row(GTK_CLIST(advertised_service_clist), 1, 0);
   gtk_clist_select_row(GTK_CLIST(advertised_service_clist), 0, 0);
}

void
save_transparent_proxy(char *host, GList *hosts)
{
   FILE *f;
   gchar tprox[4096];

   g_snprintf(tprox, sizeof(tprox), "transparent_proxy");
   f = fopen(tprox, "w");
   if (f)
     {
        GList *l;

        fprintf(f,
        	"#########################################################\n"
        	"# WARNING WARNING WARING WARNING WARNING WARING WARNING #\n"
        	"#########################################################\n"
        	"# Do NOT Edit this file by hand. This is automatically  #\n"
        	"# generated and its format is assumed by the config     #\n"
        	"# tools that generate this file.                        #\n"
        	"#########################################################\n"
        	"\n"
        	"#########################################################\n"
        	"# Config flie for IPVS transparent back end servers     #\n"
        	"# settings for transparent proxy                        #\n"
        	"# Generated by a GUI config tool                        #\n"
        	"#########################################################\n"
        	"\n"
        	"#########################################################\n"
        	"# Config follows ...                                    #\n"
        	"#########################################################\n"
        	"\n\n\n\n"
        	);
        fprintf(f, "TRANSPARENT_ADDRESSES=\"%s", host);
        if (hosts)
          {
             for (l = hosts; l; l = l->next)
        	fprintf(f, " %s", (char *)l->data);
          }
        fprintf(f, "\"\n");
        fprintf(f, 
        	"\n\n\n\n"
        	"#########################################################\n"
        	"# End config ...                                        #\n"
        	"#########################################################\n"
        	"\n"
        	"#########################################################\n"
        	"# WARNING WARNING WARING WARNING WARNING WARING WARNING #\n"
        	"#########################################################\n"
        	"# Do NOT Edit this file by hand. This is automatically  #\n"
        	"# generated and its format is assumed by the config     #\n"
        	"# tools that generate this file.                        #\n"
        	"#########################################################\n"
        	);
        fclose(f);
     }
}

void
save_config(char *file)
{
   FILE *f;

   extern options_t opt;

   /* write ipvs config */
   f = fopen(file, "w");
   if (f)
     {
        advertised_service   *t;
        back_end_server  *t2;
        GList *l, *ll;
        gchar *methods[] = {"wlc", "wrr", "rr", "lc"};

        fprintf(f, 
        	"#########################################################\n"
        	"# WARNING WARNING WARING WARNING WARNING WARING WARNING #\n"
        	"#########################################################\n"
        	"# Do NOT Edit this file by hand. This is automatically  #\n"
        	"# generated and its format is assumed by the config     #\n"
        	"# tools that generate this file.                        #\n"
        	"#########################################################\n"
        	"\n"
        	"#########################################################\n"
        	"# Config flie for IPVS                                  #\n"
        	"# Commands to configure and IPVS server, to be          #\n"
        	"# run by /etc/rc.d/init.d/ipvs                          #\n"
        	"# Generated by a GUI config tool                        #\n"
        	"#########################################################\n"
        	"\n"
        	"#########################################################\n"
        	"# Config follows ...                                    #\n"
        	"#########################################################\n"
        	"\n\n\n\n"
        	);
        for (l = service; l; l = l->next)
          {
             t = l->data;

             if (t->protocol == PROTO_TCP)
        	fprintf(f, "ipvsadm -A -t %s:%s -s %s\n", 
        		t->address, t->port, methods[t->method]);
             else if (t->protocol == PROTO_TCP_PERSIST)
        	fprintf(f, "ipvsadm -A -t %s:%s -s %s -p 300 -M 255.255.255.255\n",
        		t->address, t->port, methods[t->method]);
             else if (t->protocol == PROTO_UDP)
        	fprintf(f, "ipvsadm -A -u %s:%s -s %s\n", 
        		t->address, t->port, methods[t->method]);
             for (ll = t->back_end_servers; ll; ll = ll->next)
               {
        	  t2 = ll->data;
        	  
        	  if (t->protocol == PROTO_UDP)
        	     fprintf(f, "ipvsadm -a -u %s:%s -r %s:%s -g -w %i\n",
        		     t->address, t->port, t2->address, t2->port, t2->weight);
        	  else
        	     fprintf(f, "ipvsadm -a -t %s:%s -r %s:%s -g -w %i\n",
        		     t->address, t->port, t2->address, t2->port, t2->weight);
               }
          }
        fprintf(f, 
        	"\n\n\n\n"
        	"#########################################################\n"
        	"# End config ...                                        #\n"
        	"#########################################################\n"
        	"\n"
        	"#########################################################\n"
        	"# WARNING WARNING WARING WARNING WARNING WARING WARNING #\n"
        	"#########################################################\n"
        	"# Do NOT Edit this file by hand. This is automatically  #\n"
        	"# generated and its format is assumed by the config     #\n"
        	"# tools that generate this file.                        #\n"
        	"#########################################################\n"
        	);
        fclose(f);
     }
   /* write transparent proxy config per machine */
     {
        gchar *host;
        GList *host_names = NULL;
        GList *hosts = NULL;
        advertised_service   *t;
        back_end_server  *t2;
        GList *l, *ll;	

        /* build list of unique backend serevr hosts */
        for (l = service; l; l = l->next)
          {
             t = l->data;

             for (ll = t->back_end_servers; ll; ll = ll->next)
               {
        	  GList *l2;
        	  int    ok = 1;
        	  
        	  t2 = ll->data;		  
        	  for (l2 = host_names; l2; l2 = l2->next)
        	    {
        	       if (!strcmp(l2->data, t2->address))
        		  ok = 0;
        	    }
        	  if (ok)
        	     host_names = g_list_append(host_names, t2->address);
               }
          }
        /* go thru backend host list and build proxy host list */
        for (l = host_names; l; l = l->next)
          {
             /* go thru all frontend servers */
             for (ll = service; ll; ll = ll->next)
               {
        	  GList *l2;
        	  int    ok = 0;
        	  
        	  t = ll->data;
        	  
        	  /* find server in bacnekdn server list */
        	  for (l2 = t->back_end_servers; l2; l2 = l2->next)
        	    {
        	       t2 = l2->data;
        	       if (!strcmp(l->data, t2->address))
        		  ok = 1;
        	    }
        	  /* if front end server contains backend in list - add */
        	  if (ok)
        	    {
        	       /* if the serevsr is alreayd inthe list - dont add */
        	       for (l2 = hosts; l2; l2 = l2->next)
        		 {
        		    if (!strcmp(l2->data, t->address))
        		       ok = 0;
        		 }
        	       if (ok)
        		  hosts = g_list_append(hosts, t->address);
        	    }
               }
             host = l->data;
             save_transparent_proxy(host, hosts);
             remote_cp(NULL, "transparent_proxy", 
        	       host, opt.transparent_proxy_config_file);
             start_transparent_proxy(host);
             if (hosts)
               {
        	  g_list_free(hosts);
        	  hosts = NULL;
               }
          }
        if (host_names)
           g_list_free(host_names);
     }
   remote_cp(NULL, "ipvs", opt.master_host, opt.ipvs_config_file);
   start_ipvs(opt.master_host);
}

/* activation */

void
start_ipvs(char *machine)
{
   gchar s[4096];

   extern options_t opt;

   g_snprintf(s, sizeof(s), "%s %s@%s %s start", 
              opt.rsh_command, opt.user, machine, opt.ipvs_init_script);
   system(s);
}

/*
 void
 stop_ipvs(char *machine)
 {
    gchar s[4096];

    g_snprintf(s, sizeof(s), "%s %s@%s %s stop", 
               opt.rsh_command, opt.user, machine,  opt.ipvs_init_script);
    system(s);
 }
 */

void
start_transparent_proxy(char *machine)
{
   gchar s[4096];

   extern options_t opt;

   g_snprintf(s, sizeof(s), 
              "%s %s@%s %s start", opt.rsh_command,
              opt.user, machine, opt.transparent_proxy_init_script);
   system(s);
}

/*
 void
 stop_transparent_proxy(char *machine)
 {
    gchar s[4096];

    g_snprintf(s, sizeof(s), 
               "%s %s@%s %s stop", opt.rsh_command,
               opt.user, machine, opt.transparent_proxy_init_script);
    system(s);
 }
 */

/* copying files around */
void
remote_cp(char *machine1, char *file1, char *machine2, char *file2)
{
   gchar s[4096];

   extern options_t opt;

   if ((machine1) && (machine2))
      g_snprintf(s, sizeof(s), "%s %s@%s:%s %s@%s:%s", opt.rcp_command,
        	 opt.user, machine1, file1, opt.user, machine2, file2);
   else if ((!machine1) && (machine2))
      g_snprintf(s, sizeof(s), "%s %s %s@%s:%s",  opt.rcp_command,
        	 file1, opt.user, machine2, file2);
   else if ((machine1) && (!machine2))
      g_snprintf(s, sizeof(s), "%s %s@%s:%s %s",  opt.rcp_command,
        	 opt.user, machine1, file1, file2);
   else
      g_snprintf(s, sizeof(s), "cp %s %s", file1, file2);
   system(s);
}


/* Put GTK values into opt struct and write to disk*/

void options_to_disk(void){
   extern options_t opt;

   config_file_write(opt.rc_file);
}


/* main */

int
main(int argc, char **argv)
{
   extern options_t opt;

   options(argc, argv, OPT_FIRST_CALL);
   config_file_to_opt(opt.rc_file);
   config_file_write(opt.rc_file);

   gtk_init(&argc, &argv);   
   gui();

   remote_cp(opt.master_host, opt.ipvs_config_file, NULL, "ipvs");
   load_config("ipvs");
   gtk_main();
   return(0);
}
