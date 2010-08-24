#include "irc.h"
#include "emote_private.h"

#include <Ecore.h>
#include <Ecore_Con.h>

EMAPI Emote_Protocol_Api protocol_api =
{
   /* version, name, label */
   EMOTE_PROTOCOL_API_VERSION, "irc", "IRC"
};

static Eina_Bool _irc_cb_server_add(void *data, int type __UNUSED__, void *event);
static Eina_Bool _irc_cb_server_del(void *data, int type __UNUSED__, void *event);
static Eina_Bool _irc_cb_server_data(void *data, int type __UNUSED__, void *event);
static Eina_Bool _irc_event_handler(void *data __UNUSED__, int type, void *event);

static Emote_Protocol *m;
static Eina_Hash *_irc_servers = NULL;

EMAPI int
protocol_init(Emote_Protocol *p)
{
   m = p;
   ecore_con_init();
   _irc_servers = eina_hash_string_superfast_new(NULL);
   return 1;
}

EMAPI int
protocol_shutdown(void)
{
   ecore_con_shutdown();
   if (_irc_servers)
     eina_hash_free(_irc_servers);
   return 1;
}

EMAPI int
protocol_connect(const char *server, int port, const char *user, const char *pass)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server)))
     {
        if (port <= 0)
          port = 6667;
        serv = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
                                        server, port, NULL);
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
                                _irc_cb_server_add, server);
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
                                _irc_cb_server_del, server);
        ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                                _irc_cb_server_data, server);
        eina_hash_add(_irc_servers, server, serv);
     }
   return 1;
}

EMAPI int
protocol_disconnect(const char *server)
{
   Ecore_Con_Server *serv = NULL;
   char buf[512];
   int len = 0;

   if (!server) return 0;
   if (!(serv = eina_hash_find(_irc_servers, server))) return 0;
   if (!ecore_con_server_connected_get(serv)) return 0;
   len = snprintf(buf, sizeof(buf), "QUIT\r\n");
   ecore_con_server_send(serv, buf, len);
   ecore_con_server_flush(serv);
   ecore_con_server_del(serv);
   return 1;
}

static Eina_Bool 
_irc_cb_server_add(void *data, int type __UNUSED__, void *event)
{
   const char *server;
   Emote_Event_Chat_Server *d;

   if (!(server = data)) return EINA_FALSE;
   d = EMOTE_NEW(Emote_Event_Chat_Server, 1);
   d->protocol = m;
   d->server = server;
   printf("IRC: Server Added\n");
   emote_event_send(EMOTE_EVENT_CHAT_SERVER_CONNECTED, d); 
   return EINA_FALSE;
}

static Eina_Bool 
_irc_cb_server_del(void *data, int type __UNUSED__, void *event)
{
   const char *server;
   Emote_Event_Chat_Server *d;

   if (!(server = data)) return EINA_FALSE;
   d = EMOTE_NEW(Emote_Event_Chat_Server, 1);
   d->protocol = m;
   d->server = server;
   printf("IRC: Server Deleted\n");
   eina_hash_del_by_key(_irc_servers, server);
   emote_event_send(EMOTE_EVENT_CHAT_SERVER_DISCONNECTED, d);
   return EINA_FALSE;
}

static Eina_Bool 
_irc_cb_server_data(void *data, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Server_Data *ev;
   const char *server;
   char *msg;

   ev = event;
   server = data;
   msg = calloc((ev->size + 1), sizeof(char));
   strncpy(msg, ev->data, ev->size);
   irc_parse_input(msg, server);
   free(msg);
   return EINA_FALSE;
}

static Eina_Bool
_irc_event_handler(void *data __UNUSED__, int type, void *event)
{
   return EINA_TRUE;
}
