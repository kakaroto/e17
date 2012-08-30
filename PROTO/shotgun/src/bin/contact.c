#include "ui.h"

/* start at 255, end at shotgun/color/message */
static Eina_Bool
_contact_chat_window_animator_in(Contact *c, double pos)
{
   Evas_Object *obj;
   int x, y, w, h;
   double frame;
   int r, g, b;

   if ((!c->chat_window) || contact_chat_window_current(c))
     {
        contact_chat_window_animator_del(c);
        return EINA_FALSE;
     }

   r = c->list->alert_colors[0];
   g = c->list->alert_colors[1];
   b = c->list->alert_colors[2];
   obj = elm_toolbar_item_object_get(c->chat_tb_item);
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   evas_object_move(c->animated, x - 1, y - 1);
   frame = ecore_animator_pos_map(pos, ECORE_POS_MAP_SINUSOIDAL, 0, 0);
   evas_object_color_set(c->animated, 255 - ((255 - r) * frame), 255 - ((255 - g) * frame), 255 - ((255 - b) * frame), 255);
   return EINA_TRUE;
}

/* start at shotgun/color/message, end at 255 */
static Eina_Bool
_contact_chat_window_animator_out(Contact *c, double pos)
{
   Evas_Object *obj;
   int x, y, w, h;
   int r, g, b;
   double frame;

   if ((!c->chat_window) || contact_chat_window_current(c))
     {
        contact_chat_window_animator_del(c);
        return EINA_FALSE;
     }

   r = c->list->alert_colors[0];
   g = c->list->alert_colors[1];
   b = c->list->alert_colors[2];
   obj = elm_toolbar_item_object_get(c->chat_tb_item);
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   evas_object_move(c->animated, x - 1, y - 1);
   frame = ecore_animator_pos_map(pos, ECORE_POS_MAP_SINUSOIDAL, 0, 0);
   evas_object_color_set(c->animated, r + ((255 - r) * frame), g + ((255 - g) * frame), b + ((255 - b) * frame), 255);
   return EINA_TRUE;
}

static Eina_Bool
_contact_chat_window_animator_switch(Contact *c)
{
   int r, g, b, a;
   Ecore_Timeline_Cb cb;

   if ((!c->chat_window) || contact_chat_window_current(c))
     {
        contact_chat_window_animator_del(c);
        return EINA_FALSE;
     }
   evas_object_color_get(c->animated, &r, &g, &b, &a);
   cb = (Ecore_Timeline_Cb)((r + g + b + a >= 1000) ? _contact_chat_window_animator_in : _contact_chat_window_animator_out);
   //INF("Next %s: %d, %d, %d", ((void*)cb == (void*)_contact_chat_window_animator_in) ? "_contact_chat_window_animator_in" : "_contact_chat_window_animator_out", r, g, b);
   c->animator = ecore_animator_timeline_add(2, cb, c);

   return EINA_TRUE;
}

static Eina_Bool
_contact_chat_window_typing_cb(Contact *c)
{
   if (!c->cur)
     {
        c->sms_timer = NULL;
        return EINA_FALSE;
     }

   if (c->sms == SHOTGUN_MESSAGE_STATUS_COMPOSING)
     {
        c->sms = SHOTGUN_MESSAGE_STATUS_PAUSED;
        /* be courteous and don't leave the fucking 'composing' status set forever */
        ecore_timer_interval_set(c->sms_timer, SMS_TIMER_INTERVAL_PAUSED);
     }
   else
     {
        c->sms = SHOTGUN_MESSAGE_STATUS_INACTIVE;
        ecore_timer_del(c->sms_timer);
        c->sms_timer = NULL;
     }

   shotgun_message_send(c->list->account, c->base->jid, NULL, c->sms);
   return EINA_TRUE;
}

static void
_contact_menu_vcard_cb(Contact *c, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   contact_vcard_request(c);
}

const char *
contact_name_get(Contact *c)
{
   if (!c) return NULL;
   if (c->base->name && c->base->name[0])
     return c->base->name;
   if (c->info && c->info->full_name && c->info->full_name[0])
     return c->info->full_name;
   return c->base->jid;
}

void
contact_info_free(Contact *c, Contact_Info *ci)
{
   Contact_Info *cid;
   if (ci)
     cid = ci;
   else
     {
        if ((!c) || (!c->info)) return;
        if (c->info_thread)
          {
             c->info->dead = EINA_TRUE;
             return;
          }
        cid = c->info;
     }
   eina_stringshare_del(cid->after);
   shotgun_user_info_free((Shotgun_User_Info*)cid);
}

void
contact_free(Contact *c)
{
   Shotgun_Event_Presence *pres;

   if (!c) return;
   if (c->info_thread)
     {
        ecore_thread_cancel(c->info_thread);
        c->dead = EINA_TRUE;
        return;
     }
   if (c->list_item)
     c->list->list_item_del[c->list->mode](c->list_item);
   c->list_item = NULL;
   if (c->info) ui_eet_userinfo_update(c->list->account, c->base->jid, c->info);
   EINA_LIST_FREE(c->plist, pres)
     shotgun_event_presence_free(pres);
   shotgun_event_presence_free(c->cur);
   if (c->tooltip_timer) ecore_timer_del(c->tooltip_timer);
   if (c->sms_timer) ecore_timer_del(c->sms_timer);
   shotgun_user_free(c->base);
   contact_info_free(c, NULL);
   eina_stringshare_del(c->last_conv);
   eina_stringshare_del(c->tooltip_label);
   eina_stringshare_del(c->logdir);
   if (c->log) fclose(c->log);
   free(c);
}

void
contact_jids_menu_del(Contact *c, const char *jid)
{
   const Eina_List *l, *ll;
   Elm_Object_Item *it;
   Evas_Object *radio;
   const char *s, *rs;

   if (!c->chat_jid_menu) return;
   it = elm_menu_last_item_get(c->chat_jid_menu);
   if (!it) return;
   l = elm_menu_item_subitems_get(it);
   if (!l) return;
   s = strchr(jid, '/');
   if (s) s++;
   else s = jid;
   EINA_LIST_REVERSE_FOREACH(l, ll, it)
     {
        radio = elm_object_item_content_get(it);
        rs = elm_object_text_get(radio);
        if (strcmp(rs, s)) continue;
        if (elm_radio_state_value_get(radio) == elm_radio_value_get(radio))
          {
             elm_radio_value_set(radio, 0);
             c->force_resource = NULL;
          }
        elm_object_item_del(it);
        break;
     }
}

Contact *
do_something_with_user(Contact_List *cl, Shotgun_User *user, const char *j)
{
   Contact *c;
   char *jid, *p;

   if (user)
     {
        p = strchr(user->jid, '/');
        if (p) jid = strndupa(user->jid, p - user->jid);
        else jid = (char*)user->jid;
        c = eina_hash_find(cl->users, jid);
        if (c)
          {
             shotgun_user_free(c->base);
             c->base = user;
             return c;
          }
     }
   else
     {
        user = calloc(1, sizeof(Shotgun_User));
        user->jid = eina_stringshare_add(j);
        jid = (char*)j;
     }

   c = calloc(1, sizeof(Contact));
   c->base = user;
   c->list = cl;
   eina_hash_add(cl->users, jid, c);
   cl->users_list = eina_list_append(cl->users_list, c);
   return c;
}

Eina_Bool
contact_chat_window_current(Contact *c)
{
   if (!c->chat_window) return EINA_FALSE;

   return c->chat_panes == elm_object_content_get(c->chat_window->pager);
}

void
contact_chat_window_animator_add(Contact *c)
{
   Evas_Object *obj, *clip;
   int x, y, w, h;
   if (c->animator) return;

   obj = elm_toolbar_item_object_get(c->chat_tb_item);
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   c->animated = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_resize(c->animated, w + 2, h + 2);
   evas_object_color_set(c->animated, 0, 0, 0, 0);
   /* here we inject the newly created rect as an intermediate clip for the tb item */
   clip = evas_object_clip_get(obj);
   evas_object_clip_set(obj, c->animated);
   evas_object_clip_set(c->animated, clip);
   evas_object_show(c->animated);
   c->animator = ecore_animator_timeline_add(2, (Ecore_Timeline_Cb)_contact_chat_window_animator_in, c);
   ecore_timer_add(2, (Ecore_Task_Cb)_contact_chat_window_animator_switch, c);
}

void
contact_chat_window_animator_del(Contact *c)
{
   Evas_Object *obj, *clip;

   if (c->animator) ecore_animator_del(c->animator);
   c->animator = NULL;
   if (c->animated)
     {
        if (c->chat_tb_item)
          {
             obj = elm_toolbar_item_object_get(c->chat_tb_item);
             clip = evas_object_clip_get(c->animated);
             evas_object_clip_set(obj, clip);
          }
        evas_object_del(c->animated);
     }
   c->animated = NULL;
}

void
contact_chat_window_close(Contact *c)
{
   Chat_Window *cw = c->chat_window;
   Eina_Bool current = contact_chat_window_current(c);
   INF("Closing page for %s", contact_name_get(c));
   contact_chat_window_animator_del(c);
   if (c->last_conv != elm_entry_entry_get(c->chat_buffer))
     {
        eina_stringshare_del(c->last_conv);
        c->last_conv = eina_stringshare_ref(elm_entry_entry_get(c->chat_buffer));
     }
   elm_object_item_del(c->chat_tb_item);
   c->list->settings->chat_panes = elm_panes_content_left_size_get(c->chat_panes);
   evas_object_del(c->chat_panes);
   memset(&c->chat_window, 0, sizeof(void*) * 9);
   if (c->sms_timer)
     {
        ecore_timer_del(c->sms_timer);
        c->sms_timer = NULL;
        shotgun_message_send(c->list->account, c->base->jid, NULL, SHOTGUN_MESSAGE_STATUS_INACTIVE);
     }

   cw->contacts = eina_list_remove(cw->contacts, c);
   if (!current) return;
   c = eina_list_data_get(cw->contacts);
   if (c)
     {
        elm_win_title_set(c->chat_window->win, contact_name_get(c));
        elm_object_focus_set(c->chat_input, EINA_TRUE);
        INF("%s is current", contact_name_get(c));
     }
   else chat_window_free(cw, NULL, NULL);
}

void
contact_resource_menu_setup(Contact *c, Evas_Object *menu)
{
   Evas_Object *win, *radio, *obj;
   Elm_Object_Item *it, *sit;
   Eina_List *l;
   Shotgun_Event_Presence *pres;
   int i = 1, set = 0;

   win = elm_object_top_widget_get(menu);

   elm_menu_item_add(menu, NULL, "dialog-information", "Request VCARD", (Evas_Smart_Cb)_contact_menu_vcard_cb, c);
   elm_menu_item_add(menu, NULL, NULL, "Ignore Resource", (Evas_Smart_Cb)chat_resource_ignore_toggle, c);
   it = elm_menu_item_add(menu, NULL, "menu/arrow_right", "Send to", NULL, NULL);

   radio = elm_radio_add(win);
   elm_radio_state_value_set(radio, 0);
   elm_object_text_set(radio, "Use Priority");
   evas_object_show(radio);
   sit = elm_menu_item_add(menu, it, NULL, NULL, (Evas_Smart_Cb)contact_resource_set, c);
   elm_object_item_content_set(sit, radio);

   EINA_LIST_FOREACH(c->plist, l, pres)
     {
        const char *s;
        char *buf;
        size_t len;

        s = strchr(pres->jid, '/');
        s = s ? s + 1 : pres->jid;
        len = strlen(s);
        buf = alloca(len + 20);
        snprintf(buf, len, "%s (%d)", s ?: c->base->jid, pres->priority);
        obj = elm_radio_add(win);
        elm_radio_group_add(obj, radio);
        if (pres->jid == c->force_resource) set = i;
        elm_radio_state_value_set(obj, i++);
        elm_object_text_set(obj, buf);
        evas_object_show(obj);
        sit = elm_menu_item_add(menu, it, NULL, NULL, (Evas_Smart_Cb)contact_resource_set, c);
        elm_object_item_content_set(sit, obj);
     }
   elm_radio_value_set(radio, set);
}

void
contact_resource_set(Contact *c, Evas_Object *obj __UNUSED__, Elm_Object_Item *ev)
{
   Eina_List *l;
   Shotgun_Event_Presence *pres;
   const char *res, *s;
   Evas_Object *radio;
   int val;

   radio = (Evas_Object*)elm_object_item_content_get(ev);
   val = elm_radio_state_value_get(radio);
   if (!val)
     {
        /* selected use priority */
        c->force_resource = NULL;
        elm_radio_state_value_set(radio, 0);
        return;
     }
   res = elm_object_text_get(radio);
   if (c->force_resource)
     {
        s = strchr(c->force_resource, '/');
        if (s) s++;
        else s = c->base->jid;
        /* selected previously set resource */
        if (!memcmp(res, s, strlen(s))) return;
     }
   EINA_LIST_FOREACH(c->plist, l, pres)
     {
        s = strchr(pres->jid, '/');
        if (*s) s++;
        else s = c->base->jid;
        if (!memcmp(res, s, strlen(s))) continue;
        eina_stringshare_del(c->force_resource);
        c->force_resource = eina_stringshare_ref(pres->jid);
        contact_presence_set(c, pres);
        break;
     }
   elm_radio_state_value_set(radio, elm_menu_item_index_get(ev));
}

Shotgun_Event_Presence *
contact_presence_get(Contact *c)
{
   Eina_List *l;
   Shotgun_Event_Presence *p;

   if (!c->force_resource) return c->cur;
   if (c->cur->jid == c->force_resource) return c->cur;
   EINA_LIST_FOREACH(c->plist, l, p)
     if (p->jid == c->force_resource) return p;
   return NULL;
}

void
contact_presence_set(Contact *c, Shotgun_Event_Presence *cur)
{
   Contact_List *cl = c->list;

   if (!cur) return;
   c->status = cur->status;
   /* if status description exists and isn't the same as current status description */
   if (c->status_line && (c->description != cur->description))
     {
        elm_entry_entry_set(c->status_line, "");
        if (cur->description) elm_entry_entry_append(c->status_line, cur->description);
     }
   c->description = cur->description;
   c->priority = cur->priority;
   /* if offline view or contact has a subscription, create/update list item */
   if ((!cl->view) && (c->base->subscription == SHOTGUN_USER_SUBSCRIPTION_NONE)) return;

   c->tooltip_changed = EINA_TRUE;
   /* if no list item, create */
   if (!c->list_item)
     {
        if (c->cur->vcard)
          ui_eet_userinfo_fetch(c, EINA_FALSE);
        if (c->info)
          eina_stringshare_replace(&c->after, c->info->after);
        contact_list_user_add(cl, c);
     }
   /* otherwise, update */
   else
     {
        /* if vcard available and (not retrieved || not most recent) */
        cl->list_item_update[cl->mode](c->list_item);
     }
   /* if vcard available, fetch */
   if (c->cur->vcard && (((!c->info) && (!c->info_thread)) || (cur && cur->photo && c->info &&
       ((c->info->photo.sha1 != cur->photo) || (!c->info->photo.size)))))
     {
        INF("VCARD for %s not current; fetching.", c->base->jid);
        if (!contact_vcard_request(c))
          WRN("VCARD REQUESTED WHILE VCARD REQUEST IN PROGRESS! BUG!!!!");
     }

}

void
contact_subscription_set(Contact *c, Shotgun_Presence_Type type, Shotgun_User_Subscription sub)
{
   Shotgun_Event_Presence *pres;
   Eina_List *l;

   if (!c) return;
   if (c->cur)
     {
        c->cur->type = type;
        EINA_LIST_FOREACH(c->plist, l, pres)
          pres->type = type;
     }
   c->base->subscription = sub;
}

void
contact_presence_clear(Contact *c)
{
   Shotgun_Event_Presence *p;
   if (!c) return;

   EINA_LIST_FREE(c->plist, p)
     shotgun_event_presence_free(p);
   shotgun_event_presence_free(c->cur);
   c->cur = NULL;
   eina_stringshare_del(c->force_resource);
   c->force_resource = NULL;
   c->tooltip_changed = EINA_TRUE;
   c->description = NULL;
   c->status = 0;
   c->priority = 0;
}

/* this should never be called unless the user has explicitly added/deleted text
 * eg. sending a message should not trigger it
 */
void
contact_chat_window_typing(Contact *c, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   const char *txt;
   Shotgun_Message_Status sms = 0;

   if (!c->cur) return;
   txt = elm_entry_entry_get(c->chat_input);
   if ((!txt) || (!txt[0]))
     sms = SHOTGUN_MESSAGE_STATUS_INACTIVE;
   else
     sms = SHOTGUN_MESSAGE_STATUS_COMPOSING;

   if (c->sms == sms)
     {
        /* still composing, reset timer */
        if (c->sms_timer && (sms == SHOTGUN_MESSAGE_STATUS_COMPOSING))
          ecore_timer_reset(c->sms_timer);
        return;
     }

   if (c->sms_timer) ecore_timer_del(c->sms_timer);
   c->sms_timer = NULL;
   c->sms = sms;
   if (sms == SHOTGUN_MESSAGE_STATUS_COMPOSING)
     /* previous sms was paused or inactive */
     c->sms_timer = ecore_timer_add(SMS_TIMER_INTERVAL_COMPOSING, (Ecore_Task_Cb)_contact_chat_window_typing_cb, c);
   shotgun_message_send(c->list->account, c->base->jid, NULL, sms);
}

Eina_Bool
contact_vcard_request(Contact *c)
{
   if (c->vcard_request) return EINA_FALSE;
   c->vcard_request = shotgun_iq_vcard_get(c->list->account, c->base->jid);
   INF("New vcard request for %s", c->base->jid);
   return c->vcard_request;
}
