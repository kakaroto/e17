#include "shotgun_private.h"
#include "xml.h"

static void
shotgun_presence_free(void *d __UNUSED__, Shotgun_Event_Presence *pres)
{
   eina_stringshare_del(pres->jid);
   eina_stringshare_del(pres->photo);
   eina_stringshare_del(pres->description);
   free(pres);
}

Shotgun_Event_Presence *
shotgun_presence_new(Shotgun_Auth *auth)
{
   Shotgun_Event_Presence *pres;

   pres = calloc(1, sizeof(Shotgun_Event_Presence));
   pres->account = auth;
   return pres;
}

void
shotgun_presence_feed(Shotgun_Auth *auth, char *data, size_t size)
{
   Shotgun_Event_Presence *pres;

   pres = xml_presence_read(auth, data, size);
   EINA_SAFETY_ON_NULL_GOTO(pres, error);

   if (!pres->type)
     switch (pres->status)
       {
        case SHOTGUN_USER_STATUS_NORMAL:
          INF("Presence 'normal' from %s: %s", pres->jid, pres->description ? pres->description : "");
          break;
        case SHOTGUN_USER_STATUS_AWAY:
          INF("Presence 'away' from %s: %s", pres->jid, pres->description ? pres->description : "");
          break;
        case SHOTGUN_USER_STATUS_CHAT:
          INF("Presence 'chat' from %s: %s", pres->jid, pres->description ? pres->description : "");
          break;
        case SHOTGUN_USER_STATUS_DND:
          INF("Presence 'dnd' from %s: %s", pres->jid, pres->description ? pres->description : "");
          break;
        case SHOTGUN_USER_STATUS_XA:
          INF("Presence 'xa' from %s: %s", pres->jid, pres->description ? pres->description : "");
          break;
        default:
          INF("Presence 'unavailable' from %s: %s", pres->jid, pres->description ? pres->description : "");
       }
   ecore_event_add(SHOTGUN_EVENT_PRESENCE, pres, (Ecore_End_Cb)shotgun_presence_free, NULL);
   return;
error:
   ERR("wtf");
}

void
shotgun_event_presence_free(Shotgun_Event_Presence *pres)
{
   if (!pres) return;
   shotgun_presence_free(NULL, pres);
}

void
shotgun_presence_set(Shotgun_Auth *auth, Shotgun_User_Status st, const char *desc, int priority)
{
   auth->priority = priority;
   eina_stringshare_replace(&auth->desc, desc);
   auth->status = st;
}

Shotgun_User_Status
shotgun_presence_status_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, SHOTGUN_USER_STATUS_NONE);
   return auth->status;
}

void
shotgun_presence_status_set(Shotgun_Auth *auth, Shotgun_User_Status status)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->status = status;
}

int
shotgun_presence_priority_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, -1);
   return auth->priority;
}

void
shotgun_presence_priority_set(Shotgun_Auth *auth, int priority)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->priority = priority;
}

const char *
shotgun_presence_desc_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);
   return auth->desc;
}

void
shotgun_presence_desc_set(Shotgun_Auth *auth, const char *desc)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   eina_stringshare_replace(&auth->desc, desc);
}

void
shotgun_presence_desc_manage(Shotgun_Auth *auth, const char *desc)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   eina_stringshare_del(auth->desc);
   auth->desc = desc;
}

Eina_Bool
shotgun_presence_send(Shotgun_Auth *auth)
{
   size_t len;
   char *xml;

   xml = xml_presence_write(auth, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

const char *
shotgun_presence_get(Shotgun_Auth *auth, Shotgun_User_Status *st, int *priority)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);

   if (st) *st = auth->status;
   if (priority) *priority = auth->priority;
   return auth->desc;
}

Eina_Bool
shotgun_presence_subscription_set(Shotgun_Auth *auth, const char *jid, Eina_Bool subscribe)
{
   size_t len;
   char *xml;

   xml = xml_presence_write_subscription(jid, subscribe, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_presence_subscription_answer_set(Shotgun_Auth *auth, const char *jid, Eina_Bool subscribed)
{
   size_t len;
   char *xml;

   xml = xml_presence_write_subscription_answer(jid, subscribed, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}
