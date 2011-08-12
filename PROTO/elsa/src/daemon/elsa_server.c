#include <Ecore_Con.h>
#include "elsa.h"
#include "../event/elsa_event.h"

static Eina_Bool _elsa_server_add(void *data, int type, void *event);
static Eina_Bool _elsa_server_del(void *data, int type, void *event);
static Eina_Bool _elsa_server_data(void *data, int type, void *event);


Ecore_Con_Server *_elsa_server = NULL;
Eina_List *_handlers = NULL;


static Eina_Bool
_my_hack2(void *data)
{
   Elsa_Event eev;
   void *enc;
   int size;
   eev.type = ELSA_EVENT_ACTIONS;
   eev.event.actions.actions = elsa_action_get();
   enc = elsa_event_encode(&eev, &size);
   ecore_con_client_send(data, enc, size);
//   ecore_con_client_flush(ev->client);
   return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_my_hack(void *data)
{
   Elsa_Event eev;
   void *enc;
   int size;

   eev.type = ELSA_EVENT_XSESSIONS;
   if (elsa_config->xsessions)
     {
        eev.event.xsessions.xsessions = elsa_session_list_get();
        enc = elsa_event_encode(&eev, &size);
        ecore_con_client_send(data, enc, size);
     }
   ecore_timer_add(0.5, _my_hack2, data);
//   ecore_con_client_flush(ev->client);
   return ECORE_CALLBACK_CANCEL;
}


static Eina_Bool
_elsa_server_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Client_Add *ev;
   Elsa_Event eeu;
   ev = event;
   void *enc;
   int size;

   fprintf(stderr, PACKAGE": server client connected\n");
   eeu.type = ELSA_EVENT_USERS;
   eeu.event.users.users = elsa_history_get();
   enc = elsa_event_encode(&eeu, &size);
   ecore_con_client_send(ev->client, enc, size);
//   ecore_con_client_flush(ev->client);
   ecore_timer_add(0.5, _my_hack, ev->client);
   /*
   eev.type = ELSA_EVENT_XSESSIONS;
   if (elsa_config->xsessions)
     {
        eev.event.xsessions.xsessions = elsa_session_list_get();
        enc = elsa_event_encode(&eev, &size);
        ecore_con_client_send(data, enc, size);
     }
     */
   return ECORE_CALLBACK_RENEW;
}


static Eina_Bool
_elsa_server_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Client_Del *ev;
   ev = event;
   fprintf(stderr, PACKAGE": server client disconnected\n");

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_elsa_server_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{

   Ecore_Con_Event_Client_Data *ev;
   Elsa_Event *eev, neev;
   int size;
   void *enc;

   ev = event;
   eev = elsa_event_decode(ev->data, ev->size);
   if (eev->type == ELSA_EVENT_AUTH)
     {
        neev.type = ELSA_EVENT_STATUS;
        if (elsa_session_authenticate(eev->event.auth.login,
                                      eev->event.auth.password))
          {
             elsa_session_login(eev->event.auth.session);
             neev.event.status.granted = EINA_TRUE;
          }
        else
          {
             neev.event.status.granted = EINA_FALSE;
             fprintf(stderr, PACKAGE": server authenticate error\n");
          }
        enc = elsa_event_encode(&neev, &size);
        ecore_con_client_send(ev->client, enc, size);
     }
   else if (eev->type == ELSA_EVENT_ACTION)
     elsa_action_run(eev->event.action.action);
   else
     fprintf(stderr, PACKAGE": UNKNOW signal server\n");

   return ECORE_CALLBACK_RENEW;
}

void
elsa_server_init()
{
   Ecore_Event_Handler *h;
   ecore_con_init();
   fprintf(stderr, PACKAGE": server init\n");
   _elsa_server = ecore_con_server_add(ECORE_CON_LOCAL_SYSTEM,
                                        "elsa", 42, NULL);
   if (_elsa_server)
     fprintf(stderr, PACKAGE": server init ok\n");
   else
     fprintf(stderr, PACKAGE": server init fail\n");

   h = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD,
                               _elsa_server_add, NULL);
   _handlers = eina_list_append(_handlers, h);
   h = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL,
                               _elsa_server_del, NULL);
   _handlers = eina_list_append(_handlers, h);
   h = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA,
                               _elsa_server_data, NULL);
   _handlers = eina_list_append(_handlers, h);
}

void
elsa_server_shutdown()
{
   Ecore_Event_Handler *h;
   fprintf(stderr, PACKAGE": server shutdown\n");
   if (_elsa_server)
     ecore_con_server_del(_elsa_server);
   EINA_LIST_FREE(_handlers, h)
     ecore_event_handler_del(h);
   ecore_con_shutdown();
}

