/**********************************************************************
 * callback.c
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

#include "callback.h"
#include "lvs-gui.h"

const char *readme_txt =
#include "readme.h"
;

/**********************************************************************
 * cb_main_close
 * Callback to close the application
 * Pre: w: ignored
 *      data: ignored
 * Post: application has exited
 * Return: none
 **********************************************************************/

void cb_main_close(GtkWidget *w, void *data){
   options_to_disk();
   exit(0);
}


/**********************************************************************
 * cb_close
 * Callback to close a widget
 * Pre: w: widget to close
 *      data: ignored
 * Post: w is destroyed
 * Return: none
 **********************************************************************/

void cb_close(GtkWidget * w, void *data){
   gtk_widget_destroy(GTK_WIDGET(w));
}


/**********************************************************************
 * cb_external_service_addr_change
 * Callback to change the address of an external service
 * Pre: entry: Entry field to take address from
 *      user_data: ignored
 * Post: Address of the current external service is changed to the string 
 *       contained in entry
 * Return: none
 **********************************************************************/

void cb_external_service_addr_change(GtkEntry *entry, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(current_advertised_service->address){
         g_free(current_advertised_service->address);
      }
      current_advertised_service->address=g_strdup(gtk_entry_get_text(entry));
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_external_service_port_change
 * Callback to change the port of an external service
 * Pre: entry: Entry field to take port from
 *      user_data: ignored
 * Post: Port of the current external service is changed to the string 
 *       contained in entry
 * Return: none
 **********************************************************************/

void cb_external_service_port_change(GtkEntry *entry, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(current_advertised_service->port){
         g_free(current_advertised_service->port);
      }
      current_advertised_service->port=g_strdup(gtk_entry_get_text(entry));
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * UPDATE_BACK_END_SERVER
 * Lame macro to ipdate a backend server
 **********************************************************************/

#define UPDATE_BACK_END_SERVER \
   if(current_advertised_service){\
      update_back_end_server(\
         current_advertised_service,\
         current_back_end_server\
      );\
   }\


/**********************************************************************
 * UPDATE_IPVS_SERVER
 * Lame macro to ipdate a backend server
 **********************************************************************/

#define UPDATE_IPVS_SERVER \
   if(current_advertised_service){\
      update_ipvs_server(\
         current_advertised_service,\
         current_ipvs_server\
      );\
   }\


/**********************************************************************
 * cb_back_end_server_addr_change
 * Callback to change the address of a back end server
 * Pre: entry: Entry field to take address from
 *      user_data: ignored
 * Post: Address of the current back end server is changed to the string 
 *       contained in entry
 * Return: none
 **********************************************************************/

void cb_back_end_server_addr_change(GtkEntry *entry, gpointer user_data){
   extern advertised_service *current_advertised_service;
   extern back_end_server *current_back_end_server;

   if(current_back_end_server){
      if(current_back_end_server->address){
          g_free(current_back_end_server->address);
      }
      current_back_end_server->address=g_strdup(gtk_entry_get_text(entry));
      UPDATE_BACK_END_SERVER
   }
}


/**********************************************************************
 * cb_back_end_server_port_change
 * Callback to change the port of a back end server
 * Pre: entry: Entry field to take port from
 *      user_data: ignored
 * Post: Port of the current back end server is changed to the string 
 *       contained in entry
 * Return: none
 **********************************************************************/

void cb_back_end_server_port_change(GtkEntry *entry, gpointer user_data){
   extern advertised_service *current_advertised_service;
   extern back_end_server *current_back_end_server;

   if(current_back_end_server){
      if(current_back_end_server->port){
         g_free(current_back_end_server->port);
      }
      current_back_end_server->port = g_strdup(gtk_entry_get_text(entry));
      UPDATE_BACK_END_SERVER
   }
}


/**********************************************************************
 * cb_back_end_server_weight_change
 * Callback to change the weight of a back end server
 * Pre: entry: Entry field to take port from
 *      user_data: ignored
 * Post: Weight of the current back end server is changed to the string 
 *       contained in entry
 * Return: none
 **********************************************************************/

void cb_back_end_server_weight_change(GtkEntry *entry, gpointer user_data){
   extern advertised_service *current_advertised_service;
   extern back_end_server *current_back_end_server;

   if(current_back_end_server){
      current_back_end_server->weight = atoi(gtk_entry_get_text(entry));
      UPDATE_BACK_END_SERVER
   }
}


/**********************************************************************
 * cb_ipvs_server_addr_change
 * Callback to change the address of a IPVS server
 * Pre: entry: Entry field to take address from
 *      user_data: ignored
 * Post: Address of the current IPVS server is changed to the string 
 *       contained in entry
 * Return: none
 **********************************************************************/

void cb_ipvs_server_addr_change(GtkEntry *entry, gpointer user_data){
   extern advertised_service *current_advertised_service;
   extern ipvs_server *current_ipvs_server;

   if(current_ipvs_server){
      if(current_ipvs_server->address){
         g_free(current_ipvs_server->address);
      }
      current_ipvs_server->address = g_strdup(gtk_entry_get_text(entry));
      UPDATE_IPVS_SERVER
   }
}


/**********************************************************************
 * cb_sched_wlc_radio_toggle
 * Callback to toggle the radio button for the Weighted Least Connected (wlc)
 * connection scheduling algorithm
 * Pre: radiobutton: radiobutton for wlc
 * Post: wlc radio buttion is active on the GUI
 *       scheduling algorithm for the current advertised service
 *       is set to wlc
 * Return: none
 **********************************************************************/

void cb_sched_wlc_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))){
         current_advertised_service->method = METHOD_WLC;
      }
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_sched_wrr_radio_toggle
 * Callback to toggle the radio button for the Weighted Round Robin (wrr)
 * connection scheduling algorithm
 * Pre: radiobutton: radiobutton for wrr
 * Post: wrr radio buttion is active on the GUI
 *       scheduling algorithm for the current advertised service
 *       is set to wrr
 * Return: none
 **********************************************************************/

void cb_sched_wrr_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))){
         current_advertised_service->method = METHOD_WRR;
      }
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_sched_lc_radio_toggle
 * Callback to toggle the radio button for the Least COnnected (lc)
 * connection scheduling algorithm
 * Pre: radiobutton: radiobutton for lc
 * Post: lc radio buttion is active on the GUI
 *       scheduling algorithm for the current advertised service
 *       is set to lc
 * Return: none
 **********************************************************************/

void cb_sched_lc_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))){
         current_advertised_service->method = METHOD_RR;
      }
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_sched_rr_radio_toggle
 * Callback to toggle the radio button for the Round Robin (rr)
 * connection scheduling algorithm
 * Pre: radiobutton: radiobutton for rr
 * Post: rr radio buttion is active on the GUI
 *       scheduling algorithm for the current advertised service
 *       is set to rr
 * Return: none
 **********************************************************************/

void cb_sched_rr_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))){
         current_advertised_service->method = METHOD_LC;
      }
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_proto_tcp_radio_toggle
 * Callback to toggle the radio button for the TCP protocol
 * Pre: radiobutton: radiobutton for TCP
 * Post: TCP radio buttion is active on the GUI
 *       protocol algorithm for the current advertised service
 *       is set to TCP
 * Return: none
 **********************************************************************/


void cb_proto_tcp_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))){
        current_advertised_service->protocol = PROTO_TCP;
      }
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_proto_persistent_tcp_radio_toggle
 * Callback to toggle the radio button for the Persistent TCP protocol
 * Pre: radiobutton: radiobutton for Persistent TCP
 * Post: Persistent TCP radio buttion is active on the GUI
 *       protocol algorithm for the current advertised service
 *       is set to Persistent TCP
 * Return: none
 **********************************************************************/

void cb_proto_persistent_tcp_radio_toggle(
  GtkRadioButton *radiobutton, 
  gpointer user_data
){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))){
         current_advertised_service->protocol=PROTO_TCP_PERSIST;
      }
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_proto_udp_radio_toggle
 * Callback to toggle the radio button for the UDP protocol
 * Pre: radiobutton: radiobutton for UDP
 * Post: UDP radio buttion is active on the GUI
 *       protocol algorithm for the current advertised service
 *       is set to UDP
 * Return: none
 **********************************************************************/

void cb_proto_udp_radio_toggle(GtkRadioButton *radiobutton, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))){
         current_advertised_service->protocol = PROTO_UDP;
      }
      update_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_advertised_service_select
 * Callback to select an advertised service on the GUI
 * Pre: clist: ignored
 *      row: row to select
 *      column: ignored
 *      event: ignored
 *      user_data: ignored
 * Post: Advertised service is selected
 *       current_advertised_service is set to the advertised service
 *       corresponding to the selected service
 * Return: none
 **********************************************************************/

void cb_advertised_service_select(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
){
   GList *l;
   gint   i;

   extern advertised_service *current_advertised_service;
   extern GtkWidget *advertised_service_addr;
   extern GtkWidget *advertised_service_port;
   extern GtkWidget *sched_wlc_radio;
   extern GtkWidget *sched_wrr_radio;
   extern GtkWidget *sched_lc_radio;
   extern GtkWidget *sched_rr_radio;
   extern GtkWidget *proto_tcp_radio;
   extern GtkWidget *proto_persistent_tcp_radio;
   extern GtkWidget *proto_udp_radio;
   extern GtkWidget *advertised_address_frame;
   extern GtkWidget *back_end_server_frame;
   extern GtkWidget *ipvs_server_frame;
   extern GList *service;

   for(l=service,i=0;l;l=l->next,i++){
      if(i==row){
         current_advertised_service = (advertised_service *)l->data;
         gtk_entry_set_text(
            GTK_ENTRY(advertised_service_addr), 
            current_advertised_service->address
         );
         gtk_entry_set_text(
            GTK_ENTRY(advertised_service_port), 
            current_advertised_service->port
         );
         if(current_advertised_service->method == METHOD_WLC){
            gtk_toggle_button_set_active(
               GTK_TOGGLE_BUTTON(sched_wlc_radio), 
               TRUE
            );
         }
         else if(current_advertised_service->method == METHOD_WRR){
            gtk_toggle_button_set_active(
               GTK_TOGGLE_BUTTON(sched_wrr_radio), 
               TRUE
            );
         }
         else if(current_advertised_service->method == METHOD_RR){
            gtk_toggle_button_set_active(
               GTK_TOGGLE_BUTTON(sched_lc_radio), 
               TRUE
            );
         }
         else if(current_advertised_service->method == METHOD_LC){
            gtk_toggle_button_set_active(
               GTK_TOGGLE_BUTTON(sched_rr_radio), 
               TRUE
            );
         }
         if(current_advertised_service->protocol == PROTO_TCP){
            gtk_toggle_button_set_active(
               GTK_TOGGLE_BUTTON(proto_tcp_radio), 
               TRUE
            );
         }
         else if(current_advertised_service->protocol == PROTO_TCP_PERSIST){
            gtk_toggle_button_set_active(
               GTK_TOGGLE_BUTTON(proto_persistent_tcp_radio), 
               TRUE
            );
         }
         else if(current_advertised_service->protocol == PROTO_UDP){
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
               proto_udp_radio), 
               TRUE
            );
         }
         gtk_widget_set_sensitive(advertised_address_frame, 1);
         gtk_widget_set_sensitive(ipvs_server_frame, 0);
         gtk_widget_set_sensitive(back_end_server_frame, 0);
      }
   }
   fill_back_end_server_clist(current_advertised_service);
   fill_ipvs_server_clist(current_advertised_service);
}


/**********************************************************************
 * cb_advertised_service_unselect
 * Callback to unselect the selected advertised service on the GUI
 * Pre: clist:     ignored
 *      row:       ignored
 *      column:    ignored
 *      event:     ignored
 *      user_data: ignored
 * Post: No advertised service is selected
 *       current_advertised_service is set to null
 *       current_ipvs_server is set to null
 *       current_back_end_server is set to null
 * Return: none
 **********************************************************************/

void cb_advertised_service_unselect(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
){
   extern advertised_service *current_advertised_service;
   extern ipvs_server *current_ipvs_server;
   extern back_end_server *current_back_end_server;
   extern GtkWidget *advertised_service_addr;
   extern GtkWidget *advertised_service_port;
   extern GtkWidget *sched_wlc_radio;
   extern GtkWidget *proto_tcp_radio;
   extern GtkWidget *advertised_address_frame;
   extern GtkWidget *back_end_server_frame;
   extern GtkWidget *ipvs_server_frame;
   extern GtkWidget *ipvs_server_addr;
   extern GtkWidget *back_end_server_addr;
   extern GtkWidget *back_end_server_port;
   extern GtkWidget *back_end_server_weight;


   current_advertised_service = NULL;
   current_ipvs_server = NULL;
   current_back_end_server = NULL;
   gtk_widget_set_sensitive(advertised_address_frame, 0);
   gtk_widget_set_sensitive(ipvs_server_frame, 0);
   gtk_widget_set_sensitive(back_end_server_frame, 0);
   gtk_entry_set_text(GTK_ENTRY(advertised_service_addr), "");   
   gtk_entry_set_text(GTK_ENTRY(advertised_service_port), "");   
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sched_wlc_radio), TRUE);
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(proto_tcp_radio), TRUE);
   gtk_entry_set_text(GTK_ENTRY(ipvs_server_addr), "");   
   gtk_entry_set_text(GTK_ENTRY(back_end_server_addr), "");   
   gtk_entry_set_text(GTK_ENTRY(back_end_server_port), "");   
   gtk_entry_set_text(GTK_ENTRY(back_end_server_weight), "");   
   fill_ipvs_server_clist(current_advertised_service);
   fill_back_end_server_clist(current_advertised_service);
}


/**********************************************************************
 * cb_back_end_server_select
 * Callback to select an back end server on the GUI
 * Pre: clist: ignored
 *      row: row to select
 *      column: ignored
 *      event: ignored
 *      user_data: ignored
 * Post: Back end server is selected
 *       current_advertised_service is set to the advertised service
 *       corresponding to the selected service
 * Return: none
 **********************************************************************/

void cb_back_end_server_select(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
){
   GList *l;
   int i;

   extern advertised_service *current_advertised_service;
   extern back_end_server *current_back_end_server;
   extern GtkWidget *back_end_server_addr;
   extern GtkWidget *back_end_server_port;
   extern GtkWidget *back_end_server_weight;
   extern GtkWidget *back_end_server_frame;

   if(current_advertised_service){
      for(l=current_advertised_service->back_end_servers,i=0;l;l=l->next,i++){
         if(i==row){
            gchar s[32];

            current_back_end_server = (back_end_server *)l->data;		  
            gtk_entry_set_text(
               GTK_ENTRY(back_end_server_addr), 
               current_back_end_server->address
            );
            gtk_entry_set_text(
               GTK_ENTRY(back_end_server_port), 
               current_back_end_server->port
            );
            g_snprintf(s, sizeof(s), "%i", current_back_end_server->weight);
            gtk_entry_set_text(GTK_ENTRY(back_end_server_weight), s);
            gtk_widget_set_sensitive(back_end_server_frame, 1);
         }
      }
   }
}


/**********************************************************************
 * cb_back_end_server_unselect
 * Callback to unselect the selected advertised service on the GUI
 * Pre: clist:     ignored
 *      row:       ignored
 *      column:    ignored
 *      event:     ignored
 *      user_data: ignored
 * Post: No advertised service is selected
 *       current_back_end_server is set to null
 * Return: none
 **********************************************************************/

void cb_back_end_server_unselect(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
){
   extern back_end_server *current_back_end_server;
   extern GtkWidget *back_end_server_addr;
   extern GtkWidget *back_end_server_port;
   extern GtkWidget *back_end_server_weight;
   extern GtkWidget *back_end_server_frame;

   current_back_end_server = NULL;
   gtk_entry_set_text(GTK_ENTRY(back_end_server_addr), "");   
   gtk_entry_set_text(GTK_ENTRY(back_end_server_port), "");   
   gtk_entry_set_text(GTK_ENTRY(back_end_server_weight), "");   
   gtk_widget_set_sensitive(back_end_server_frame, 0);
}


/**********************************************************************
 * cb_ipvs_server_select
 * Callback to select an back end server on the GUI
 * Pre: clist: ignored
 *      row: row to select
 *      column: ignored
 *      event: ignored
 *      user_data: ignored
 * Post: Back end server is selected
 *       current_advertised_service is set to the advertised service
 *       corresponding to the selected service
 * Return: none
 **********************************************************************/

void cb_ipvs_server_select(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
){
   GList *l;
   int i;

   extern advertised_service *current_advertised_service;
   extern ipvs_server *current_ipvs_server;
   extern GtkWidget *ipvs_server_addr;
   extern GtkWidget *ipvs_server_frame;

   if(current_advertised_service){
      for(l=current_advertised_service->ipvs_servers,i=0;l;l=l->next,i++){
         if(i==row){
            current_ipvs_server = (ipvs_server *)l->data;		  
            gtk_entry_set_text(
               GTK_ENTRY(ipvs_server_addr), 
               current_ipvs_server->address
            );
            gtk_widget_set_sensitive(ipvs_server_frame, 1);
         }
      }
   }
}


/**********************************************************************
 * cb_ipvs_server_unselect
 * Callback to unselect the selected advertised service on the GUI
 * Pre: clist:     ignored
 *      row:       ignored
 *      column:    ignored
 *      event:     ignored
 *      user_data: ignored
 * Post: No advertised service is selected
 *       current_ipvs_server is set to null
 * Return: none
 **********************************************************************/

void cb_ipvs_server_unselect(
   GtkCList *clist, 
   gint row, 
   gint column, 
   GdkEventButton *event,
   gpointer user_data
){
   extern ipvs_server *current_ipvs_server;
   extern GtkWidget *ipvs_server_addr;
   extern GtkWidget *ipvs_server_frame;

   current_ipvs_server = NULL;
   gtk_entry_set_text(GTK_ENTRY(ipvs_server_addr), "");   
   gtk_widget_set_sensitive(ipvs_server_frame, 0);
}


/**********************************************************************
 * cb_add_advertised_service
 * Callback to add a new advertised service
 * Pre: button: ignored
 *      user_data: ignored
 * Post: New advertised service is added as per add_advertised_service
 *       If no advertised service exists already then it is creaded
 *       with IP_DEFAULT, PORT_DEFAULT, METHOD_WLC and PROTO_TCP
 *       Else the last advertised service in the list is copied
 * Return: None
 **********************************************************************/

void cb_add_advertised_service(GtkButton *button, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      add_advertised_service(
         current_advertised_service->address, 
         current_advertised_service->port, 
         current_advertised_service->method,
         current_advertised_service->protocol
      );
   }
   else{
      add_advertised_service(IP_DEFAULT, PORT_DEFAULT, METHOD_WLC, PROTO_TCP);
   }
}


/**********************************************************************
 * cb_del_advertised_service
 * Callback to delete an advertised service
 * Pre: button: ignored
 *      user_data: ignored
 * Post: current_advertised_service is deleted as per
 *       del_advertised_service
 * Return: None
 **********************************************************************/

void cb_del_advertised_service(GtkButton *button, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      del_advertised_service(current_advertised_service);
   }
}


/**********************************************************************
 * cb_add_back_end_server
 * Callback to add a new back end server
 * Pre: button: ignored
 *      user_data: ignored
 * Post: New back end server is added as per add_back_end_server
 *       with IP_ADDR_NEXT, PORT_CURRENT and WEIGHT_CURRENT for
 *       the current_advertised_service
 *       If there is no current_advertised_service then nothing is done
 * Return: None
 **********************************************************************/

void cb_add_back_end_server(GtkButton *button, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      add_back_end_server(
        current_advertised_service,
        IP_ADDR_NEXT,PORT_CURRENT,
        WEIGHT_CURRENT
     );
   }
}


/**********************************************************************
 * cb_del_back_end_server
 * Callback to delete an back end server
 * Pre: button: ignored
 *      user_data: ignored
 * Post: current_back_end_server is deleted as per
 *       del_back_end_server
 * Return: None
 **********************************************************************/

void cb_del_back_end_server(GtkButton *button, gpointer user_data){
   extern advertised_service *current_advertised_service;
   extern back_end_server *current_back_end_server;

   if(current_advertised_service && current_back_end_server){
      del_back_end_server(current_advertised_service, current_back_end_server);
   }
}

/**********************************************************************
 * cb_add_ipvs_server
 * Callback to add a new back end server
 * Pre: button: ignored
 *      user_data: ignored
 * Post: New back end server is added as per add_ipvs_server
 *       with IP_ADDR_NEXT for the current_advertised_service
 *       If there is no current_advertised_service then nothing is done
 * Return: None
 **********************************************************************/

void cb_add_ipvs_server(GtkButton *button, gpointer user_data){
   extern advertised_service *current_advertised_service;

   if(current_advertised_service){
      add_ipvs_server(current_advertised_service, IP_ADDR_NEXT);
   }
}


/**********************************************************************
 * cb_del_ipvs_server
 * Callback to delete an back end server
 * Pre: button: ignored
 *      user_data: ignored
 * Post: current_ipvs_server is deleted as per
 *       del_ipvs_server
 * Return: None
 **********************************************************************/

void cb_del_ipvs_server(GtkButton *button, gpointer user_data){
   extern advertised_service *current_advertised_service;
   extern ipvs_server *current_ipvs_server;

   if(current_advertised_service && current_ipvs_server){
      del_ipvs_server(current_advertised_service, current_ipvs_server);
   }
}


/**********************************************************************
 * cb_apply
 * Callback for apply button
 * Saves current options and config and propgates it to nodes
 * Pre: button: ignored
 *      user_data: ignored
 * Post: Current options are saved to disk
 *       Config is propogated to nodes
 * Return: none
 **********************************************************************/

void cb_apply(GtkButton *button, gpointer user_data){
   options_to_disk();
   save_config("ipvs");
}


/**********************************************************************
 * cb_ok
 * Callback for OK button
 * Saves current options and config, propgates it to nodes and exits
 * Pre: button: ignored
 *      user_data: ignored
 * Post: Config is propogated to nodes
 *       Current options are saved to disk
 *       Application exits
 * Return: none
 **********************************************************************/

void cb_ok(GtkButton *button, gpointer user_data){
   cb_apply(button, user_data);
   options_to_disk();
   exit(0);
}


/**********************************************************************
 * cb_cancel
 * Callback for cancel button
 * Saves current options and exits
 * Pre: button: ignored
 *      user_data: ignored
 * Post: Current options are saved to disk
 *       Application exits
 * Return: none
 **********************************************************************/

void cb_cancel(GtkButton *button, gpointer user_data){
   options_to_disk();
   exit(0);
}


/**********************************************************************
 * cb_reload
 * Callback for reload button
 * Save current options to disk, load config from master node
 * Pre: button: ignored
 *      user_data: ignored
 * Post: Current option sare saved to disk
 *       Configuration is loaded from master node
 * Return: none
 **********************************************************************/

void cb_reload(GtkButton *button, gpointer user_data){
   extern options_t opt;

   options_to_disk();
   remote_cp(opt.master_host, opt.ipvs_config_file, NULL, "ipvs");
   load_config("ipvs");
}


/**********************************************************************
 * cb_options_change
 * Callback to effect a change to options
 * Pre: entry: GtkEntry containing new configuration
 *      user_data: Old configurarion
 * Post: Old configuration is freed
 *       New configuration is stored
 * Return: none
 **********************************************************************/

void cb_options_change(GtkEntry *entry, gpointer user_data){
   char **txt;

   txt = user_data;
   if(*txt){
      g_free(*txt);
   }
   *txt = NULL;
   *txt = g_strdup(gtk_entry_get_text(entry));
}


/**********************************************************************
 * cb_options_close
 * Callback called when options dialog is closed
 * Pre: button: ignored
 *      user_data: GtkWidget representing options dialog
 * Post: options dialog is destroyed
 * Return: none
 **********************************************************************/ 

void cb_options_close(GtkButton *button, gpointer user_data){
   gtk_widget_destroy(GTK_WIDGET(user_data));
}


/**********************************************************************
 * cb_about
 * Callback to display about dialog
 * Pre: button: ignored
 *      user_data: ignored
 * Post: About dialog is displayed
 * Return: none
 **********************************************************************/

void cb_about(GtkButton *button, gpointer user_data){
   GtkWidget *sw;
   GtkWidget *win;
   GtkWidget *vbox;
   GtkWidget *label;
#ifdef WITH_LOGO
   GdkPixmap *pm;
   GdkPixmap *mm;
   GtkWidget *pix;

   extern static char * va_xpm[];
#endif
   extern const char *readme_txt;

   win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   CONNECT(win, "delete_event", cb_close, win);
   gtk_window_set_wmclass(GTK_WINDOW(win), "IPVS-Gui-About", "IPVS");
   gtk_window_set_title(GTK_WINDOW(win), "IPVS About");
   gtk_container_border_width(GTK_CONTAINER(win), 2);
   gtk_window_set_policy(GTK_WINDOW(win), 0, 0, 1);
   gtk_widget_set_usize(win, 512, 400);

   sw = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(
      GTK_SCROLLED_WINDOW(sw),
      GTK_POLICY_AUTOMATIC,
      GTK_POLICY_AUTOMATIC
   );
   gtk_container_add(GTK_CONTAINER(win), sw);

   vbox = gtk_vbox_new(FALSE, 2);

#ifdef WITH_LOGO
   gtk_widget_realize(win);
   pm = gdk_pixmap_create_from_xpm_d(win->window, &mm, NULL, va_xpm);
   pix = gtk_pixmap_new(pm, mm);
   gdk_pixmap_unref(pm);
   gdk_pixmap_unref(mm);
   gtk_box_pack_start(GTK_BOX(vbox), pix, FALSE, FALSE, 2);
#endif

   label = gtk_label_new(readme_txt);
   gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);

   gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 2);

   gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), vbox);

   gtk_widget_show_all(win);
}

/**********************************************************************
 * GTK_TABLE_ATTACH_RIGHT
 * Lame macro to attach a widget to a left hand column in a table
 * This column is _not_ expanded to fill available space
 **********************************************************************/

#define GTK_TABLE_ATTACH_LEFT(child, y) \
   gtk_table_attach( \
      GTK_TABLE(table), \
      child, \
      0, \
      1, \
      y, \
      y+1, \
      GTK_FILL, \
      (GtkAttachOptions)0, \
      0, \
      0  \
    );


/**********************************************************************
 * GTK_TABLE_ATTACH_RIGHT
 * Lame macro to attach a widget to a right hand column in a table
 * This column is expanded to fill available space
 **********************************************************************/

#define GTK_TABLE_ATTACH_RIGHT(child, y) \
   gtk_table_attach( \
      GTK_TABLE(table), \
      child, \
      1, \
      2, \
      y, \
      y+1, \
      GTK_EXPAND | GTK_FILL, \
      (GtkAttachOptions)0, \
      0,  \
      0 \
   );


/**********************************************************************
 * cb_options
 * Callback to display about dialog
 * Pre: button: ignored
 *      user_data: ignored
 * Post: About dialog is displayed
 * Return: none
 **********************************************************************/

void cb_options(GtkButton *button, gpointer user_data){
   GtkWidget *win;
   GtkWidget *label;
   GtkWidget *entry;
   GtkWidget *table;
   GtkWidget *align;
   GtkWidget *hbox;
   GtkWidget *close;
   GtkWidget *master_host_entry;
   GtkWidget *ipvs_config_file_entry;
   GtkWidget *ipvs_init_script_entry;
   GtkWidget *transparent_proxy_config_file_entry;
   GtkWidget *transparent_proxy_init_script_entry;
   GtkWidget *rsh_entry;
   GtkWidget *rcp_entry;
   GtkWidget *user_entry;

   extern options_t opt;

   win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   CONNECT(win, "delete_event", cb_close, win);
   gtk_window_set_wmclass(GTK_WINDOW(win), "IPVS-GUI-Config", "IPVS");
   gtk_window_set_title(GTK_WINDOW(win), "IPVS GUI Configuration Options");
   gtk_container_border_width(GTK_CONTAINER(win), 2);
   /*gtk_window_set_policy(GTK_WINDOW(win), 0, 0, 1);*/

   table = gtk_table_new(2, 2, FALSE);
   gtk_container_add(GTK_CONTAINER(win), table);
   gtk_table_set_row_spacings(GTK_TABLE(table), 4);
   gtk_table_set_col_spacings(GTK_TABLE(table), 4);

   label = gtk_label_new("IPVS Configuration Editor Options");
   gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 0, 1);

   align = gtk_alignment_new(0.0, 0.5, 0.0, 1.0);
   GTK_TABLE_ATTACH_LEFT(align, 1);
   label = gtk_label_new("Master Host");
   gtk_container_add(GTK_CONTAINER(align), label);
   master_host_entry = entry = gtk_entry_new();
   GTK_TABLE_ATTACH_RIGHT(entry, 1);

   align = gtk_alignment_new(0.0, 0.5, 0.0, 1.0);
   GTK_TABLE_ATTACH_LEFT(align, 2);
   label = gtk_label_new("IPVS Config File");
   gtk_container_add(GTK_CONTAINER(align), label);
   ipvs_config_file_entry = entry = gtk_entry_new();
   GTK_TABLE_ATTACH_RIGHT(entry, 2);

   align = gtk_alignment_new(0.0, 0.5, 0.0, 1.0);
   GTK_TABLE_ATTACH_LEFT(align, 3);
   label = gtk_label_new("IPVS init script");
   gtk_container_add(GTK_CONTAINER(align), label);
   ipvs_init_script_entry = entry = gtk_entry_new();
   GTK_TABLE_ATTACH_RIGHT(entry, 3);

   align = gtk_alignment_new(0.0, 0.5, 0.0, 1.0);
   GTK_TABLE_ATTACH_LEFT(align, 4);
   label = gtk_label_new("Transparent Proxy Config File");
   gtk_container_add(GTK_CONTAINER(align), label);
   transparent_proxy_config_file_entry = entry = gtk_entry_new();
   GTK_TABLE_ATTACH_RIGHT(entry, 4);

   align = gtk_alignment_new(0.0, 0.5, 0.0, 1.0);
   GTK_TABLE_ATTACH_LEFT(align, 5);
   label = gtk_label_new("Transparent Proxy Init Script");
   gtk_container_add(GTK_CONTAINER(align), label);
   transparent_proxy_init_script_entry = entry = gtk_entry_new();
   GTK_TABLE_ATTACH_RIGHT(entry, 5);

   align = gtk_alignment_new(0.0, 0.5, 0.0, 1.0);
   GTK_TABLE_ATTACH_LEFT(align, 6);
   label = gtk_label_new("Remote Execution Command");
   gtk_container_add(GTK_CONTAINER(align), label);
   rsh_entry = entry = gtk_entry_new();
   GTK_TABLE_ATTACH_RIGHT(entry, 6);

   align = gtk_alignment_new(0.0, 0.5, 0.0, 1.0);
   GTK_TABLE_ATTACH_LEFT(align, 7);
   label = gtk_label_new("Remote Copy Command");
   gtk_container_add(GTK_CONTAINER(align), label);
   rcp_entry = entry = gtk_entry_new();
   GTK_TABLE_ATTACH_RIGHT(entry, 7);

   align = gtk_alignment_new(0.0, 0.5, 0.0, 1.0);
   GTK_TABLE_ATTACH_LEFT(align, 8);
   label = gtk_label_new("Remote User");
   gtk_container_add(GTK_CONTAINER(align), label);
   user_entry = entry = gtk_entry_new();
   GTK_TABLE_ATTACH_RIGHT(entry, 8);

   align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
   gtk_table_attach_defaults(GTK_TABLE(table), align, 0, 2, 9, 10);

   hbox = gtk_hbox_new(TRUE, 2);
   gtk_container_add(GTK_CONTAINER(align), hbox);

   close = gtk_button_new_with_label("Close");
   gtk_box_pack_start(GTK_BOX(hbox), close, TRUE, TRUE, 2);   

   gtk_entry_set_text(GTK_ENTRY(master_host_entry), opt.master_host);
   gtk_entry_set_text(GTK_ENTRY(ipvs_config_file_entry), opt.ipvs_config_file);
   gtk_entry_set_text(GTK_ENTRY(ipvs_init_script_entry), opt.ipvs_init_script);
   gtk_entry_set_text(
      GTK_ENTRY(transparent_proxy_config_file_entry), 
      opt.transparent_proxy_config_file
   );
   gtk_entry_set_text(
      GTK_ENTRY(transparent_proxy_init_script_entry), 
      opt.transparent_proxy_init_script
   );
   gtk_entry_set_text(GTK_ENTRY(rsh_entry), opt.rsh_command);
   gtk_entry_set_text(GTK_ENTRY(rcp_entry), opt.rcp_command);
   gtk_entry_set_text(GTK_ENTRY(user_entry), opt.user);

   CONNECT(master_host_entry, "changed", cb_options_change, &(opt.master_host));
   CONNECT(
      ipvs_config_file_entry, 
      "changed", 
      cb_options_change, 
      &(opt.ipvs_config_file)
   );
   CONNECT(
      ipvs_init_script_entry, 
      "changed", 
      cb_options_change, 
      &(opt.ipvs_init_script)
   );
   CONNECT(
      transparent_proxy_config_file_entry, 
      "changed", 
      cb_options_change, 
      &(opt.transparent_proxy_config_file)
   );
   CONNECT(
      transparent_proxy_init_script_entry, 
      "changed", 
      cb_options_change, 
      &(opt.transparent_proxy_init_script)
   );
   CONNECT(rsh_entry,  "changed", cb_options_change, &(opt.rsh_command));
   CONNECT(rcp_entry,  "changed", cb_options_change, &(opt.rcp_command));
   CONNECT(user_entry, "changed", cb_options_change, &(opt.user));
   CONNECT(close,      "clicked", cb_options_close,  win);
   gtk_widget_show_all(win);
}
