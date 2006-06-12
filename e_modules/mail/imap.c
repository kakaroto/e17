#include <e.h>
#include "e_mod_main.h"
#include "imap.h"

static ImapServer *_mail_imap_server_get(Ecore_Con_Server *server);
static ImapClient *_mail_imap_client_get(void *data);
static int _mail_imap_server_add(void *data, int type, void *event);
static int _mail_imap_server_del(void *data, int type, void *event);
static int _mail_imap_server_data(void *data, int type, void *event);
static void _mail_imap_server_logout(ImapServer *is);

static Evas_List *iservers;

void
_mail_imap_check_mail(void *data) 
{
   Ecore_Con_Type type = ECORE_CON_REMOTE_SYSTEM;
   Evas_List *l, *j;

   for (l = iservers; l; l = l->next) 
     {
	ImapServer *is;
	
	is = l->data;
	is->data = data;
	if (!is->server) 
	  {
	     if (!is->add_handler)
	       is->add_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _mail_imap_server_add, NULL);
	     if (!is->del_handler)
	       is->del_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _mail_imap_server_del, NULL);
	     if (!is->data_handler)
	       is->data_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, _mail_imap_server_data, NULL);

	     for (j = is->clients; j; j = j->next) 
	       {
		  ImapClient *ic;
		  
		  ic = j->data;
		  if (!ic->server->server) 
		    {
		       if (ecore_con_ssl_available_get() && (ic->config->ssl))
			 type |= ECORE_CON_USE_SSL;
		       ic->server->server = ecore_con_server_connect(type, ic->config->host, ic->config->port, NULL);
		       ic->server->state = IMAP_STATE_DISCONNECTED;
		       ic->server->cmd = 0;
		    }
	       }
	  }
	is->current = evas_list_nth(is->clients, 0);
     }
}

void
_mail_imap_add_mailbox(void *data) 
{
   ImapClient *ic;
   Config_Box *cb;

   cb = data;
   if (!cb) return;
   ic = _mail_imap_client_get(cb);
   ic->server->clients = evas_list_append(ic->server->clients, ic);
}

void 
_mail_imap_del_mailbox(void *data) 
{
   ImapClient *ic;
   Config_Box *cb;
   
   cb = data;
   if (!cb) return;
   ic = _mail_imap_client_get(cb);
   ic->server->clients = evas_list_remove(ic->server->clients, ic);
}

void
_mail_imap_shutdown() 
{

   if (!iservers) return;
   while (iservers) 
     {
	ImapServer *is;
	
	is = iservers->data;
	if (is->add_handler)
	  ecore_event_handler_del(is->add_handler);
	if (is->del_handler)
	  ecore_event_handler_del(is->del_handler);
	if (is->data_handler)
	  ecore_event_handler_del(is->data_handler);	
	iservers = evas_list_remove_list(iservers, iservers);
	free(is);
     }
}

/* PRIVATES */
static ImapServer *
_mail_imap_server_get(Ecore_Con_Server *server) 
{
   Evas_List *l;
   
   for (l = iservers; l; l = l->next) 
     {
	ImapServer *is;
	
	is = l->data;
	if (is->server == server)
	  return is;
     }
   return NULL;
}

static ImapClient *
_mail_imap_client_get(void *data) 
{
   ImapServer *is;
   ImapClient *ic;
   Config_Box *cb;
   Evas_List *l, *j;
   int found = 0;
   
   cb = data;
   if (!cb) return NULL;

   if ((!iservers) || (evas_list_count(iservers) <= 0)) 
     {
	is = E_NEW(ImapServer, 1);
	is->server = NULL;
	is->cmd = 0;
	is->state = IMAP_STATE_DISCONNECTED;
	iservers = evas_list_append(iservers, is);
     }
   
   for (l = iservers; l; l = l->next) 
     {
	is = l->data;
	if (!is->clients) continue;
	for (j = is->clients; j; j = j->next) 
	  {
	     ic = j->data;
	     if (!ic->config) continue;
	     if ((!strcmp(ic->config->host, cb->host)) && 
		 (!strcmp(ic->config->user, cb->user)) && 
		 (!strcmp(ic->config->pass, cb->pass)))
	       {
		  found = 1;
		  break;
	       }
	  }
	if (found) break;
     }
   if (!found) 
     {
	ic = E_NEW(ImapClient, 1);
	ic->config = cb;
	ic->server = is;
	ic->config->num_new = 0;
	ic->config->num_total = 0;
     }
   return ic;
}

static int 
_mail_imap_server_add(void *data, int type, void *event) 
{
   Ecore_Con_Event_Server_Add *ev = event;
   ImapServer *is;
   
   is = _mail_imap_server_get(ev->server);
   if (!is) return 1;

   is->state = IMAP_STATE_CONNECTED;
   is->cmd = 0;
   return 0;
}

static int 
_mail_imap_server_del(void *data, int type, void *event) 
{
   Ecore_Con_Event_Server_Del *ev = event;
   ImapServer *is;
   Instance *inst;
   
   is = _mail_imap_server_get(ev->server);
   if (!is) return 1;
   
   if (is->state == IMAP_STATE_DISCONNECTED)
     printf("Imap Server Disconnected\n");
   else
     is->state = IMAP_STATE_DISCONNECTED;
   
   ecore_con_server_del(is->server);
   is->server = NULL;
   inst = is->data;
   inst->count = is->count;
   _mail_set_text(inst);
   
   return 0;
}

static int 
_mail_imap_server_data(void *data, int type, void *event) 
{
   Ecore_Con_Event_Server_Data *ev = event;
   ImapServer *is;
   ImapClient *ic;
   Instance *inst;
   int len, num = 0, total = 0;
   char in[1024], out[1024], *spc;
   size_t slen;
   
   is = _mail_imap_server_get(ev->server);
   if (!is) return 1;
   if (is->state == IMAP_STATE_DISCONNECTED) return 1;
   
   len = sizeof(in) - 1;
   len = (len > ev->size) ? (ev->size) : (len);

   memcpy(in, ev->data, len);
   in[len] = 0;

   if (spc = strchr(in, ' ')) 
     {
	slen = strlen(spc);
	if ((slen > 5) && (!strncmp(spc + 1, "NO ", 3))) 
	  {
	     _mail_imap_server_logout(is);
	     printf("Imap Failure: %s\n", spc + 4);
	     return 0;
	  }
	else if ((slen > 6) && (!strncmp(spc + 1, "BAD ", 4))) 
	  {
	     _mail_imap_server_logout(is);
	     printf("Imap Bad Command: %s\n", spc + 5);
	     return 0;
	  }
     }

   ic = is->current;
   is->state++;
   
   switch (is->state) 
     {
      case IMAP_STATE_SERVER_READY:
	len = snprintf(out, sizeof(out), "A%03i LOGIN %s %s\r\n", ++is->cmd, ic->config->user, ic->config->pass);
	ecore_con_server_send(ev->server, out, len);
	break;
      case IMAP_STATE_LOGGED_IN:
	len = snprintf(out, sizeof(out), "A%03i STATUS %s (MESSAGES UNSEEN)\r\n", ++is->cmd, ic->config->new_path);
	ecore_con_server_send(ev->server, out, len);
	break;
      case IMAP_STATE_STATUS_OK:
	if (sscanf(in, "* STATUS %*s (MESSAGES %i UNSEEN %i)", &total, &num) == 2) 
	  {
	     is->count += num;
	     ic->config->num_new = num;
	     ic->config->num_total = total;
	     inst = is->data;
	     inst->count = num;
	     _mail_set_text(inst); 
	     if ((num > 0) && (ic->config->use_exec) && (ic->config->exec))
	       _mail_start_exe(ic->config);
	     
	     is->clients = evas_list_next(is->clients);
	     if (is->clients) 
	       {		  
		  is->current = is->clients->data;
		  if (is->current) 
		    {
		       ic = is->current;
		       is->state = IMAP_STATE_SERVER_READY;
		    }
		  else 
		    {
		       _mail_imap_server_logout(is);
		       ic = NULL;
		    }
	       }
	     else 
	       {
		  _mail_imap_server_logout(is);
		  ic = NULL;		  
	       }	     
	  }
	if (!ic) break;
      default:
	break;
     }
   return 0;
}

static void 
_mail_imap_server_logout(ImapServer *is) 
{
   int len;
   char out[1024];
   
   if (!is) return;
   
   if (is->state >= IMAP_STATE_LOGGED_IN) 
     {
	len = snprintf(out, sizeof(out), "A%03i LOGOUT", ++is->cmd);
	ecore_con_server_send(is->server, out, len);
     }
   ecore_con_server_del(is->server);
   is->server = NULL;
   is->state = IMAP_STATE_DISCONNECTED;
}
