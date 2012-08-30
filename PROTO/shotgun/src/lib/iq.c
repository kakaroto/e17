#include "shotgun_private.h"
#include "xml.h"
/*
errors: http://www.rfc-editor.org/rfc/rfc6120.txt
The "stanza-kind" MUST be one of message, presence, or iq.

The "error-type" MUST be one of the following:

o  auth -- retry after providing credentials

o  cancel -- do not retry (the error cannot be remedied)

o  continue -- proceed (the condition was only a warning)

o  modify -- retry after changing the data sent

o  wait -- retry after waiting (the error is temporary)
*/

void
shotgun_user_setting_free(Shotgun_User_Setting *sus)
{
   if (!sus) return;
   eina_stringshare_del(sus->jid);
   free(sus);
}

void
shotgun_user_free(Shotgun_User *user)
{
   const char *g;
   if (!user) return;
   eina_stringshare_del(user->jid);
   eina_stringshare_del(user->name);
   EINA_LIST_FREE(user->groups, g)
     eina_stringshare_del(g);
   free(user);
}

void
shotgun_user_info_free(Shotgun_User_Info *info)
{
   if (!info) return;
   eina_stringshare_del(info->jid);
   eina_stringshare_del(info->full_name);
   eina_stringshare_del(info->photo.type);
   eina_stringshare_del(info->photo.sha1);
   if (info->photo.data) free(info->photo.data);
   free(info);
}

void
shotgun_iq_last_free(Shotgun_Iq_Last *last)
{
   if (!last) return;
   eina_stringshare_del(last->message);
   eina_stringshare_del(last->jid);
   free(last);
}

void
shotgun_incoming_file_free(Shotgun_Incoming_File *file)
{
   if (!file) return;
   eina_stringshare_del(file->name);
   eina_stringshare_del(file->hash);
   eina_stringshare_del(file->from);
   eina_stringshare_del(file->id);
   eina_stringshare_del(file->sid);
   eina_stringshare_del(file->path);
   eina_stringshare_del(file->data);
   free(file);
}

static void
shotgun_iq_event_free(void *data __UNUSED__, Shotgun_Event_Iq *iq)
{
   Shotgun_User *user;
   Shotgun_User_Setting *sus;
   switch (iq->type)
     {
      case SHOTGUN_IQ_EVENT_TYPE_ROSTER:
        EINA_LIST_FREE(iq->ev, user)
          shotgun_user_free(user);
        break;
      case SHOTGUN_IQ_EVENT_TYPE_INFO:
        shotgun_user_info_free(iq->ev);
        break;
      case SHOTGUN_IQ_EVENT_TYPE_OTR_QUERY:
        EINA_LIST_FREE(iq->ev, sus)
          shotgun_user_setting_free(sus);
        break;
      case SHOTGUN_IQ_EVENT_TYPE_IDLE:
        shotgun_iq_last_free(iq->ev);
        break;
      default:
        break;
     }
   free(iq);
}

void
shotgun_iq_feed(Shotgun_Auth *auth, char *data, size_t size)
{
   Shotgun_Event_Iq *iq;
   Eina_List *l;
   Shotgun_User *user;

   iq = xml_iq_read(auth, data, size);
   if (!iq) return; /* no event needed */

   switch (iq->type)
     {
      case SHOTGUN_IQ_EVENT_TYPE_ROSTER:
        if (!eina_log_domain_level_check(shotgun_log_dom, EINA_LOG_LEVEL_INFO)) break;
        EINA_LIST_FOREACH(iq->ev, l, user)
          {
             if (user->name)
               INF("User found: %s (%s)", user->name, user->jid);
             else
               INF("User found: %s", user->jid);
          }
        break;
      case SHOTGUN_IQ_EVENT_TYPE_INFO:
        {
           Shotgun_User_Info *info = iq->ev;

           INF("User: %s", info->jid);
           INF("Full Name: %s", info->full_name);
           if (info->photo.size)
             INF("Found image type %s: %zu bytes", info->photo.type, info->photo.size);
        }
      default:
        break;
     }
   ecore_event_add(SHOTGUN_EVENT_IQ, iq, (Ecore_End_Cb)shotgun_iq_event_free, NULL);
}

Eina_Bool
shotgun_iq_contact_add(Shotgun_Auth *auth, const char *user, const char *alias, Eina_List *groups)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(user, EINA_FALSE);

   xml = xml_iq_write_contact_add(user, alias, groups, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_contact_del(Shotgun_Auth *auth, const char *user)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(user, EINA_FALSE);

   xml = xml_iq_write_contact_del(user, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_roster_get(Shotgun_Auth *auth)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);

   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_ROSTER, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_server_query(Shotgun_Auth *auth)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);

   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_DISCO_INFO, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_gsettings_query(Shotgun_Auth *auth)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   if (!auth->features.google_settings) return EINA_FALSE;

   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_GSETTINGS_GET, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_vcard_get(Shotgun_Auth *auth, const char *user)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);

   xml = xml_iq_write_get_vcard(user, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_si_result(Shotgun_Event_Iq *ev)
{
   size_t len;
   char *xml;
   Shotgun_Incoming_File *file; 

   EINA_SAFETY_ON_NULL_RETURN_VAL(ev->account, EINA_FALSE);

   file = (Shotgun_Incoming_File *)ev->ev;
   xml = xml_iq_write_get_si(shotgun_jid_get(ev->account), file->from, file->id, &len);
   shotgun_write(ev->account->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}


Eina_Bool
shotgun_iq_bytestream_result(Shotgun_Event_Iq *ev)
{
   size_t len;
   char *xml;
   Shotgun_Incoming_File *file; 

   EINA_SAFETY_ON_NULL_RETURN_VAL(ev->account, EINA_FALSE);

   file = (Shotgun_Incoming_File *)ev->ev;
   xml = xml_iq_write_get_bytestream(shotgun_jid_get(ev->account), 
                                     file->from, 
                                     file->id,
                                     file->sid, 
                                     &len);
   shotgun_write(ev->account->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}


Eina_Bool
shotgun_iq_ibb_error(Shotgun_Event_Iq *ev)
{
   size_t len;
   char *xml;
   Shotgun_Incoming_File *file; 

   EINA_SAFETY_ON_NULL_RETURN_VAL(ev->account, EINA_FALSE);

   file = (Shotgun_Incoming_File *)ev->ev;
   xml = xml_iq_ibb_error(shotgun_jid_get(ev->account), file->from, file->id, &len);
   shotgun_write(ev->account->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}


Eina_Bool
shotgun_iq_vcard_send(Shotgun_Auth *auth){
   size_t len;
   char *xml;
   
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   
   xml = xml_iq_write_vcard(auth, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_activity_query(Shotgun_Auth *auth, const char *jid)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(jid, EINA_FALSE);

   xml = xml_iq_activity_query(shotgun_jid_get(auth),
                               jid,
                               "last1",
                               &len);

   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_archive_get(Shotgun_Auth *auth, const char *user, unsigned int max)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   if (!auth->features.archive_management) return EINA_FALSE;

   xml = xml_iq_write_archive_get(user, max, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_gsettings_available(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   return auth->features.google_settings;
}

Eina_Bool
shotgun_iq_otr_available(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   /* TODO: other types of OTR */
   return auth->features.google_nosave;
}

Eina_Bool
shotgun_iq_otr_set(Shotgun_Auth *auth, Eina_Bool enable)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   /* TODO: other types of OTR */
   if (!shotgun_iq_otr_available(auth)) return EINA_FALSE;
   shotgun_iq_gsettings_archiving_set(auth, !enable);
   return EINA_TRUE;
}

Eina_Bool
shotgun_iq_otr_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   /* TODO: other types of OTR */
   if (!shotgun_iq_otr_available(auth)) return EINA_FALSE;
   return shotgun_iq_gsettings_archiving_get(auth);
}

Eina_Bool
shotgun_iq_otr_query(Shotgun_Auth *auth)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   /* TODO: other types of OTR */
   if (!auth->features.google_nosave) return EINA_FALSE;

   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_OTR_QUERY, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}

void
shotgun_iq_gsettings_archiving_set(Shotgun_Auth *auth, Eina_Bool enable)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN(auth);
   enable = !!enable;
   if (auth->features.gsettings.archiving == enable) return;

   auth->features.gsettings.archiving = enable;

   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_GSETTINGS_SET, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
}

Eina_Bool
shotgun_iq_gsettings_archiving_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   return auth->features.gsettings.archiving;
}

void
shotgun_iq_gsettings_mailnotify_set(Shotgun_Auth *auth, Eina_Bool enable)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN(auth);
   enable = !!enable;
   if (auth->features.gsettings.mail_notifications == enable) return;

   auth->features.gsettings.mail_notifications = enable;

   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_GSETTINGS_SET, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
}

void
shotgun_iq_gsettings_mailnotify_ping(Shotgun_Auth *auth)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN(auth);
   if (!shotgun_iq_gsettings_available(auth)) return;

   xml = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_MAIL_SEARCH, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
}

Eina_Bool
shotgun_iq_gsettings_mailnotify_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   return auth->features.gsettings.mail_notifications;
}

Eina_Bool
shotgun_iq_contact_otr_set(Shotgun_Auth *auth, const char *jid, Eina_Bool enable)
{
   size_t len;
   char *xml;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   if (!shotgun_iq_otr_available(auth)) return EINA_FALSE;

   xml = xml_iq_write_contact_otr_set(jid, !!enable, &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
   return EINA_TRUE;
}
