#include "emote_private.h"
#include "Emote.h"

#include <stdarg.h>
#include <unistd.h>

#define EMOTE_EVENT_ALLOC_STRING(type, e, param, data) \
  ((type*)e)->param = ((data) ? eina_stringshare_add(data) : NULL)

static int _emote_events[EMOTE_EVENT_COUNT];

static void _emote_event_free(void *data, void *event);

static void _emote_event_server_free(void *e);
static void _emote_event_server_connect_free(void *e);
static void _emote_event_server_message_free(void *e);
static void _emote_event_chat_free(void *e);
static void _emote_event_chat_message_free(void *e);

EM_INTERN int
emote_event_init()
{
  unsigned int i;

  for(i = 0; i < sizeof(_emote_events); ++i)
    _emote_events[i] = ecore_event_type_new();

  return 1;
}

EM_INTERN int
emote_event_shutdown()
{
  return 1;
}

EMAPI Emote_Event*
emote_event_new(Emote_Protocol *p, int type, ...)
{
   Emote_Event *ev;
   va_list args;
   char *s;
   int i;

   va_start(args, type);

   switch(type)
   {
      // Emote_Event_Server
      case EMOTE_EVENT_SERVER_DISCONNECT:
      case EMOTE_EVENT_SERVER_CONNECTED:
      case EMOTE_EVENT_SERVER_DISCONNECTED:
        ev = (Emote_Event*)EMOTE_OBJECT_ALLOC(Emote_Event_Server,
                                           EMOTE_EVENT_SERVER_TYPE,
                                           _emote_event_server_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        break;

      // Emote_Event_Server_Connect
      case EMOTE_EVENT_SERVER_CONNECT:
        ev = (Emote_Event*)EMOTE_OBJECT_ALLOC(Emote_Event_Server_Connect,
                                           EMOTE_EVENT_SERVER_CONNECT_TYPE,
                                           _emote_event_server_connect_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        i = va_arg(args, int);
        EMOTE_EVENT_SERVER_CONNECT_T(ev)->port = i;
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server_Connect, ev, username, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server_Connect, ev, password, s);
        break;

      // Emote_Event_Server_Message
      case EMOTE_EVENT_SERVER_MESSAGE_SEND:
      case EMOTE_EVENT_SERVER_MESSAGE_RECEIVED:
        ev = (Emote_Event*)EMOTE_OBJECT_ALLOC(Emote_Event_Server_Message,
                                           EMOTE_EVENT_SERVER_MESSAGE_TYPE,
                                           _emote_event_server_message_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server_Message, ev, user, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server_Message, ev, message, s);
        break;

      // Emote_Event_Chat
      case EMOTE_EVENT_CHAT_JOIN:
      case EMOTE_EVENT_CHAT_JOINED:
      case EMOTE_EVENT_CHAT_PART:
      case EMOTE_EVENT_CHAT_PARTED:
        ev = (Emote_Event*)EMOTE_OBJECT_ALLOC(Emote_Event_Chat,
                                           EMOTE_EVENT_CHAT_TYPE,
                                           _emote_event_chat_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat, ev, channel, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat, ev, user, s);
        break;

      // Emote_Event_Chat Message
      case EMOTE_EVENT_CHAT_MESSAGE_SEND:
      case EMOTE_EVENT_CHAT_MESSAGE_RECEIVED:
        ev = (Emote_Event*)EMOTE_OBJECT_ALLOC(Emote_Event_Chat_Message,
                                           EMOTE_EVENT_CHAT_MESSAGE_TYPE,
                                           _emote_event_chat_message_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat, ev, channel, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat, ev, user, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat_Message, ev, message, s);
        break;

      // Unknown Event
      default:
        printf("********Unknown Event!\n");
        return NULL;
   }

   va_end(args);

   if (ev)
     {
       EMOTE_EVENT_T(ev)->protocol = p;
       EMOTE_EVENT_T(ev)->type = type;
     }

   return ev;
}

EMAPI void
emote_event_send(Emote_Event *ev)
{
   printf("Sending Event:\n\tType: %u(%u)\n\tProtocol: %s\n", ev->type, _emote_events[ev->type], ev->protocol->api->label);

   ecore_event_add(_emote_events[ev->type], ev, _emote_event_free, (void*)ev);
}

EMAPI void
emote_event_handler_add(int type, Eina_Bool (*cb)(void*,int,void*), void *data)
{
  ecore_event_handler_add(_emote_events[type], cb, data);
}

/* local functions */
static void
_emote_event_free(void *data __UNUSED__, void *event)
{
  emote_object_del(event);
}

static void
_emote_event_server_free(void *e)
{
  Emote_Event_Server *ee;

  ee = e;

  if (ee->server)
    eina_stringshare_del(ee->server);
}

static void
_emote_event_server_connect_free(void *e)
{
  Emote_Event_Server_Connect *ee;

  ee = e;

  if (ee->username)
    eina_stringshare_del(ee->username);

  if (ee->password)
    eina_stringshare_del(ee->password);

  _emote_event_server_free(e);
}


static void
_emote_event_server_message_free(void *e)
{
  Emote_Event_Server_Message *ee;

  ee = e;

  if (ee->message)
    eina_stringshare_del(ee->message);

  _emote_event_server_free(e);
}

static void
_emote_event_chat_free(void *e)
{
  Emote_Event_Chat *ee;

  ee = e;

  if (ee->channel)
    eina_stringshare_del(ee->channel);

  if (ee->user)
    eina_stringshare_del(ee->user);

  _emote_event_server_free(e);
}

static void
_emote_event_chat_message_free(void *e)
{
  Emote_Event_Chat_Message *ee;

  ee = e;

  if (ee->message)
    eina_stringshare_del(ee->message);

  _emote_event_chat_free(e);
}
