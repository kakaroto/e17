/**********************************************************************
 * util.c
 *
 * Utility GUI funtions
 * 
 * Copyright (C) 2000 Carsten Haitzler and Simon Horman
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

#include "util.h"

/* Global Variables */

gchar *s_methods[] = {
  "Weighted least connection",
  "Weighted round robin",
  "Round robin",
  "Least connection"
};

gchar *s_protocols[] = { "TCP", "TCP Persistant", "UDP" };   


/**********************************************************************
 * _del_back_end_server
 * Funtion to be rolled into del_back_end_server and 
 * del_advertised_service that cleans up a back_end_server
 * Pre: be: back_end_server to free
 * Post: be and contents are freed
 * Return: none
 **********************************************************************/

static inline void _del_back_end_server(back_end_server *be){
   if(be->address){
      g_free(be->address);
   }
   if(be->port){
      g_free(be->port);
   }
   g_free(be);
}


/**********************************************************************
 * del_back_end_server
 * Delete a back_end server
 * Pre: a: advertised service that the back_end server belongs to
 *      be: back_end server to delete
 * Post: be is deleted
 * Return: none
 **********************************************************************/

void del_back_end_server(advertised_service *a, back_end_server *be){
   GList *l;
   gint i;

   extern GtkWidget *back_end_server_clist;

   _del_back_end_server(be);
   
   for(i=0,l=a->back_end_servers;l;l=l->next,i++){
      if(l->data==be){
         gtk_clist_select_row(GTK_CLIST(back_end_server_clist), i + 1, 0);
         gtk_clist_remove(GTK_CLIST(back_end_server_clist), i);
         break;
      }
   }
   a->back_end_servers = g_list_remove(a->back_end_servers, be);
}


/**********************************************************************
 * _del_ipvs_server
 * Funtion to be rolled into del_ipvs_server and 
 * del_advertised_service that cleans up a ipvs_server
 * Pre: be: ipvs_server to free
 * Post: be and contents are freed
 * Return: none
 **********************************************************************/

static inline void _del_ipvs_server(ipvs_server *ipvs){
   if(ipvs->address){
      g_free(ipvs->address);
   }
   g_free(ipvs);
}


/**********************************************************************
 * del_ipvs_server
 * Delete a ipvs server
 * Pre: a: advertised service that the ipvs server belongs to
 *      be: ipvs server to delete
 * Post: be is deleted
 * Return: none
 **********************************************************************/

void del_ipvs_server(advertised_service *a, ipvs_server *ipvs){
   GList *l;
   gint i;

   extern GtkWidget *ipvs_server_clist;

   _del_ipvs_server(ipvs);
   
   for(i=0,l=a->ipvs_servers;l;l=l->next,i++){
      if(l->data==ipvs){
         gtk_clist_select_row(GTK_CLIST(ipvs_server_clist), i + 1, 0);
         gtk_clist_remove(GTK_CLIST(ipvs_server_clist), i);
         break;
      }
   }
   a->ipvs_servers = g_list_remove(a->ipvs_servers, ipvs);
}


/**********************************************************************
 * del_advertised_service
 * Delete an advertised service
 * Pre: a: advertised service to delete
 * Post: advertised service is deleted
 * Return: none
 **********************************************************************/

void del_advertised_service(advertised_service *a){
   GList *l;
   gint i;

   extern GList *service;
   extern GtkWidget *advertised_service_clist;

   /* Delete back end servers */
   for(l=a->back_end_servers;l;l=l->next){
      _del_back_end_server((back_end_server *)l->data);
   }
   if(a->back_end_servers){
      g_list_free(a->back_end_servers);
   }

   /* Delete IPVS servers */
   for(l=a->ipvs_servers;l;l=l->next){
      _del_ipvs_server((ipvs_server *)l->data);
   }
   if(a->ipvs_servers){
      g_list_free(a->ipvs_servers);
   }

   /* Delete the rest of the advertised service */
   if(a->address){
      g_free(a->address);
   }
   if(a->port){
      g_free(a->port);
   }
   g_free(a);
   for(l=service,i=0;l;l=l->next,i++){
      if(l->data==a){
         gtk_clist_select_row(GTK_CLIST(advertised_service_clist), i + 1, 0);
         gtk_clist_remove(GTK_CLIST(advertised_service_clist), i);
         break;
      }
   }
   service = g_list_remove(service, a);
}


/**********************************************************************
 * add_advertised_service
 * Add a new advertised service
 * Pre: addr: Address of service
 *            May be an IP address or hostname
 *      port: Port for service
 *            May be an port number or service name as per /etc/services
 *      method: Connection Scheduling Method
 *              Index to extern gchar *s_methods[]
 *      protocol: Protocol
 *              Index to extern gchar *s_protocols[]
 * Post: New advertised service is created
 * Return: none
 **********************************************************************/

advertised_service *add_advertised_service(
   char *addr, 
   char *port, 
   int method, 
   int protocol
){
   advertised_service *t;
   gchar *s_text[4];

   extern gchar *s_methods[];
   extern gchar *s_protocols[];
   extern GList *service;
   extern GtkWidget *advertised_service_clist;

   t = g_malloc(sizeof(advertised_service));

   t->address           = g_strdup(addr);
   t->port              = g_strdup(port);
   t->back_end_servers  = NULL;
   t->ipvs_servers      = NULL;
   t->method            = method;
   t->protocol          = protocol;

   service = g_list_append(service, t);

   s_text[0] = t->address;
   s_text[1] = t->port;
   s_text[2] = s_methods[t->method];
   s_text[3] = s_protocols[t->protocol];
   gtk_clist_append(GTK_CLIST(advertised_service_clist), s_text);

   return t;
}


/**********************************************************************
 * add_back_end_server
 * Add a back end server
 * Pre: a: advertised service to add the back end server to
 *      addr: IP address or hostname of back end server
 *            If IP_ADDR_NEXT is used then
 *              If there are any back end servers defined then
 *              the address of the last one in the list is taken
 *              and incremented and used
 *              Else if there are any ipvs servers defined then
 *              the address of the last on in the list is taken
 *              and incremented and used
 *              Else the address of the advertised service is taken
 *              and incremented and used
 *            Else the address supplied is used
 *      port: Port or port name 
 *            If IP_PORT_CURRENT is used then
 *              If there are any back end servers defined then
 *              the port of the last one in the list is used
 *              Else the port of the advertised service is used
 *            Else the port supplied is used
 *     weight: Weight to use
 *             If WEIGHT_CURRENT is used then
 *              If there are any back end servers defined then
 *              the weight of the last one in the list is used
 *              Else WEIGHT_DEFAULT is used
 *            Else the port supplied is used
 * Post: New backend server is created
 * Return: None
 * Note: Incrementing will loop, so 255.255.255.255 + 1 = 0.0.0.0
 *       If the back end server, ipvs server or advertised service
 *       used as the base address for a new back end server has a
 *       hostname instead then an attempt will be made to resolve
 *       this. If this is succesul then the resulting IP address
 *       - the first on returned - will be incremented and used, 
 *       else a blank address will result.
 **********************************************************************/

void add_back_end_server(
   advertised_service *a, 
   char *addr, 
   char *port, 
   int weight
){
   back_end_server *last_be=NULL;
   back_end_server *be;
   ipvs_server *last_ipvs=NULL;
   gchar s_weight[32];
   gchar *s_text[3];
   GList *list_last;

   extern GtkWidget *back_end_server_clist;

   be = g_malloc(sizeof(back_end_server));

   if((list_last=g_list_last(a->back_end_servers))==NULL){
      if((list_last=g_list_last(a->ipvs_servers))!=NULL){
         last_ipvs=(ipvs_server *)list_last->data;
      }
   }
   else{
      last_be=(back_end_server *)list_last->data;
   }

   if(strcmp(addr, IP_ADDR_NEXT)){
      be->address=g_strdup(addr);
   }
   else{
      if(last_be==NULL){
         if(last_ipvs==NULL){
            be->address=ip_addr_next(a->address);
         }
         else{
            be->address=ip_addr_next(last_ipvs->address);
         }
      }
      else{
         be->address=ip_addr_next(last_be->address);
      }
   }
   if(strcmp(port, PORT_CURRENT)){
      be->port=g_strdup(port);
   }
   else{
      if(last_be==NULL){
         be->port=g_strdup(a->port);
      }
      else{
         be->port=g_strdup(last_be->port);
      }
   }

   if(weight!=WEIGHT_CURRENT){
      be->weight=weight;
   }
   else{
      if(last_be==NULL){
         be->weight=WEIGHT_DEFAULT;
      }
      else{
         be->weight=last_be->weight;
      }
   }


   a->back_end_servers = g_list_append(a->back_end_servers, be);

   g_snprintf(s_weight, sizeof(s_weight), "%i", be->weight);

   s_text[0] = be->address;
   s_text[1] = be->port;
   s_text[2] = s_weight;
   gtk_clist_append(GTK_CLIST(back_end_server_clist), s_text);
}


/**********************************************************************
 * add_ipvs_server
 * Add a IPVS server
 * Pre: a: advertised service to add the IPVS server to
 *      addr: IP address or hostname of IPVS server
 *            If IP_ADDR_NEXT is used then
 *              If there are any IPVS servers defined then
 *              the address of the last one in the list is taken
 *              and incremented and used
 *              Else if there are any back end servers defined then
 *              the address of the last on in the list is taken
 *              and incremented and used
 *              Else the address of the advertised service is taken
 *              and incremented and used
 *            Else the address supplied is used
 * Post: New IPVS server is created
 * Return: None
 * Note: Incrementing will loop, so 255.255.255.255 + 1 = 0.0.0.0
 *       If the IPVS server, back end server or advertised service
 *       used as the base address for a new IPVS server has a
 *       hostname instead then an attempt will be made to resolve
 *       this. If this is succesul then the resulting IP address
 *       - the first on returned - will be incremented and used, 
 *       else a blank address will result.
 **********************************************************************/

void add_ipvs_server(advertised_service *a, char *addr){
   back_end_server *last_be=NULL;
   ipvs_server *last_ipvs=NULL;
   ipvs_server *ipvs;
   gchar *s_text[1];
   GList *list_last;

   extern GtkWidget *ipvs_server_clist;

   ipvs = g_malloc(sizeof(ipvs_server));

   if((list_last=g_list_last(a->ipvs_servers))==NULL){
      if((list_last=g_list_last(a->back_end_servers))!=NULL){
         last_be=(back_end_server *)list_last->data;
      }
   }
   else{
      last_ipvs=(ipvs_server *)list_last->data;
   }

   if(strcmp(addr, IP_ADDR_NEXT)){
      ipvs->address=g_strdup(addr);
   }
   else{
      if(last_ipvs==NULL){
         if(last_be==NULL){
            ipvs->address=ip_addr_next(a->address);
         }
         else{
            ipvs->address=ip_addr_next(last_be->address);
         }
      }
      else{
         ipvs->address=ip_addr_next(last_ipvs->address);
      }
   }

   a->ipvs_servers = g_list_append(a->ipvs_servers, ipvs);

   s_text[0] = ipvs->address;
   gtk_clist_append(GTK_CLIST(ipvs_server_clist), s_text);
}


/**********************************************************************
 * fill_clist
 * For a given advertised service fill in the relevant gui lists
 * so the display is updated to reflect the components of the
 * advertised service
 * Pre: a: The advertised service to update the lists for
 * Post: Lists that controls the display elememnt of the advertised
 *       service are updated
 * Return: none
 **********************************************************************/

void fill_clist(advertised_service *a) {
  fill_back_end_server_clist(a);
  fill_ipvs_server_clist(a);
}


/**********************************************************************
 * fill_back_end_server_clist
 * For a given advertised service fill in the gui list for the
 * back end servers so the display is updated to reflect the
 * back end servers for an advertised service
 * Pre: a: The advertised service to show the back end servers for
 * Post: List that controls the display of back end servers
 *       is updated
 * Return: none
 **********************************************************************/

void fill_back_end_server_clist(advertised_service *a) {
   extern GtkWidget *back_end_server_clist;

   gtk_clist_freeze(GTK_CLIST(back_end_server_clist));
   gtk_clist_clear(GTK_CLIST(back_end_server_clist));
   if (a && a->back_end_servers) {
      GList *l;

      for(l=a->back_end_servers;l;l=l->next){
         back_end_server *be;
         gchar s_weight[32], *s_text[3];

         be=l->data;
         g_snprintf(s_weight, sizeof(s_weight), "%i", be->weight);

         s_text[0] = be->address;
         s_text[1] = be->port;
         s_text[2] = s_weight;
         gtk_clist_append(GTK_CLIST(back_end_server_clist), s_text);	     
      }
   }
   gtk_clist_thaw(GTK_CLIST(back_end_server_clist));
}


/**********************************************************************
 * fill_ipvs_server_clist
 * For a given advertised service fill in the gui list for the
 * IPVS servers so the display is updated to reflect the
 * IPVS servers for an advertised service
 * Pre: a: The advertised service to show the IPVS servers for
 * Post: List that controls the display of IPVS servers
 *       is updated
 * Return: none
 **********************************************************************/

void fill_ipvs_server_clist(advertised_service *a) {
   extern GtkWidget *ipvs_server_clist;

   gtk_clist_freeze(GTK_CLIST(ipvs_server_clist));
   gtk_clist_clear(GTK_CLIST(ipvs_server_clist));
   if (a && a->ipvs_servers) {
      GList *l;

      for(l=a->ipvs_servers;l;l=l->next){
         ipvs_server *server;
         gchar *s_text[1];

         server=l->data;

         s_text[0] = server->address;
         gtk_clist_append(GTK_CLIST(ipvs_server_clist), s_text);	     
      }
   }
   gtk_clist_thaw(GTK_CLIST(ipvs_server_clist));
}


/**********************************************************************
 * update_advertised_service
 * Update a row in the the advertised service clist after changes have 
 * been made
 * Pre: a: advertised service to update
 * Post: clist entry for a modified advertised service is updated
 * Return: none
 **********************************************************************/

void update_advertised_service(advertised_service *a){
   GList *l;
   gint i;
   gchar *s_text[4];

   extern gchar *s_methods[];
   extern gchar *s_protocols[];
   extern GList *service;
   extern GtkWidget *advertised_service_clist;

   for(l=service,i=0;l;l=l->next,i++){
      if(l->data==a){
         s_text[0] = a->address;
         s_text[1] = a->port;
         s_text[2] = s_methods[a->method];
         s_text[3] = s_protocols[a->protocol];
         gtk_clist_freeze(GTK_CLIST(advertised_service_clist));
         gtk_clist_set_text(GTK_CLIST(advertised_service_clist),i,0,s_text[0]);
         gtk_clist_set_text(GTK_CLIST(advertised_service_clist),i,1,s_text[1]);
         gtk_clist_set_text(GTK_CLIST(advertised_service_clist),i,2,s_text[2]);
         gtk_clist_set_text(GTK_CLIST(advertised_service_clist),i,3,s_text[3]);
         gtk_clist_thaw(GTK_CLIST(advertised_service_clist));
      }
   }
}


/**********************************************************************
 * update_back_end_server
 * Update a row in the the back end server clist after changes have 
 * been made
 * Pre: a: advertised service that the back end server belogns to
 *      be: back end server that has been modified
 * Post: clist entry for a modified back end server is updated
 * Return: none
 **********************************************************************/

void update_back_end_server(advertised_service *a, back_end_server *be){
   GList *l;
   gint i;
   gchar *s_text[3];

   extern GtkWidget *back_end_server_clist;

   for(l=a->back_end_servers,i=0;l;l=l->next,i++){
      if(l->data==be){
         gchar s_weight[32];

         g_snprintf(s_weight, sizeof(s_weight), "%i", be->weight);
         s_text[0] = be->address;
         s_text[1] = be->port;
         s_text[2] = s_weight;
         gtk_clist_freeze(GTK_CLIST(back_end_server_clist));
         gtk_clist_set_text(GTK_CLIST(back_end_server_clist),i,0,s_text[0]);
         gtk_clist_set_text(GTK_CLIST(back_end_server_clist),i,1,s_text[1]);
         gtk_clist_set_text(GTK_CLIST(back_end_server_clist),i,2,s_text[2]);
         gtk_clist_thaw(GTK_CLIST(back_end_server_clist));
      }
   }
}


/**********************************************************************
 * update_ipvs_server
 * Update a row in the the IPVS server clist after changes have 
 * been made
 * Pre: a: advertised service that the IPVS server belogns to
 *      be: IPVS server that has been modified
 * Post: clist entry for a modified IPVS server is updated
 * Return: none
 **********************************************************************/

void update_ipvs_server(advertised_service *a, ipvs_server *ipvs){
   GList *l;
   gint i;
   gchar *s_text[3];

   extern GtkWidget *ipvs_server_clist;

   for(l=a->ipvs_servers,i=0;l;l=l->next,i++){
      if(l->data==ipvs){

         s_text[0] = ipvs->address;
         gtk_clist_freeze(GTK_CLIST(ipvs_server_clist));
         gtk_clist_set_text(GTK_CLIST(ipvs_server_clist),i,0,s_text[0]);
         gtk_clist_thaw(GTK_CLIST(ipvs_server_clist));
      }
   }
}


/**********************************************************************
 * ip_addr_next
 * Return the numerically next IP address
 * Pre: addr: Address either as an UP address or hostname
 * Post: none
 * Return: numerically next address
 * Note: If addr is a hostname then an attempt will be made to resolve
 *       this. If this is succesul then the resulting IP address
 *       - the first on returned - will be used, else a blank address 
 *       will result.
 **********************************************************************/

gchar *ip_addr_next(gchar *addr){
   struct in_addr in;
   struct hostent *hp;

   if((hp=gethostbyname(addr))==NULL){
      return(NULL);
   }
   bcopy(hp->h_addr, &in.s_addr, hp->h_length);
   in.s_addr=htonl(ntohl(in.s_addr)+1);
   return(g_strdup(inet_ntoa(in)));
}


