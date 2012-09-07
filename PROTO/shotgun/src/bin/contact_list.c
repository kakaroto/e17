#include "ui.h"

static void
_contact_list_quit_cb(Contact_List *cl, Evas_Object *obj __UNUSED__, void *ev __UNUSED__)
{
   evas_object_del(cl->win);
   cl->win = NULL;
}

static void
_contact_list_free_cb(Contact_List *cl, Evas *e __UNUSED__, Evas_Object *obj, void *ev __UNUSED__)
{
   Contact *c;
   Chat_Window *cw;

   ecore_event_handler_del(cl->event_handlers.iq);
   ecore_event_handler_del(cl->event_handlers.presence);
   ecore_event_handler_del(cl->event_handlers.message);

   IF_ILLUME(cl)
     evas_object_geometry_get(cl->illume_frame, NULL, NULL, &cl->settings->list_w, &cl->settings->list_h);
   else
     evas_object_geometry_get(obj, NULL, NULL, &cl->settings->list_w, &cl->settings->list_h);

   EINA_LIST_FREE(cl->chat_wins, cw)
     chat_window_free(cw, NULL, NULL);

   if (cl->users) eina_hash_free(cl->users);
   if (cl->images) eina_hash_free(cl->images);
   if (cl->image_list) eina_list_free(cl->image_list);
   EINA_LIST_FREE(cl->users_list, c)
     contact_free(c);

   if (cl->logs_refresh) ecore_timer_del(cl->logs_refresh);

   ui_eet_presence_set(cl->account);
   ui_eet_settings_set(cl->account, cl->settings);
   ui_eet_auth_set(cl->account, cl->settings, EINA_FALSE);
   ui_eet_shutdown(cl->account);

   free(cl);
   ecore_main_loop_quit();
}

static void
_contact_list_resize_cb(Contact_List *cl, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   const Eina_List *l;
   Evas_Object *box;
   int w, h, sw, sh;
   double size;

   if (!cl->chat_wins) return;
   evas_object_geometry_get(cl->illume_frame, NULL, NULL, &w, &h);
   evas_object_geometry_get(cl->win, NULL, NULL, &sw, &sh);

   /* check against default size ratio */
   if ((double)w / (double)sw <= 300./850.) return;
   size = (double)sw * 300. / 850.;
   l = elm_box_children_get(cl->illume_box);
   box = l->next->data;
   evas_object_resize(cl->illume_frame, size, h);
   evas_object_resize(box, (double)sw - size, sh);
   elm_box_recalculate(cl->illume_box);
}

static void
_contact_list_user_del_cb(Contact *c, Evas_Object *obj __UNUSED__, void *ev  __UNUSED__)
{
   if (!c) return;
   shotgun_iq_contact_del(c->list->account, c->base->jid);
   eina_hash_del_by_data(c->list->users, c);
   c->list->users_list = eina_list_remove(c->list->users_list, c);
   contact_free(c);
}

static void
_contact_list_user_del(Contact *c)
{
   Evas_Object *menu, *ic;
   Elm_Object_Item *mi;
   char buf[128];
   int x, y;

   menu = elm_menu_add(c->list->win);
   snprintf(buf, sizeof(buf), "Really remove %s?", c->base->jid);
   mi = elm_menu_item_add(menu, NULL, NULL, buf, NULL, NULL);
   elm_object_item_disabled_set(mi, 1);
   elm_menu_item_separator_add(menu, NULL);
   mi = elm_menu_item_add(menu, NULL, "shotgun/dialog_ok", "Yes", (Evas_Smart_Cb)_contact_list_user_del_cb, c);
   ic = elm_object_item_content_get(mi);
   evas_object_color_set(ic, 0, 255, 0, 255);
   mi = elm_menu_item_add(menu, NULL, "close", "No", (Evas_Smart_Cb)_contact_list_user_del_cb, NULL);
   ic = elm_object_item_content_get(mi);
   evas_object_color_set(ic, 255, 0, 0, 255);
   evas_pointer_canvas_xy_get(evas_object_evas_get(c->list->win), &x, &y);
   elm_menu_move(menu, x, y);
   evas_object_show(menu);
}

static void
_contact_list_reorder_cb(Contact_List *cl, Evas_Object *obj __UNUSED__, Elm_Object_Item *it)
{
   Contact *c, *before, *after;
   Eina_List *l;
   Elm_Object_Item *i;
   Eina_Bool ua = EINA_FALSE, ub = EINA_FALSE;
   const char *jid;

   c = elm_object_item_data_get(it);
   i = elm_genlist_item_prev_get(it);
   before = i ? elm_object_item_data_get(i) : NULL;
   jid = before ? before->after : NULL;
   INF("%s moved after %s", contact_name_get(c), contact_name_get(before));
   EINA_LIST_FOREACH(cl->users_list, l, after)
     {
        if ((!ub) && (after->after == jid))
          {
             INF("Updating previous item %s", contact_name_get(after));
             eina_stringshare_replace(&after->after, c->base->jid);
             ub++;
          }
        if ((!ua) && (after->after == c->base->jid))
          {
             INF("Updating previously previous item %s", contact_name_get(after));
             eina_stringshare_replace(&after->after, c->after);
             ua++;
          }
        if (ua && ub) break;
     }
   eina_stringshare_replace(&c->after, jid);
}

static void
_contact_list_click_cb(Contact_List *cl, Evas_Object *obj __UNUSED__, void *ev)
{
   Contact *c;

   c = elm_object_item_data_get(ev);
   if (c->chat_window)
     {
        elm_toolbar_item_selected_set(c->chat_tb_item, EINA_TRUE);
        elm_win_activate(c->chat_window->win);
        return;
     }
   if (!cl->chat_wins) chat_window_new(cl);
   chat_window_chat_new(c, cl->chat_wins->data, EINA_TRUE);
   elm_win_activate(c->chat_window->win);
}

static void
_contact_list_remove_cb(Elm_Object_Item *it, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Contact *c = elm_object_item_data_get(it);

   if (shotgun_connection_state_get(c->list->account) != SHOTGUN_CONNECTION_STATE_CONNECTED) return;
   _contact_list_user_del(c);
}

static void
_contact_list_subscribe_cb(Elm_Object_Item *it, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Contact *c;
   Eina_Bool s;

   c = elm_object_item_data_get(it);
   if (shotgun_connection_state_get(c->list->account) != SHOTGUN_CONNECTION_STATE_CONNECTED) return;
   s = (c->base->subscription == SHOTGUN_USER_SUBSCRIPTION_FROM) ? EINA_TRUE : EINA_FALSE;
   shotgun_presence_subscription_set(c->list->account, c->base->jid, s);
}

static void
_contact_list_add_pager_cb_next(Contact_List *cl, Evas_Object *obj __UNUSED__, void *ev __UNUSED__)
{
   const char *text;
   Evas_Object *o;

   cl->pager_state++;
   if (cl->pager_state)
     {
        elm_naviframe_item_promote(elm_naviframe_bottom_item_get(cl->pager));
        elm_object_focus_set(cl->pager_entries->next->data, 1);
        elm_entry_cursor_end_set(cl->pager_entries->next->data);
        return;
     }
   o = cl->pager_entries->data;
   text = elm_entry_entry_get(o);
   if ((!text) || (!strchr(text, '@'))) /* FIXME */
     {
        ERR("Invalid contact jid");
        cl->pager_state--;
        return;
     }
   o = cl->pager_entries->next->data;
   shotgun_iq_contact_add(cl->account, text, elm_entry_entry_get(o), NULL); /* add contact */
   shotgun_presence_subscription_set(cl->account, text, EINA_TRUE); /* subscribe */
   evas_object_del(cl->pager);
   cl->pager_entries = eina_list_free(cl->pager_entries);
   cl->pager = NULL;
}

static void
_contact_list_add_pager_cb_prev(Contact_List *cl, Evas_Object *obj __UNUSED__, void *ev __UNUSED__)
{
   cl->pager_state--;
   if (cl->pager_state)
     {
        evas_object_del(cl->pager);
        cl->pager_entries = eina_list_free(cl->pager_entries);
        cl->pager = NULL;
        cl->pager_state = 0;
        return;
     }

   elm_naviframe_item_promote(elm_naviframe_bottom_item_get(cl->pager));
   elm_object_focus_set(cl->pager_entries->data, 1);
   elm_entry_cursor_end_set(cl->pager_entries->data);
}

static void
_contact_list_add_cb(Contact_List *cl, Evas_Object *obj __UNUSED__, Elm_Object_Item *ev)
{
   Evas_Object *p, *b, *b2, *o, *i;

   elm_toolbar_item_selected_set(ev, EINA_FALSE);
   if (cl->pager) return;
   if (shotgun_connection_state_get(cl->account) != SHOTGUN_CONNECTION_STATE_CONNECTED) return;
   cl->pager = p = elm_naviframe_add(cl->win);
   WEIGHT(p, EVAS_HINT_EXPAND, 0);
   FILL(p);
   elm_box_pack_after(cl->box, p, cl->list);
   elm_object_style_set(p, "slide");

   {
      i = elm_icon_add(cl->win);
      elm_icon_order_lookup_set(i, ELM_ICON_LOOKUP_THEME);
      elm_icon_standard_set(i, "arrow_left");
      evas_object_show(i);
      b = elm_button_add(cl->win);
      elm_object_content_set(b, i);
      elm_object_tooltip_text_set(b, "Back");
      elm_object_tooltip_window_mode_set(b, EINA_TRUE);
      evas_object_smart_callback_add(b, "clicked", (Evas_Smart_Cb)_contact_list_add_pager_cb_prev, cl);
      evas_object_show(b);

      i = elm_icon_add(cl->win);
      elm_icon_order_lookup_set(i, ELM_ICON_LOOKUP_THEME);
      elm_icon_standard_set(i, "shotgun/dialog_ok");
      evas_object_color_set(i, 0, 255, 0, 255);
      evas_object_show(i);
      b2 = elm_button_add(cl->win);
      elm_object_content_set(b2, i);
      elm_object_tooltip_text_set(b2, "Add contact");
      elm_object_tooltip_window_mode_set(b2, EINA_TRUE);
      evas_object_smart_callback_add(b2, "clicked", (Evas_Smart_Cb)_contact_list_add_pager_cb_next, cl);
      evas_object_show(b2);

      o = elm_entry_add(cl->win);
      EXPAND(o);
      ALIGN(o, EVAS_HINT_FILL, 0.5);
      cl->pager_entries = eina_list_append(cl->pager_entries, o);
      elm_entry_entry_append(o, "Example Name");
      elm_entry_line_wrap_set(o, ELM_WRAP_MIXED);
      elm_entry_single_line_set(o, 1);
      elm_entry_editable_set(o, 1);
      elm_entry_scrollable_set(o, 1);
      elm_object_tooltip_text_set(o, "Contact's display name");
      elm_object_tooltip_window_mode_set(o, EINA_TRUE);
      elm_scroller_policy_set(o, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
      evas_object_smart_callback_add(o, "activated", (Evas_Smart_Cb)_contact_list_add_pager_cb_next, cl);
      evas_object_show(o);
      elm_entry_cursor_begin_set(o);
      elm_entry_select_all(o);

      elm_naviframe_item_push(p, "Contact's Name (Alias):", b, b2, o, NULL);
   }

   {
      i = elm_icon_add(cl->win);
      elm_icon_order_lookup_set(i, ELM_ICON_LOOKUP_THEME);
      elm_icon_standard_set(i, "close");
      evas_object_color_set(i, 255, 0, 0, 255);
      evas_object_show(i);
      b = elm_button_add(cl->win);
      elm_object_content_set(b, i);
      elm_object_tooltip_text_set(b, "Cancel");
      elm_object_tooltip_window_mode_set(b, EINA_TRUE);
      evas_object_smart_callback_add(b, "clicked", (Evas_Smart_Cb)_contact_list_add_pager_cb_prev, cl);
      evas_object_show(b);

      i = elm_icon_add(cl->win);
      elm_icon_order_lookup_set(i, ELM_ICON_LOOKUP_THEME);
      elm_icon_standard_set(i, "arrow_right");
      evas_object_show(i);
      b2 = elm_button_add(cl->win);
      elm_object_content_set(b2, i);
      elm_object_tooltip_text_set(b2, "Next");
      elm_object_tooltip_window_mode_set(b2, EINA_TRUE);
      evas_object_smart_callback_add(b2, "clicked", (Evas_Smart_Cb)_contact_list_add_pager_cb_next, cl);
      evas_object_show(b2);

      o = elm_entry_add(cl->win);
      EXPAND(o);
      ALIGN(o, EVAS_HINT_FILL, 0.5);
      cl->pager_entries = eina_list_prepend(cl->pager_entries, o);
      elm_entry_entry_append(o, "contact@example.com");
      elm_entry_line_wrap_set(o, ELM_WRAP_MIXED);
      elm_entry_single_line_set(o, 1);
      elm_entry_editable_set(o, 1);
      elm_entry_scrollable_set(o, 1);
      elm_object_tooltip_text_set(o, "Contact's full address in the format \"ABC@XYZ.TLD\"");
      elm_object_tooltip_window_mode_set(o, EINA_TRUE);
      elm_scroller_policy_set(o, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
      evas_object_smart_callback_add(o, "activated", (Evas_Smart_Cb)_contact_list_add_pager_cb_next, cl);
      evas_object_show(o);
      elm_entry_cursor_begin_set(o);
      elm_entry_select_all(o);
      elm_object_focus_set(o, 1);

      elm_naviframe_item_push(p, "Contact's Address:", b, b2, o, NULL);
   }

   evas_object_show(p);
}

static void
_contact_list_del_cb(Contact_List *cl, Evas_Object *obj __UNUSED__, Elm_Object_Item *ev)
{
   Contact *c;

   c = elm_object_item_data_get(cl->list_selected_item_get[cl->mode](cl->list));
   elm_toolbar_item_selected_set(ev, EINA_FALSE);
   if ((!c) || (shotgun_connection_state_get(c->list->account) != SHOTGUN_CONNECTION_STATE_CONNECTED)) return;
   _contact_list_user_del(c);
}

static void
_contact_list_rightclick_cb(Contact_List *cl, Evas *e __UNUSED__, Evas_Object *obj, Evas_Event_Mouse_Down *ev)
{
   Evas_Object *menu;
   Elm_Object_Item *mi;
   void *it;
   Contact *c;
   const char *name;
   char buf[128];

   if (ev->button != 3) return;
   if (shotgun_connection_state_get(cl->account) != SHOTGUN_CONNECTION_STATE_CONNECTED) return;

/* FIXME: HACKS!! */
   if (cl->mode)
     it = elm_gengrid_selected_item_get(obj);
   else
     it = cl->list_at_xy_item_get[cl->mode](obj, ev->output.x, ev->output.y, NULL);
   if (!it) return;
   menu = elm_menu_add(elm_object_top_widget_get(obj));
   if (cl->mode)
     name = elm_object_item_part_text_get(it, NULL);
   else
     name = elm_object_item_part_text_get(it, "elm.text");

   c = elm_object_item_data_get(it);
   contact_resource_menu_setup(c, menu);
   elm_menu_item_separator_add(menu, NULL);

   switch (c->base->subscription)
     {
      case SHOTGUN_USER_SUBSCRIPTION_TO:
      case SHOTGUN_USER_SUBSCRIPTION_BOTH:
        snprintf(buf, sizeof(buf), "Unsubscribe from %s", name);
        elm_menu_item_add(menu, NULL, "shotgun/arrow_pending_right", buf, (Evas_Smart_Cb)_contact_list_subscribe_cb, it);
        break;
      case SHOTGUN_USER_SUBSCRIPTION_FROM:
        if (c->base->subscription_pending)
          {
             mi = elm_menu_item_add(menu, NULL, "shotgun/arrow_pending_left", "Subscription request sent", NULL, NULL);
             elm_object_item_disabled_set(mi, EINA_TRUE);
          }
        else
          {
             snprintf(buf, sizeof(buf), "Subscribe to %s", name);
             elm_menu_item_add(menu, NULL, "shotgun/arrow_pending_left", buf, (Evas_Smart_Cb)_contact_list_subscribe_cb, it);
          }
        break;
      default:
        break;
     }
   snprintf(buf, sizeof(buf), "Remove %s", name);
   elm_menu_item_add(menu, NULL, "menu/delete", buf, (Evas_Smart_Cb)_contact_list_remove_cb, it);
   elm_menu_move(menu, ev->output.x, ev->output.y);
   evas_object_show(menu);
}

static char *
_it_text_get_grid(Contact *c, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return strdup(contact_name_get(c));
}

static char *
_it_text_get_list(Contact *c, Evas_Object *obj __UNUSED__, const char *part)
{
   int ret;
   if (!strcmp(part, "elm.text"))
     return strdup(contact_name_get(c));
   if (!strcmp(part, "elm.text.sub"))
     {
        char *buf;
        const char *status;

        switch(c->status)
          {
           case SHOTGUN_USER_STATUS_NORMAL:
              status = "Normal";
              break;
           case SHOTGUN_USER_STATUS_AWAY:
              status = "Away";
              break;
           case SHOTGUN_USER_STATUS_CHAT:
              status = "Chat";
              break;
           case SHOTGUN_USER_STATUS_DND:
              status = "Busy";
              break;
           case SHOTGUN_USER_STATUS_XA:
              status = "Very Away";
              break;
           case SHOTGUN_USER_STATUS_NONE:
              status = "Offline?";
              break;
           default:
              status = "What the fuck aren't we handling?";
          }

        if (!c->description)
          return strdup(status);
        ret = asprintf(&buf, "%s: %s", status, c->description);
        if (ret > 0) return buf;
     }

   return NULL;
}

static Evas_Object *
_it_content_get(Contact *c, Evas_Object *obj, const char *part)
{
   Evas_Object *ic;
   const char *str = NULL;
   int alpha = 255;

   ic = elm_icon_add(obj);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   if (!strcmp(part, "elm.swallow.end"))
     {
        if (c->info && c->info->photo.size)
          {
             /* FIXME: eet_file_get() */
             char buf[1024];

             snprintf(buf, sizeof(buf), "%s/%s/img", shotgun_jid_get(c->list->account), c->base->jid);
             if (!elm_image_file_set(ic, eet_file_get(shotgun_data_get(c->list->account)), buf))
               elm_icon_standard_set(ic, "shotgun/userunknown");
          }
        else
          elm_icon_standard_set(ic, "shotgun/userunknown");
        evas_object_show(ic);
        return ic;
     }
   elm_icon_order_lookup_set(ic, ELM_ICON_LOOKUP_THEME);
   if (c->cur)
     {
        switch (c->cur->type)
          {
           case SHOTGUN_PRESENCE_TYPE_SUBSCRIBE:
             str = "shotgun/arrows_pending_right";
             break;
           case SHOTGUN_PRESENCE_TYPE_UNSUBSCRIBE:
             str = "shotgun/arrows_rejected";
           default:
             break;
          }
        if (c->cur->idle)
          {
             unsigned int x;
             for (x = 10; (x <= c->cur->idle) && (alpha > 64); x += 10)
               alpha -= 12;
          }
     }
   else if (c->base->subscription_pending)
     str = "shotgun/arrows_pending_left";
   else if (!c->base->subscription)
     str = "shotgun/x";
   if (!str)
     {
        switch (c->base->subscription)
          {
           case SHOTGUN_USER_SUBSCRIPTION_NONE:
           case SHOTGUN_USER_SUBSCRIPTION_REMOVE:
             str = "close";
             break;
           case SHOTGUN_USER_SUBSCRIPTION_TO:
             str = "shotgun/arrows_pending_left";
             break;
           case SHOTGUN_USER_SUBSCRIPTION_FROM:
             str = "shotgun/arrows_pending_right";
             break;
           case SHOTGUN_USER_SUBSCRIPTION_BOTH:
             str = "shotgun/arrows_both";
           default:
             break;
          }
     }
   elm_icon_standard_set(ic, str);
   switch (c->status)
     {
        case SHOTGUN_USER_STATUS_NORMAL:
          evas_object_color_set(ic, 0, MIN(200, alpha), 0, alpha);
          break;
        case SHOTGUN_USER_STATUS_AWAY:
          evas_object_color_set(ic, MIN(255, alpha), MIN(204, alpha), 51, alpha);
          break;
        case SHOTGUN_USER_STATUS_CHAT:
          evas_object_color_set(ic, 0, MIN(255, alpha), 0, alpha);
          break;
        case SHOTGUN_USER_STATUS_DND:
          evas_object_color_set(ic, 0, 0, MIN(255, alpha), alpha);
          break;
        case SHOTGUN_USER_STATUS_XA:
          evas_object_color_set(ic, MIN(255, alpha), 0, 0, alpha);
        default:
          break;
     }

   return ic;
}

static Eina_Bool
_it_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

static void
_it_del(Contact *c, Evas_Object *obj __UNUSED__)
{
   c->list_item = NULL;
}

static void
_contact_list_list_add(Contact_List *cl)
{
   Evas_Object *list;
   const Eina_List *l;

   cl->list = list = elm_genlist_add(cl->win);
   cl->mode = EINA_FALSE;
   elm_genlist_reorder_mode_set(list, EINA_TRUE);
   elm_scroller_bounce_set(list, EINA_FALSE, EINA_FALSE);
   elm_scroller_policy_set(list, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
   EXPAND(list);
   FILL(list);
   elm_genlist_mode_set(list, ELM_LIST_COMPRESS);
   l = elm_box_children_get(cl->box);
   elm_box_pack_after(cl->box, list, l->data);
   evas_object_show(list);
   evas_object_smart_callback_add(list, "activated",
                                  (Evas_Smart_Cb)_contact_list_click_cb, cl);
   evas_object_smart_callback_add(list, "moved",
                                  (Evas_Smart_Cb)_contact_list_reorder_cb, cl);
   evas_object_event_callback_add(list, EVAS_CALLBACK_MOUSE_DOWN,
                                  (Evas_Object_Event_Cb)_contact_list_rightclick_cb, cl);
}


/* FIXME: gengrid is too sucky to use
static void
_contact_list_grid_add(Contact_List *cl)
{
   Evas_Object *grid;
   const Eina_List *l;

   cl->list = grid = elm_gengrid_add(cl->win);
   cl->mode = EINA_TRUE;
   elm_gengrid_always_select_mode_set(grid, EINA_FALSE);
   elm_gengrid_item_size_set(grid, 75, 100);
   elm_gengrid_bounce_set(grid, EINA_FALSE, EINA_FALSE);
   EXPAND(grid);
   FILL(grid);
   l = elm_box_children_get(cl->box);
   elm_box_pack_after(cl->box, grid, l->data);
   evas_object_show(grid);
   evas_object_smart_callback_add(grid, "activated",
                                  (Evas_Smart_Cb)_contact_list_click_cb, cl);
   evas_object_event_callback_add(grid, EVAS_CALLBACK_MOUSE_DOWN,
                                  (Evas_Object_Event_Cb)_contact_list_rightclick_cb, cl);
}
*/

static void
_contact_list_status_close(Contact_List *cl, Evas_Object *obj, void *ev __UNUSED__)
{
   char *s;

   s = elm_entry_markup_to_utf8(elm_entry_entry_get(cl->status_entry));
   shotgun_presence_desc_set(cl->account, s);
   free(s);
   shotgun_presence_send(cl->account);
#ifdef HAVE_DBUS
   ui_dbus_signal_status_self(cl);
#endif
   cl->status_entry = NULL;
   evas_object_del(obj);
}

static void
_contact_list_status_change(Contact_List *cl, Evas_Object *radio,  void*ev __UNUSED__)
{
   Shotgun_User_Status val;

   val = elm_radio_value_get(radio);
   if ((Shotgun_User_Status)shotgun_presence_status_get(cl->account) == val) return;
   elm_radio_value_set(radio, val);
   shotgun_presence_status_set(cl->account, val);
   elm_entry_select_all(cl->status_entry);
   elm_object_focus_set(cl->status_entry, EINA_TRUE);
}

static void
_contact_list_status_priority(Contact_List *cl, Evas_Object *obj, void *ev __UNUSED__)
{
   shotgun_presence_priority_set(cl->account, elm_spinner_value_get(obj));
}

static void
_contact_list_status_activate(Contact_List *cl, Evas_Object *obj __UNUSED__, void *ev __UNUSED__)
{
   elm_ctxpopup_dismiss(cl->status_popup);
}

static void
_contact_list_status_click(Contact_List *cl, Evas_Object *o __UNUSED__, Elm_Object_Item *ev)
{
   Evas_Object *cx, *b, *box, *scr, *obj, *radio;
   int w, h, x, y;

   elm_toolbar_item_selected_set(ev, EINA_FALSE);

   evas_object_geometry_get(cl->win, NULL, NULL, &w, &h);
   b = elm_box_add(cl->win);
   evas_object_size_hint_min_set(b, 0.7 * w, 0.5 * h);
   evas_object_size_hint_max_set(b, 0.8 * w, 0.8 * h);

   scr = elm_scroller_add(b);
   elm_scroller_bounce_set(scr, EINA_FALSE, EINA_FALSE);
   elm_scroller_policy_set(scr, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
   FILL(scr);
   EXPAND(scr);
   elm_box_pack_end(b, scr);
   evas_object_show(scr);

   box = elm_box_add(scr);
   FILL(box);
   EXPAND(box);

   radio = elm_radio_add(scr);
   elm_radio_state_value_set(radio, SHOTGUN_USER_STATUS_NORMAL);
   elm_object_text_set(radio, "Available");
   evas_object_smart_callback_add(radio, "changed", (Evas_Smart_Cb)_contact_list_status_change, cl);
   ALIGN(radio, 0, EVAS_HINT_FILL);
   elm_box_pack_end(box, radio);
   evas_object_show(radio);

#define STATUS_RADIO(STATUS, LABEL) \
   obj = elm_radio_add(scr); \
   elm_radio_state_value_set(obj, SHOTGUN_USER_STATUS_##STATUS); \
   elm_radio_group_add(obj, radio); \
   elm_object_text_set(obj, LABEL); \
   evas_object_smart_callback_add(obj, "changed", (Evas_Smart_Cb)_contact_list_status_change, cl); \
   ALIGN(obj, 0, EVAS_HINT_FILL); \
   elm_box_pack_end(box, obj); \
   evas_object_show(obj)

   STATUS_RADIO(CHAT, "Chatty");
   STATUS_RADIO(AWAY, "Away");
   STATUS_RADIO(XA, "Really Away");
   STATUS_RADIO(DND, "DND");
   elm_radio_value_set(radio, shotgun_presence_status_get(cl->account));

   cl->status_entry = obj = elm_entry_add(scr);
   elm_entry_line_wrap_set(obj, ELM_WRAP_MIXED);
   elm_entry_single_line_set(obj, 1);
   elm_scroller_policy_set(obj, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
   elm_entry_select_all(obj);
   EXPAND(obj);
   FILL(obj);
   elm_box_pack_end(box, obj);
   evas_object_smart_callback_add(obj, "activated", (Evas_Smart_Cb)_contact_list_status_activate, cl);
   elm_entry_entry_set(obj, shotgun_presence_desc_get(cl->account));
   elm_scroller_policy_set(obj, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
   evas_object_show(obj);

   obj = elm_spinner_add(scr);
   elm_spinner_label_format_set(obj, "Priority: %1.0f");
   elm_spinner_wrap_set(obj, EINA_TRUE);
   elm_spinner_step_set(obj, 1);
   elm_spinner_min_max_set(obj, 0, 9999);
   elm_spinner_value_set(obj, shotgun_presence_priority_get(cl->account));
   FILL(obj);
   elm_box_pack_end(box, obj);
   evas_object_smart_callback_add(obj, "changed", (Evas_Smart_Cb)_contact_list_status_priority, cl);
   evas_object_show(obj);

   evas_object_show(box);
   elm_object_content_set(scr, box);

   cl->status_popup = cx = elm_ctxpopup_add(cl->win);
   evas_object_smart_callback_add(cx, "dismissed", (Evas_Smart_Cb)_contact_list_status_close, cl);
   elm_object_content_set(cx, b);
   evas_pointer_canvas_xy_get(evas_object_evas_get(obj), &x, &y);
   evas_object_move(cx, x, y);
   evas_object_show(cx);

   elm_object_focus_set(cl->status_entry, EINA_TRUE);
}

static Eina_Bool
_contact_list_item_tooltip_update_cb(Contact *c)
{
   c->tooltip_changed = EINA_TRUE;
   c->tooltip_timer = NULL;
   return ECORE_CALLBACK_CANCEL;
}

static Evas_Object *
_contact_list_item_tooltip_cb(Contact *c, Evas_Object *obj __UNUSED__, Evas_Object *tt, void *it __UNUSED__)
{
   Evas_Object *label;
   const char *text;
   char *desc;
   Eina_Strbuf *buf;
   Eina_List *l;
   Shotgun_Event_Presence *p;
   unsigned int timer = 0, t2;

   if (!c->tooltip_changed) goto out;
   if (c->status && c->cur)
     {
        buf = eina_strbuf_new();
        desc = elm_entry_utf8_to_markup(c->description);
        if (c->cur->idle)
          {
             eina_strbuf_append_printf(buf,
               "<b><title>%s</title></b><ps>"
               "<b><subtitle><u>%s (%i)%s</u></subtitle></b><ps>"
               "%s<ps>"
               "<i>Idle: %u minutes</i>",
               c->base->jid,
               c->cur->jid + strlen(c->base->jid) + 1, c->priority, desc ? ":" : "",
               desc ?: "",
               (c->cur->idle + (unsigned int)(ecore_time_unix_get() - c->cur->timestamp)) / 60 + (c->cur->idle % 60 > 30));
             timer = c->cur->idle % 60;
          }
        else
          eina_strbuf_append_printf(buf,
            "<b><title>%s</title></b><ps>"
            "<b><subtitle><u>%s (%i)%s</u></subtitle></b><ps>"
            "%s%s",
            c->base->jid,
            c->cur->jid + strlen(c->base->jid) + 1, c->priority, desc ? ":" : "",
            desc ?: "", desc ? "<ps>" : "");
        free(desc);
        EINA_LIST_FOREACH(c->plist, l, p)
          {
             desc = elm_entry_utf8_to_markup(p->description);
             if (p->idle)
               {
                  eina_strbuf_append_printf(buf,
                    "<ps><b>%s (%i)%s</b><ps>"
                    "%s<ps>"
                    "<i>Idle: %u minutes</i>",
                    p->jid + strlen(c->base->jid) + 1, p->priority, p->description ? ":" : "",
                    desc ?: "",
                    (p->idle + (unsigned int)(ecore_time_unix_get() - p->timestamp)) / 60 + (p->idle % 60 > 30));
                  t2 = p->idle % 60;
                  if (t2 > timer) timer = t2;
               }
             else
               eina_strbuf_append_printf(buf,
                 "<ps><b>%s (%i)%s</b><ps>"
                 "%s%s",
                 p->jid + strlen(c->base->jid) + 1, p->priority, p->description ? ":" : "",
                 desc ?: "", p->description ? "<ps>" : "");
             if (p->description) free(desc);
          }
        text = eina_stringshare_add(eina_strbuf_string_get(buf));
        eina_strbuf_free(buf);
     }
   else
     text = eina_stringshare_printf("<b><title>%s</title></b><ps>", c->base->jid);

   if (timer)
     {
        if (c->tooltip_timer) ecore_timer_interval_set(c->tooltip_timer, timer);
        else c->tooltip_timer = ecore_timer_add((double)timer, (Ecore_Task_Cb)_contact_list_item_tooltip_update_cb, c);
     }
   else if (c->tooltip_timer)
     {
        ecore_timer_del(c->tooltip_timer);
        c->tooltip_timer = NULL;
     }
   eina_stringshare_del(c->tooltip_label);
   c->tooltip_label = text;
out:
   label = elm_label_add(tt);
   elm_label_line_wrap_set(label, ELM_WRAP_NONE);
   elm_object_text_set(label, c->tooltip_label);
   EXPAND(label);
   FILL(label);
   return label;
}

static void
_contact_list_window_key(Contact_List *cl, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, Evas_Event_Key_Down *ev)
{
   //DBG("%s", ev->keyname);
   if (!strcmp(ev->keyname, "Escape"))
     {
        if (!cl->pager)
          {
             if (elm_flip_front_visible_get(cl->flip)) return;
             settings_toggle((UI_WIN*)cl, NULL, NULL);
             return;
          }
        _contact_list_add_pager_cb_prev(cl, NULL, NULL);
     }
   else if (!strcmp(ev->keyname, "q"))
     evas_object_del(cl->win);
}

static int
_contact_list_sorted_insert(Elm_Object_Item *it1, Elm_Object_Item *it2)
{
   const char *c1, *c2;
   Contact *a = elm_object_item_data_get(it1), *b = elm_object_item_data_get(it2);

   c1 = contact_name_get(a);
   c2 = contact_name_get(b);
   return tolower(c1[0]) - tolower(c2[0]);
}

void
contact_list_show_toggle(Contact_List *cl, Evas_Object *obj __UNUSED__, Elm_Object_Item *ev __UNUSED__)
{
   Eina_List *l;
   Contact *c;

   if (shotgun_connection_state_get(cl->account) != SHOTGUN_CONNECTION_STATE_CONNECTED) return;
   cl->view++;
   EINA_LIST_FOREACH(cl->users_list, l, c)
     {
        if (cl->view && (!c->list_item)) contact_list_user_add(cl, c);
        else if ((!cl->view) && (((!c->cur) || (!c->cur->status)) || (!c->base->subscription)))
          contact_list_user_del(c, c->cur ?: NULL);
     }
}

void
contact_list_mode_toggle(Contact_List *cl, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Eina_List *l;
   Contact *c;
   evas_object_del(cl->list);
/*
   if (cl->mode)
     _contact_list_grid_add(cl);
   else
*/
     _contact_list_list_add(cl);
   EINA_LIST_FOREACH(cl->users_list, l, c)
     {
        c->list_item = NULL;
        if (cl->view || ((c->base->subscription > SHOTGUN_USER_SUBSCRIPTION_NONE) && c->status))
          contact_list_user_add(cl, c);
     }
}

void
contact_list_user_add(Contact_List *cl, Contact *c)
{
   static Elm_Genlist_Item_Class glit = {
        .item_style = NULL,
        .func = {
             .text_get = (Elm_Genlist_Item_Text_Get_Cb)_it_text_get_list,
             .content_get = (Elm_Genlist_Item_Content_Get_Cb)_it_content_get,
             .state_get = (Elm_Genlist_Item_State_Get_Cb)_it_state_get,
             .del = (Elm_Genlist_Item_Del_Cb)_it_del
        },
        .version = ELM_GENLIST_ITEM_CLASS_VERSION
   };
   static Elm_Gengrid_Item_Class ggit = {
        .item_style = "default",
        .func = {
             .text_get = (Elm_Gengrid_Item_Text_Get_Cb)_it_text_get_grid,
             .content_get = (Elm_Gengrid_Item_Content_Get_Cb)_it_content_get,
             .state_get = (Elm_Gengrid_Item_State_Get_Cb)_it_state_get,
             .del = (Elm_Gengrid_Item_Del_Cb)_it_del
        }
   };
   glit.item_style = cl->settings->disable_list_status ? "default" : "double_label";
   if ((!c) || c->list_item) return;
   if (!cl->view)
     {
        if (!c->cur) return;
        if ((!c->status) && (c->base->subscription == SHOTGUN_USER_SUBSCRIPTION_BOTH))
          return;
     }
   c->tooltip_changed = EINA_TRUE;
   if (cl->mode)
     c->list_item = elm_gengrid_item_append(cl->list, &ggit, c, NULL, NULL);
   else
     {
        Contact *after;
        Elm_Object_Item *it;
        if (cl->settings->enable_list_sort_alpha)
          {
             c->list_item = elm_genlist_item_sorted_insert(cl->list, &glit, c, NULL,
                                                    ELM_GENLIST_ITEM_NONE, (Eina_Compare_Cb)_contact_list_sorted_insert, NULL, NULL);
          }
        else if (c->after)
          {
             after = eina_hash_find(cl->users, c->after);
             /* find the next previous contact which has an item */
             while (after && after->after && (!after->list_item) && (after != c))
               {
                  //DBG("Found c->after %s", after->base->jid);
                  after = eina_hash_find(cl->users, after->after);
               }
             if (after && after->list_item)
               {
                  INF("Inserting after %s", contact_name_get(after));
                  c->list_item = elm_genlist_item_insert_after(cl->list, &glit, c, NULL,
                                                               after->list_item,
                                                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
               }
             else
               c->list_item = elm_genlist_item_prepend(cl->list, &glit, c, NULL,
                                                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
          }
        if (!c->list_item)
          {
             c->list_item = elm_genlist_item_append(cl->list, &glit, c, NULL,
                                                    ELM_GENLIST_ITEM_NONE, NULL, NULL);
             it = elm_genlist_item_prev_get(c->list_item);
             if (it)
               {
                  after = elm_object_item_data_get(it);
                  eina_stringshare_replace(&c->after, after->base->jid);
               }
          }
     }
   cl->list_item_tooltip_add[cl->mode](c->list_item,
     (Elm_Tooltip_Item_Content_Cb)_contact_list_item_tooltip_cb, c, NULL);
   cl->list_item_tooltip_resize[cl->mode](c->list_item, EINA_TRUE);
}

void
contact_list_user_del(Contact *c, Shotgun_Event_Presence *ev)
{
   Eina_List *l, *ll;
   Shotgun_Event_Presence *pres;
   if ((!c->plist) || (!ev))
     {
        if (c->list_item)
          {
             INF("Removing user %s", c->base->jid);
             c->list->list_item_del[c->list->mode](c->list_item);
          }
        c->list_item = NULL;
        contact_presence_clear(c);
        return;
     }
   if (ev->jid == c->cur->jid)
     {
        c->plist = eina_list_remove(c->plist, c->cur);
        shotgun_event_presence_free(c->cur);
        c->cur = NULL;
        EINA_LIST_FOREACH(c->plist, l, pres)
          {
             if (!c->cur)
               {
                  c->cur = pres;
                  continue;
               }
             if (pres->priority < c->cur->priority) continue;
             c->cur = pres;
          }
        if (ev->jid == c->force_resource) eina_stringshare_replace(&c->force_resource, NULL);
        c->plist = eina_list_remove(c->plist, c->cur);
     }
   else
     {
        EINA_LIST_FOREACH_SAFE(c->plist, l, ll, pres)
          {
             if (ev->jid != pres->jid) continue;

             if (ev->jid == c->force_resource) eina_stringshare_replace(&c->force_resource, NULL);
             shotgun_event_presence_free(pres);
             c->plist = eina_list_remove_list(c->plist, l);
             break;
          }
     }
   contact_jids_menu_del(c, ev->jid);
   if (!c->cur)
     {
        contact_presence_clear(c);
        return;
     }
#if 0
   EINA_LIST_FOREACH_SAFE(c->plist, l, ll, pres)
     {
        if (pres->jid) continue;
        c->plist = eina_list_remove_list(c->plist, l);
        shotgun_event_presence_free(pres);
     }
#endif
   c->status = c->cur->status;
   c->description = c->cur->description;
   c->list->list_item_update[c->list->mode](c->list_item);
}

Contact_List *
contact_list_init(UI_WIN *ui, Shotgun_Auth *auth)
{
   Evas_Object *win, *tb, *box;
   Elm_Object_Item *it;
   Contact_List *cl;

   cl = calloc(1, sizeof(Contact_List));
   cl->account = auth;

   if (ui)
     memcpy(cl, ui, sizeof(UI_WIN));
   else
     {
        /* straight to list from launch */
        ui_win_init((UI_WIN*)cl);
     }

   win = cl->win;
   cl->type = 0;
   elm_win_title_set(win, cl->settings->enable_illume ? "Shotgun" : "Contacts");

   IF_ILLUME(cl) evas_object_event_callback_add(cl->illume_frame, EVAS_CALLBACK_RESIZE, (Evas_Object_Event_Cb)_contact_list_resize_cb, cl);
   evas_object_event_callback_add(win, EVAS_CALLBACK_FREE, (Evas_Object_Event_Cb)_contact_list_free_cb, cl);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, (Evas_Object_Event_Cb)_contact_list_window_key, cl);

   if (!ui) box = cl->box;
   else
     {/* from login */
        cl->box = box = elm_box_add(win);
        elm_box_homogeneous_set(box, EINA_FALSE);
        IF_ILLUME(cl)
          WEIGHT(box, 0, EVAS_HINT_EXPAND);
        else
          EXPAND(box);
        evas_object_show(box);

        elm_object_part_content_set(cl->flip, "front", box);
        IF_ILLUME(cl) elm_object_text_set(ui->illume_frame, "Contacts");
        settings_new((UI_WIN*)cl);
     }
   cl->settings->settings_exist = EINA_TRUE;

   tb = elm_toolbar_add(win);
   ALIGN(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_scale_set(tb, 0.8);
   it = elm_toolbar_item_append(tb, "shotgun/logout", "Quit", (Evas_Smart_Cb)_contact_list_quit_cb, cl);
   elm_object_item_tooltip_text_set(it, "Quit SHOTGUN! (Please don't. We will miss you.)");
   elm_object_item_tooltip_window_mode_set(it, EINA_TRUE);
   it = elm_toolbar_item_append(tb, "shotgun/settings", "Settings", (Evas_Smart_Cb)settings_toggle, cl);
   elm_object_item_tooltip_text_set(it, "Change application settings");
   elm_object_item_tooltip_window_mode_set(it, EINA_TRUE);
   elm_box_pack_end(box, tb);
   evas_object_show(tb);

   it = elm_toolbar_item_append(tb, "shotgun/status", "Status", (Evas_Smart_Cb)_contact_list_status_click, cl);
   elm_object_item_tooltip_text_set(it, "Change the current status");
   elm_object_item_tooltip_window_mode_set(it, EINA_TRUE);

   /* FIXME: tooltips need window mode here */
   it = elm_toolbar_item_append(tb, "shotgun/useradd", "Add", (Evas_Smart_Cb)_contact_list_add_cb, cl);
   elm_object_item_tooltip_text_set(it, "Add a new contact");
   elm_object_item_tooltip_window_mode_set(it, EINA_TRUE);
   it = elm_toolbar_item_append(tb, "shotgun/userdel", "Remove", (Evas_Smart_Cb)_contact_list_del_cb, cl);
   elm_object_item_tooltip_text_set(it, "Remove the selected contact");
   elm_object_item_tooltip_window_mode_set(it, EINA_TRUE);

   cl->list_at_xy_item_get[0] = (Contact_List_At_XY_Item_Get)elm_genlist_at_xy_item_get;
   cl->list_at_xy_item_get[1] = NULL;
   //cl->list_at_xy_item_get[1] = (Ecore_Data_Cb)elm_gengrid_at_xy_item_get;
   cl->list_selected_item_get[0] = (Ecore_Data_Cb)elm_genlist_selected_item_get;
   cl->list_selected_item_get[1] = (Ecore_Data_Cb)elm_gengrid_selected_item_get;
   cl->list_item_parent_get[0] = (Ecore_Data_Cb)elm_object_item_widget_get;
   cl->list_item_parent_get[1] = (Ecore_Data_Cb)elm_object_item_widget_get;
   cl->list_item_del[0] = (Ecore_Cb)elm_object_item_del;
   cl->list_item_del[1] = (Ecore_Cb)elm_object_item_del;
   cl->list_item_update[0] = (Ecore_Cb)elm_genlist_item_update;
   cl->list_item_update[1] = (Ecore_Cb)elm_gengrid_item_update;
   cl->list_item_promote[0] = (Ecore_Cb)elm_genlist_item_promote;
   cl->list_item_promote[1] = (Ecore_Cb)NULL;
   cl->list_item_tooltip_add[0] = (Contact_List_Item_Tooltip_Cb)elm_genlist_item_tooltip_content_cb_set;
   cl->list_item_tooltip_add[1] = (Contact_List_Item_Tooltip_Cb)elm_gengrid_item_tooltip_content_cb_set;
   cl->list_item_tooltip_resize[0] = (Contact_List_Item_Tooltip_Resize_Cb)elm_genlist_item_tooltip_window_mode_set;
   cl->list_item_tooltip_resize[1] = (Contact_List_Item_Tooltip_Resize_Cb)elm_gengrid_item_tooltip_window_mode_set;

   _contact_list_list_add(cl);

   cl->users = eina_hash_string_superfast_new(NULL);
//   cl->users = eina_hash_string_superfast_new((Eina_Free_Cb)contact_free);
   cl->images = eina_hash_string_superfast_new((Eina_Free_Cb)chat_image_free);

   cl->event_handlers.iq = ecore_event_handler_add(SHOTGUN_EVENT_IQ,
                                                       (Ecore_Event_Handler_Cb)event_iq_cb, cl);
   cl->event_handlers.presence =
      ecore_event_handler_add(SHOTGUN_EVENT_PRESENCE, (Ecore_Event_Handler_Cb)event_presence_cb,
                              cl);
   cl->event_handlers.message =
      ecore_event_handler_add(SHOTGUN_EVENT_MESSAGE, (Ecore_Event_Handler_Cb)event_message_cb,
                              cl);

   {
      const char *color;
      int x;
      color = elm_theme_data_get(NULL, "shotgun/color/message");
      if (color && (strlen(color) == 6))
        {
           char buf[3];

           for (x = 0; x < 3; x++)
             {
                strncpy(buf, color + (2 * x), 2);
                errno = 0;
                cl->alert_colors[x] = strtol(buf, NULL, 16);
                if (errno)
                  {
                     ERR("Illegal hex color: %s", buf);
                     cl->alert_colors[x] = 1;
                  }
             }
        }
      else
        cl->alert_colors[0] = 32, cl->alert_colors[1] = 68, cl->alert_colors[2] = 87;
      for (x = 0; x < 3; x++)
        {
           if (cl->alert_colors[x] < 1) cl->alert_colors[x] = 1;
        }
      //INF("r, g, b: %d, %d, %d", cl->alert_colors[0], cl->alert_colors[1], cl->alert_colors[2]);
   }


   evas_object_resize(win, cl->settings->list_w ?: 300, cl->settings->list_h ?: 700);
   evas_object_show(win);
   return cl;
}
