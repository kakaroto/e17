#include <Ecore_Con.h>
#include "elsa_client.h"
#include "elsa_gui.h"
#include "../daemon/elsa_server.h"

Ecore_Con_Server *_elsa_connect;
Eina_List *_handlers;

#define ELSA_EVENT_DESKTOP_REQUEST 1
#define ELSA_EVENT_DESKTOP

static void _elsa_connect_xserver_get(const void *data , const int size);
//static void _elsa_connect_send_auth(Ecore_Con_Server *server, const char *login, const char *password, const char *session);
static Eina_Bool _elsa_connect_add(void *data, int type, void *event);
static Eina_Bool _elsa_connect_del(void *data, int type, void *event);
static Eina_Bool _elsa_connect_data(void *data, int type, void *event);

typedef struct Elsa_Xsession_
{
   const char *name;
   const char *command;
   const char *icon;
} Elsa_Xsession;

typedef struct Elsa_Xsession_Send_
{
   Eina_List *xsessions;
} Elsa_Xsession_Send;

typedef struct Elsa_Auth_Send_
{
   const char *login;
   const char *password;
   const char *session;
} Elsa_Auth_Send;

typedef struct Elsa_Server_Event_
{
   int type;
   void *data;
} Elsa_Server_Event;

static Eina_Bool
_elsa_connect_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Server_Add *ev;
   ev = event;
   fprintf(stderr, PACKAGE": client connected\n");

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_elsa_connect_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Server_Del *ev;
   ev = event;
   fprintf(stderr, PACKAGE": client disconnected\n");
//   ecore_con_server_del(_elsa_connect);
   _elsa_connect = NULL;
   elm_exit();

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_elsa_connect_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Server_Data *ev;
   ev = event;

   fprintf(stderr, PACKAGE": client data received %d\n", ev->size);
   _elsa_connect_xserver_get(ev->data, ev->size);

   return ECORE_CALLBACK_RENEW;
}

static void
_elsa_connect_xserver_get(const void *data , const int size)
{
   Eet_Data_Descriptor *edd, *eddl;
   Eet_Data_Descriptor_Class eddc, eddcl;
   Elsa_Xsession_Send *exs;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddcl, Elsa_Xsession);
   edd = eet_data_descriptor_stream_new(&eddcl);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Xsession, "name",
                                 name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Xsession, "icon",
                                 icon, EET_T_STRING);

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Xsession_Send);
   eddl = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_LIST(eddl, Elsa_Xsession_Send, "xsessions", xsessions, edd);

   exs = eet_data_descriptor_decode(eddl, data, size);
   elsa_gui_xsession_set(exs->xsessions);
}

void
elsa_connect_auth_send(const char *login, const char *password, const char *session)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;
   Elsa_Auth_Send *eas;
   void *data;
   int size;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Auth_Send);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Auth_Send, "login",
                                 login, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Auth_Send, "password",
                                 password, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Auth_Send, "session",
                                 session, EET_T_STRING);
   eas = (Elsa_Auth_Send *) malloc(sizeof(Elsa_Auth_Send));
   eas->login = login;
   eas->password = password;
   eas->session = session;

   data = eet_data_descriptor_encode(edd, eas, &size);

   ecore_con_server_send(_elsa_connect, data, size);
   elm_exit();
}

void
elsa_connect_init()
{
   Ecore_Event_Handler *h;
   ecore_con_init();
   _elsa_connect = ecore_con_server_connect(ECORE_CON_LOCAL_SYSTEM,
                                            "elsa", 42, NULL);
   if (_elsa_connect)
     printf(PACKAGE": client server init ok\n");
   else
     printf(PACKAGE": client server init fail\n");
   h = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
                               _elsa_connect_add, NULL);
   _handlers = eina_list_append(_handlers, h);
   h = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
                               _elsa_connect_del, NULL);
   _handlers = eina_list_append(_handlers, h);
   h = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                               _elsa_connect_data, NULL);
   _handlers = eina_list_append(_handlers, h);
}

void
elsa_connect_shutdown()
{
   Ecore_Event_Handler *h;
   printf(PACKAGE": client server shutdown\n");
   EINA_LIST_FREE(_handlers, h)
      ecore_event_handler_del(h);
   ecore_con_shutdown();
}

