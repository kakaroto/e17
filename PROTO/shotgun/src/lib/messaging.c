#include <Ecore.h>
#include "shotgun_private.h"
#include "xml.h"

static void
shotgun_message_free(void *data __UNUSED__, Shotgun_Event_Message *msg)
{
   free(msg->msg);
   eina_stringshare_del(msg->jid);
   free(msg);
}

Shotgun_Event_Message *
shotgun_message_new(Shotgun_Auth *auth)
{
   Shotgun_Event_Message *msg;
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);

   msg = calloc(1, sizeof(Shotgun_Event_Message));
   msg->account = auth;
   return msg;
}

void
shotgun_message_feed(Shotgun_Auth *auth, char *data, size_t size)
{
   Shotgun_Event_Message *msg;

   msg = xml_message_read(auth, data, size);
   EINA_SAFETY_ON_NULL_GOTO(msg, error);

   INF("Message from %s: %s", msg->jid, msg->msg);
   ecore_event_add(SHOTGUN_EVENT_MESSAGE, msg, (Ecore_End_Cb)shotgun_message_free, NULL);
   return;
error:
   ERR("wtf");
}

void
shotgun_event_message_free(Shotgun_Event_Message *msg)
{
   if (!msg) return;
   shotgun_message_free(NULL, msg);
}

Eina_Bool
shotgun_message_send(Shotgun_Auth *auth, const char *to, const char *msg, Shotgun_Message_Status status)
{
   size_t len;
   char *xml;

   xml = xml_message_write(auth, to, msg, status, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}
