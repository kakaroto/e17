#include "ui.h"

Eina_Bool
event_iq_cb(Contact_List *cl, int type __UNUSED__, Shotgun_Event_Iq *ev)
{
   Contact *c;
   switch(ev->type)
     {
      case SHOTGUN_IQ_EVENT_TYPE_ROSTER:
        {
           Shotgun_User *user;
           EINA_LIST_FREE(ev->ev, user)
             {
                c = do_something_with_user(cl, user, NULL);
                contact_subscription_set(c, 0, user->subscription);
                if (user->subscription == SHOTGUN_USER_SUBSCRIPTION_REMOVE)
                  {
                     Shotgun_Event_Presence *pres;

                     EINA_LIST_FREE(c->plist, pres)
                       shotgun_event_presence_free(pres);
                     contact_list_user_del(c, c->cur);
                     if (c->base == user) c->base = NULL;
                     shotgun_user_free(user);
                     eina_hash_del_by_data(cl->users, c);
                     cl->users_list = eina_list_remove(cl->users_list, c);
                     contact_free(c);
                     continue;
                  }
                if (c->list_item)
                  {
                     if ((!cl->view) && (!c->status) && (c->base->subscription == SHOTGUN_USER_SUBSCRIPTION_BOTH))
                       contact_list_user_del(c, NULL);
                     else
                       cl->list_item_update[cl->mode](c->list_item);
                  }
                else
                  {
                     if (cl->view || (user->subscription != SHOTGUN_USER_SUBSCRIPTION_BOTH) || user->subscription_pending)
                       contact_list_user_add(cl, c);
                  }
             }
           break;
        }
      case SHOTGUN_IQ_EVENT_TYPE_INFO:
        {
           Shotgun_User_Info *info = ev->ev;

           c = eina_hash_find(cl->users, info->jid);
           if (!c)
             {
                ERR("WTF!");
                break;
             }
           if (util_userinfo_eq((Shotgun_User_Info*)c->info, ev->ev))
             {
                INF("User info for %s unchanged, not updating cache", c->base->jid);
                break;
             }
           contact_info_free(c, NULL);
           if (c->cur && c->cur->photo)
             {
                INF("Found contact photo sha1: %s", c->cur->photo);
                info->photo.sha1 = eina_stringshare_ref(c->cur->photo);
             }
           if (info->photo.size)
             {
                c->info_img = evas_object_image_add(evas_object_evas_get(c->list->win));
                evas_object_image_memfile_set(c->info_img, info->photo.data, info->photo.size, NULL, NULL);
             }
           c->info = (Contact_Info*)info;
           ui_eet_userinfo_fetch(c, EINA_TRUE);
           ev->ev = NULL;
           break;
        }
      case SHOTGUN_IQ_EVENT_TYPE_SERVER_QUERY:
        if (shotgun_iq_gsettings_available(cl->account))
          shotgun_iq_gsettings_query(cl->account);
        break;
      case SHOTGUN_IQ_EVENT_TYPE_SETTINGS:
        cl->settings->enable_global_otr = shotgun_iq_otr_get(cl->account);
        cl->settings->enable_mail_notifications = shotgun_iq_gsettings_mailnotify_get(cl->account);
        if (cl->settings->enable_mail_notifications)
          shotgun_iq_gsettings_mailnotify_ping(cl->account);
        break;
      case SHOTGUN_IQ_EVENT_TYPE_MAILNOTIFY:
        if (cl->settings->enable_mail_notifications)
          ui_dbus_notify(cl, NULL, "Mail", "You have new mail!");
        break;
      default:
        ERR("WTF!");
     }
   return ECORE_CALLBACK_RENEW;
}

static int
_list_sort_cb(Shotgun_Event_Presence *a, Shotgun_Event_Presence *b)
{
   return a->priority - b->priority;
}

static Shotgun_Event_Presence *
_event_presence_steal(Shotgun_Event_Presence *p)
{
   Shotgun_Event_Presence *pres;

   pres = calloc(1, sizeof(Shotgun_Event_Presence));
   pres->jid = p->jid;
   p->jid = NULL;
   pres->priority = p->priority;
   pres->status = p->status;
   pres->description = p->description;
   p->description = NULL;
   pres->photo = p->photo;
   p->photo = NULL;
   pres->vcard = p->vcard;
   pres->idle = p->idle;
   pres->timestamp = p->timestamp;
   return pres;
}

Eina_Bool
event_presence_cb(Contact_List *cl, int type __UNUSED__, Shotgun_Event_Presence *ev)
{
   Contact *c;
   Shotgun_Event_Presence *pres;
   char *jid, *p;
   Eina_List *l;

   p = strchr(ev->jid, '/');
   if (p) jid = strndupa(ev->jid, p - ev->jid);
   else jid = (char*)ev->jid;
   c = eina_hash_find(cl->users, jid);
   if (!c)
     {
        /* un-added contact with subscribe request */
        if (ev->type != SHOTGUN_PRESENCE_TYPE_SUBSCRIBE)
          {
             if (ev->jid != shotgun_jid_get(cl->account))
               DBG("Received message from non-contact '%s', probably a bug", ev->jid);
             return ECORE_CALLBACK_RENEW;
          }
        c = do_something_with_user(cl, NULL, jid);
        c->cur = _event_presence_steal(ev);
        c->base->subscription = SHOTGUN_USER_SUBSCRIPTION_FROM;
        contact_presence_set(c, c->cur);
        contact_list_user_add(cl, c);
        return ECORE_CALLBACK_RENEW;
     }

   if (!ev->status)
     {
        contact_list_user_del(c, ev);
        return EINA_TRUE;
     }
   if (c->cur)
     {
        switch (ev->type)
          {
           case SHOTGUN_PRESENCE_TYPE_SUBSCRIBE:
           case SHOTGUN_PRESENCE_TYPE_UNSUBSCRIBE:
             contact_subscription_set(c, ev->type, c->base->subscription);
             if (c->list_item)
               cl->list_item_update[cl->mode](c->list_item);
             else
               contact_list_user_add(cl, c);
             return ECORE_CALLBACK_RENEW;
           default:
             break;
          }
     }
   /* if current resource is not event resource */
   if ((!c->cur) || (ev->jid != c->cur->jid))
     {
        EINA_LIST_FOREACH(c->plist, l, pres)
          {
             /* update existing resource if found */
             if (ev->jid != pres->jid) continue;

             pres->priority = ev->priority;
             pres->status = ev->status;

             eina_stringshare_del(pres->description);
             if (ev->description && ev->description[0])
               {
                  pres->description = ev->description;
                  ev->description = NULL;
               }
             else
               pres->description = NULL;

             eina_stringshare_del(pres->photo);
             if (ev->photo && ev->photo[0])
               {
                  pres->photo = ev->photo;
                  ev->photo = NULL;
               }
             else
               pres->photo = NULL;

             pres->vcard = ev->vcard;
             break;
          }
        /* if not found, copy */
        if ((!pres) || (pres->jid != ev->jid))
          {
             pres = _event_presence_steal(ev);
          }
        /* if found, update */
        else if (pres && (pres->jid == ev->jid))
          {
             pres->priority = ev->priority;
             pres->status = ev->status;
             pres->vcard = ev->vcard;
             pres->idle = ev->idle;
             pres->timestamp = ev->timestamp;
             if (pres->description != ev->description)
               {
                  eina_stringshare_del(pres->description);
                  pres->description = ev->description;
                  ev->description = NULL;
               }
             if (pres->photo != ev->photo)
               {
                  eina_stringshare_del(pres->photo);
                  pres->photo = ev->photo;
                  ev->photo = NULL;
               }
             pres->vcard = ev->vcard;
             /* must sort! */
             c->plist = eina_list_sort(c->plist, 0, (Eina_Compare_Cb)_list_sort_cb);
          }
        /* if not the current resource, update current */
        if (c->cur)
          {
             /* if current resource has no photo, use low priority photo */
             if (pres->photo && (!c->cur->photo))
               c->cur->photo = eina_stringshare_ref(pres->photo);
             c->cur->vcard |= pres->vcard;
             /* if lower priority, add to plist */
             if (ev->priority < c->cur->priority)
               {
                  if ((!l) || (l->data != pres))
                    c->plist = eina_list_sorted_insert(c->plist, (Eina_Compare_Cb)_list_sort_cb, pres);
                  /* if vcard available and (not retrieved || not most recent) */
                  if (ev->vcard && (((!c->info) && (!c->info_thread)) || (c->cur && c->info &&
                      ((c->info->photo.sha1 != c->cur->photo) ||
                       (c->cur->photo && (!c->info->photo.size))))))
                    {
                       INF("VCARD for %s not current; fetching.", c->base->jid);
                       if (!contact_vcard_request(c))
                         CRI("VCARD REQUESTED WHILE VCARD REQUEST IN PROGRESS! BUG!!!!");
                    }
                  return ECORE_CALLBACK_RENEW;
               }
             c->plist = eina_list_remove(c->plist, pres);
             c->plist = eina_list_sorted_insert(c->plist, (Eina_Compare_Cb)_list_sort_cb, c->cur);
          }
        c->cur = pres;
     }

   if (!c->force_resource)
     contact_presence_set(c, c->cur);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
event_message_cb(Contact_List *cl, int type __UNUSED__, Shotgun_Event_Message *msg)
{
   Contact *c;
   char *jid, *p;
   Chat_Window *cw;

   jid = strdupa(msg->jid);
   p = strchr(jid, '/');
   if (p) *p = 0;
   c = eina_hash_find(cl->users, jid);
   if (!c)
     {
        DBG("Received message from non-contact '%s', probably a bug", jid);
        return ECORE_CALLBACK_RENEW;
     }

   if (msg->msg)
     {

        if (!c->chat_window)
          {
             if (!cl->chat_wins) chat_window_new(cl);
             c->chat_window = cw = eina_list_data_get(cl->chat_wins);
             chat_window_chat_new(c, cw, cl->settings->enable_chat_newselect);
          }
        else if (!contact_chat_window_current(c))
          contact_chat_window_animator_add(c);
        chat_message_insert(c, contact_name_get(c), msg->msg, EINA_FALSE);
#ifdef HAVE_DBUS
        ui_dbus_signal_message(cl, c, msg);
#endif
     }
   if (c->chat_window && msg->status)
     chat_message_status(c, msg);

   return ECORE_CALLBACK_RENEW;
}
