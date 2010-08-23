#include "irc.h"
#include "emote_private.h"

#include <Ecore.h>
#include <Ecore_Con.h>

EMAPI Emote_Protocol_Api protocol_api =
{
   /* version, name, label */
   EMOTE_PROTOCOL_API_VERSION, "irc", "IRC"
};

static Eina_Bool _irc_msg_send(void *data, int type, void *event);

static Emote_Protocol *m;

EMAPI int
protocol_init(Emote_Protocol *p)
{
   m = p;
   ecore_event_handler_add(EMOTE_EVENT_MSG_SEND, _irc_msg_send, NULL);
   return 1;
}

EMAPI int
protocol_shutdown(void)
{
   return 1;
}

EMAPI int
protocol_connect(const char *server, int port, const char *user, const char *pass)
{
   return 1;
}

EMAPI int
protocol_disconnect(const char *server)
{
   return 1;
}

static Eina_Bool
_irc_msg_send(void *data, int type __UNUSED__, void *event)
{
   Emote_Event_Data *d;

   d = event;
   if (d->protocol != m) return EINA_TRUE;

   printf("Message Sent To %s (%s)\n", d->protocol->api->label,
          (const char *)d->data);

   emote_event_send(EMOTE_EVENT_MSG_RECEIVED, m, d->data);
   return EINA_TRUE;
}
