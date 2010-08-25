#include "emote_private.h"
#include "Emote.h"

#include <stdarg.h>
#include <unistd.h>

#define EMOTE_EVENT_ALLOC_STRING(type, e, param, data) \
  ((type*)e)->param = ((data != NULL) ? eina_stringshare_add(data) : NULL)

static int _emote_events[EMOTE_EVENT_COUNT];

static void _emote_event_data_free(void *data, void *event);

static void _emote_event_free(void *e);
static void _emote_event_server_free(void *e);
static void _emote_event_server_connect_free(void *e);
static void _emote_event_server_message_free(void *e);
static void _emote_event_chat_channel_free(void *e);
static void _emote_event_chat_channel_message_free(void *e);

int
emote_event_init()
{
  unsigned int i;

  for(i = 0; i < sizeof(_emote_events); ++i)
    _emote_events[i] = ecore_event_type_new();

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
        ev = (Emote_Event*)EM_OBJECT_ALLOC(Emote_Event_Server,
                                           EMOTE_EVENT_SERVER_TYPE,
                                           _emote_event_server_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        break;

      // Emote_Event_Server_Connect
      case EMOTE_EVENT_SERVER_CONNECT:
        ev = (Emote_Event*)EM_OBJECT_ALLOC(Emote_Event_Server_Connect,
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
        ev = (Emote_Event*)EM_OBJECT_ALLOC(Emote_Event_Server_Message,
                                           EMOTE_EVENT_SERVER_MESSAGE_TYPE,
                                           _emote_event_server_message_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server_Message, ev, message, s);
        break;

      // Emote_Event_Chat_Channel
      case EMOTE_EVENT_CHAT_CHANNEL_JOIN:
      case EMOTE_EVENT_CHAT_CHANNEL_JOINED:
        ev = (Emote_Event*)EM_OBJECT_ALLOC(Emote_Event_Chat_Channel,
                                           EMOTE_EVENT_CHAT_CHANNEL_TYPE,
                                           _emote_event_chat_channel_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat_Channel, ev, channel, s);
        break;

      // Emote_Event_Chat_Channel Message
      case EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_SEND:
      case EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_RECEIVED:
        ev = (Emote_Event*)EM_OBJECT_ALLOC(Emote_Event_Chat_Channel_Message,
                                           EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_TYPE,
                                           _emote_event_chat_channel_message_free
                                          );

        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Server, ev, server, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat_Channel, ev, channel, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat_Channel_Message, ev, user, s);
        s = va_arg(args, char*);
        EMOTE_EVENT_ALLOC_STRING(Emote_Event_Chat_Channel_Message, ev, message, s);
        break;

      // Unknown Event
      default:
        printf("********Unknown Event!\n");
        return NULL;
   }

   va_end(args);

   if (ev)
     {
       ev->protocol = p;
       ev->type = type;
     }
   else

   printf("Created event %u @ %p\n", ev->type, ev);

   return ev;
}

EMAPI void
emote_event_send(Emote_Event *ev)
{
   printf("Sending Event:\n\tType: %u(%u)\n\tProtocol: %s\n", ev->type, _emote_events[ev->type], ev->protocol->api->label);

   ecore_event_add(_emote_events[ev->type], ev, _emote_event_data_free, (void*)ev);
}

EMAPI void
emote_event_handler_add(int type, Eina_Bool (*cb)(void*,int,void*), void *data)
{
  ecore_event_handler_add(_emote_events[type], cb, data);
}

/* local functions */
static void
_emote_event_data_free(void *data , void *event)
{
  printf("Freeing event %u @ %p\n", EMOTE_EVENT_T(event)->type, event);

  em_object_del(event);
}

static void
_emote_event_free(void *e)
{
   free(e);
}

static void
_emote_event_server_free(void *e)
{
  Emote_Event_Server *ee;

  ee = e;

  printf("Freeing server %s\n", ee->server);

  if (ee->server)
    eina_stringshare_del(ee->server);

  _emote_event_free(e);
}

static void
_emote_event_server_connect_free(void *e)
{
  Emote_Event_Server_Connect *ee;

  ee = e;

  printf("Freeing Server Connect\n");

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

  printf("Freeing Server Message\n");

  if (ee->message)
    eina_stringshare_del(ee->message);

  _emote_event_server_free(e);
}

static void
_emote_event_chat_channel_free(void *e)
{
  Emote_Event_Chat_Channel *ee;

  ee = e;

  printf("Freeing Channel %s\n", ee->channel);

  if (ee->channel)
    eina_stringshare_del(ee->channel);

  _emote_event_server_free(e);
}

static void
_emote_event_chat_channel_message_free(void *e)
{
  Emote_Event_Chat_Channel_Message *ee;

  ee = e;

return;

  printf("Freeing Channel Message %s\n", ee->message);
return;
  if (ee->user)
    eina_stringshare_del(ee->user);

  if (ee->message)
    eina_stringshare_del(ee->message);

  _emote_event_chat_channel_free(e);
}
