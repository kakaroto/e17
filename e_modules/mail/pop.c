#include <e.h>
#include "e_mod_main.h"
#include "pop.h"

static Ecore_Event_Handler *add_handler;
static Ecore_Event_Handler *del_handler;
static Ecore_Event_Handler *data_handler;

static int _mail_pop_server_add(void *data, int type, void *event);
static int _mail_pop_server_del(void *data, int type, void *event);
static int _mail_pop_server_data(void *data, int type, void *event);

void
_mail_pop_check_mail(void *data, void *data2) 
{
   PopClient *pc;
   Ecore_Con_Type type = ECORE_CON_REMOTE_SYSTEM;
   Config_Box *cb;
   
   cb = data2;
   if (!cb) return;
   
   pc = E_NEW(PopClient, 1);
   pc->data = data;
   pc->config = cb;
   pc->config->num_new = 0;
   pc->config->num_total = 0;
   add_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _mail_pop_server_add, pc);
   del_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _mail_pop_server_del, pc);
   data_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, _mail_pop_server_data, pc);
   if ((ecore_con_ssl_available_get()) && (cb->ssl))
     type |= ECORE_CON_USE_SSL;   
   pc->server = ecore_con_server_connect(type, cb->host, cb->port, NULL);
   pc->state = POP_STATE_DISCONNECTED;
}

/* PRIVATES */
static int 
_mail_pop_server_add(void *data, int type, void *event) 
{
   Ecore_Con_Event_Server_Add *ev = event;
   PopClient *pc;
   
   if (!data) return 1;
   pc = data;
   pc->state = POP_STATE_CONNECTED;
   return 0;
}

static int 
_mail_pop_server_del(void *data, int type, void *event) 
{
   Ecore_Con_Event_Server_Del *ev = event;
   PopClient *pc;
   
   pc = data;
   if (!pc) return 1;

   ecore_con_server_del(pc->server);
   pc->server = NULL;

   if (add_handler)
     ecore_event_handler_del(add_handler);
   if (del_handler)
     ecore_event_handler_del(del_handler);
   if (data_handler)
     ecore_event_handler_del(data_handler);

   return 0;
}

static int 
_mail_pop_server_data(void *data, int type, void *event) 
{
   Ecore_Con_Event_Server_Data *ev = event;
   PopClient *pc;
   char in[2048], out[2048];
   int len, num = 0, total = 0;
   
   if (!data) return 1;

   pc = data;
   if ((!pc->server) || (pc->server != ev->server)) return 1;
   
   len = sizeof(in) -1;
   len = (((len) > (ev->size)) ? ev->size : len);
   memcpy(in, ev->data, len);
   in[len] = 0;
   
   if (!strncmp(in, "-ERR", 4)) 
     {
	printf("ERROR: %s\n", in);
	pc->state = POP_STATE_DISCONNECTED;
	ecore_con_server_del(ev->server);
	pc->server = NULL;
	return 0;
     }
   else if (strncmp(in, "+OK", 3)) 
     {
	printf("Unexpected reply: %s\n", in);
	pc->state = POP_STATE_DISCONNECTED;
	ecore_con_server_del(ev->server);
	pc->server = NULL;
	return 0;
     }

   if (pc->state == POP_STATE_CONNECTED)
     pc->state++;
   
   switch (pc->state) 
     {
      case POP_STATE_SERVER_READY:
	len = snprintf(out, sizeof(out), "USER %s\r\n", pc->config->user);
	ecore_con_server_send(ev->server, out, len);
	pc->state = POP_STATE_USER_OK;
	break;
      case POP_STATE_USER_OK:
	len = snprintf(out, sizeof(out), "PASS %s\r\n", pc->config->pass);
	ecore_con_server_send(ev->server, out, len);
	pc->state = POP_STATE_PASS_OK;
	break;
      case POP_STATE_PASS_OK:
	len = snprintf(out, sizeof(out), "STAT\r\n");
	ecore_con_server_send(ev->server, out, len);
	pc->state = POP_STATE_STATUS_OK;
	break;
      case POP_STATE_STATUS_OK:
	if (sscanf(in, "+OK %i %i", &num, &total) == 2) 
	  {
	     pc->config->num_new = num;
	     pc->config->num_total = num;
	  }
	
	_mail_set_text(pc->data, num);
	
	ecore_con_server_del(ev->server);
	pc->state = POP_STATE_DISCONNECTED;
	pc->server = NULL;
	break;
      default:
	break;
     }   
   return 0;   
}
