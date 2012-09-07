#include "ui.h"

static void
_chat_conv_anchor_click(Contact *c, Evas_Object *obj __UNUSED__, Elm_Entry_Anchor_Info *ev)
{
   DBG("anchor click: '%s' (%i, %i)", ev->name, ev->x, ev->y);
   switch (ev->button)
     {
      case 1:
        {
           char *cmd;
           size_t size;

           if (!c->list->settings->browser)
             {
                DBG("BROWSER set to NULL; not opening link");
                return;
             }
           size = sizeof(char) * (strlen(ev->name) + strlen(c->list->settings->browser) + 3) + 1;
           if (size > 32000)
             cmd = malloc(size);
           else
             cmd = alloca(size);
           snprintf(cmd, size, "%s '%s'", c->list->settings->browser, ev->name);
           DBG("Running BROWSER command: %s", cmd);
           ecore_exe_run(cmd, NULL);
           if (size > 32000) free(cmd);
           return;
        }
      case 3:
        {
           size_t len;
           len = strlen(ev->name);
           if (len == sizeof(int)) len++; /* workaround for stupid elm cnp bug which breaks the universe */
           elm_cnp_selection_set(c->chat_window->win, ELM_SEL_TYPE_CLIPBOARD, ELM_SEL_FORMAT_TEXT, ev->name, len);
        }
      default:
        break;
     }
}

void
chat_message_insert(Contact *c, const char *from, const char *msg, Eina_Bool me)
{
   size_t len;
   char timebuf[11];
   char *buf, *s;
   Evas_Object *e = c->chat_buffer;
   const char *color;

   len = strftime(timebuf, sizeof(timebuf), "[%H:%M:%S]",
            localtime((time_t[]){ ecore_time_unix_get() }));

   s = elm_entry_utf8_to_markup(msg);
   if (me)
     {
        color = elm_theme_data_get(NULL, "shotgun/color/me");
        if ((!color) || (eina_strlen_bounded(color, 7) != 6))
          {
             DBG("valid shotgun/color/me data not present in theme!");
             color = "00FF01";
          }
     }
   else
     {
        color = elm_theme_data_get(NULL, "shotgun/color/you");
        if ((!color) || (eina_strlen_bounded(color, 7) != 6))
          {
             DBG("valid shotgun/color/you data not present in theme!");
             color = "0001FF";
          }
     }
   len += strlen(from) + strlen(s) + sizeof("<color=#123456>%s <b>%s:</b></color> %s<ps>") + 5;
   buf = alloca(len);
   snprintf(buf, len, "<color=#%s>%s <b>%s:</b></color> %s<ps>", color, timebuf, from, s);
   free(s);

#ifdef HAVE_NOTIFY
   if (!me)
     {
        Eina_Bool notify = EINA_FALSE;
        //if (!contact_chat_window_current(c))
        if (!elm_win_focus_get(c->chat_window->win)) notify = EINA_TRUE;
        else
          {
             if (!contact_chat_window_current(c)) notify = EINA_TRUE;
          }
        if (notify)
          {
             Evas_Object *img = NULL;

             if (c->info && c->info->photo.size)
               {
                  char buf[1024];

                  snprintf(buf, sizeof(buf), "%s/%s/img", shotgun_jid_get(c->list->account), c->base->jid);
                  img = evas_object_image_add(evas_object_evas_get(c->list->win));
                  evas_object_image_file_set(img, eet_file_get(shotgun_data_get(c->list->account)), buf);
               }
             ui_dbus_notify(c->list, img, from, msg);
             if (img) evas_object_del(img);
          }
     }
#endif
   elm_entry_entry_append(e, buf);
   if (c->log)
     {
        /* switch <ps> for \n to be more readable */
        len--;
        while (buf[len] != '>') len--;
        fwrite(buf, sizeof(char), len, c->log);
        fwrite("\n", sizeof(char), 1, c->log);
     }
   elm_entry_cursor_end_set(e);
   if (c->list->settings->enable_chat_focus)
     elm_win_activate(c->chat_window->win);
   if (c->list_item && c->list->settings->enable_chat_promote)
     /* FIXME: gengrid doesn't have item promote */
     if (c->list->list_item_promote[c->list->mode])
       /* FIXME: FIXME: fuck gengrid */
       c->list->list_item_promote[c->list->mode](c->list_item);
}

void
chat_message_status(Contact *c __UNUSED__, Shotgun_Event_Message *msg)
{
   switch (msg->status)
     {
      case SHOTGUN_MESSAGE_STATUS_ACTIVE:
      case SHOTGUN_MESSAGE_STATUS_COMPOSING:
      case SHOTGUN_MESSAGE_STATUS_PAUSED:
      case SHOTGUN_MESSAGE_STATUS_INACTIVE:
      case SHOTGUN_MESSAGE_STATUS_GONE:
      default:
        break;
     }
}

static void
_chat_window_focus(Chat_Window *cw, Evas_Object *obj __UNUSED__, void *ev __UNUSED__)
{
   Evas_Object *panes;
   Contact *c;

   panes = elm_object_content_get(cw->pager);
   c = evas_object_data_get(panes, "contact");
   if (!c) return;
   elm_object_focus_set(c->chat_input, EINA_TRUE);
}

static void
_chat_window_send_cb(Chat_Window *cw)
{
   char *s;
   const char *jid, *txt;
   Contact *c;

   c = cw->contacts->data;
   /* FIXME: add popup error or something */
   if (shotgun_connection_state_get(c->list->account) != SHOTGUN_CONNECTION_STATE_CONNECTED) return;

   txt = elm_entry_entry_get(c->chat_input);
   if ((!txt) || (!txt[0])) return;

   s = elm_entry_markup_to_utf8(txt);

   if (c->ignore_resource)
     jid = c->base->jid;
   else if (c->force_resource)
     jid = c->force_resource;
   else if (c->cur)
     jid = c->cur->jid;
   else
     jid = c->base->jid;
   shotgun_message_send(c->base->account, jid, s, SHOTGUN_MESSAGE_STATUS_ACTIVE);
   chat_message_insert(c, "me", s, EINA_TRUE);
#ifdef HAVE_DBUS
   ui_dbus_signal_message_self(c->list, jid, s);
#endif
   elm_entry_entry_set(c->chat_input, "");
   elm_entry_cursor_end_set(c->chat_input);
   if (c->sms_timer) ecore_timer_del(c->sms_timer);
   c->sms_timer = NULL;

   free(s);
}

static void
_chat_window_close_cb(Chat_Window *cw, Evas_Object *obj __UNUSED__, const char *ev __UNUSED__)
{
   contact_chat_window_close(evas_object_data_get(elm_object_content_get(cw->pager), "contact"));
}

static void
_chat_window_scrollback_clear_cb(Contact *c, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   eina_stringshare_del(c->last_conv);
   elm_entry_entry_set(c->chat_buffer, "");
}

static void
_chat_window_archive_get_cb(Contact *c, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   shotgun_iq_archive_get(c->list->account, c->base->jid, 10);
}

static void
_chat_window_otr_toggle_cb(Contact *c, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (shotgun_iq_contact_otr_set(c->list->account, c->base->jid, !c->otr_enabled))
     c->otr_enabled = !c->otr_enabled;
}

static void
_chat_window_toolbar_menu(Contact *c)
{
   Evas_Coord x, y;
   Evas_Object *win, *menu, *ck;
   Elm_Object_Item *it;

   win = c->chat_window->win;
   menu = elm_menu_add(win);
   evas_pointer_canvas_xy_get(evas_object_evas_get(menu), &x, &y);
   elm_menu_move(menu, x, y);
   evas_object_show(menu);

   elm_menu_item_add(menu, NULL, NULL, "Clear chat scrollback", (Evas_Smart_Cb)_chat_window_scrollback_clear_cb, c);
   it = elm_menu_item_add(menu, NULL, NULL, NULL, (Evas_Smart_Cb)_chat_window_otr_toggle_cb, c);
   ck = elm_check_add(win);
   if (c->list->settings->enable_global_otr)
     {
        elm_check_state_set(ck, EINA_TRUE);
        elm_object_text_set(ck, "OTR GLOBALLY ENABLED");
     }
   else
     {
        elm_check_state_set(ck, c->otr_enabled);
        elm_object_text_set(ck, "Toggle OTR");
     }
   evas_object_show(ck);
   elm_object_item_content_set(it, ck);
   if ((!shotgun_iq_otr_available(c->list->account)) || c->list->settings->enable_global_otr)
     elm_object_item_disabled_set(it, EINA_TRUE);
   elm_menu_item_add(menu, NULL, NULL, "Request chat archive", (Evas_Smart_Cb)_chat_window_archive_get_cb, c);
   elm_menu_item_separator_add(menu, NULL);
   contact_resource_menu_setup(c, menu);
}

static void
_chat_window_longpress(Chat_Window *cw __UNUSED__, Evas_Object *obj __UNUSED__, Elm_Object_Item *it)
{
   Contact *c;

   c = elm_object_item_data_get(it);
   _chat_window_toolbar_menu(c);
}

static void
_chat_conv_filter_helper(Contact_List *cl, int d, Eina_Strbuf **sbuf, const char *http, size_t *len)
{
   Eina_Strbuf *buf = *sbuf;
   const char *lt;
   char fmt[64];
   unsigned int skip = 0;

   lt = memchr(http, '<', *len);
   if (lt) *len = lt - http;

   if (((unsigned int)d != eina_strbuf_length_get(buf)) &&
       (!memcmp(http + *len - 3, "&gt", 3)))
     *len -= 3, skip = 3;
   else if (!strcmp(http + *len - 4, "<ps>"))
     *len -= 4, skip = 4;
   if (http[*len - 1] == '>')
     {
        while (--*len > 1)
          {
             skip++;
             if (http[*len - 1] == '<') break;
          }
     }
   if ((*len <= 1) || (http[*len - 1] != '<') ||
       strcmp(http + *len, "</a>") || (d < 5) ||
       memcmp(http - 5, "href=", 5))
     {
        snprintf(fmt, sizeof(fmt), "<a href=%%.%zus>%%.%zus</a>", *len, *len);
        eina_strbuf_append_printf(buf, fmt, http, http);
     }
   else
     {
        eina_strbuf_free(buf);
        *sbuf = NULL;
     }
   chat_image_add(cl, eina_stringshare_add_length(http, *len));
   *len += skip;
}

static void
_chat_conv_filter(Contact_List *cl, Evas_Object *obj __UNUSED__, char **str)
{
   char *http, *last;
   const char *start, *end;
   Eina_Strbuf *buf;
   size_t len;

   start = *str;
   http = strstr(start, "http");
   if (!http) return;

   buf = eina_strbuf_new();
   while (1)
     {
        int d;

        d = http - start;
        if (d > 0)
          {
             if ((d > 3) && (!memcmp(http - 3, "&lt", 3)))
               eina_strbuf_append_length(buf, start, d - 3);
             else
               eina_strbuf_append_length(buf, start, d);
          }
        start = end = strchr(http, ' ');
        if (!end) /* address goes to end of message */
          {
             len = strlen(http);
             _chat_conv_filter_helper(cl, d, &buf, http, &len);
             //DBG("ANCHOR: ");
             //DBG(fmt, http);
             break;
          }
        len = end - http;
        _chat_conv_filter_helper(cl, d, &buf, http, &len);
             //DBG("ANCHOR: ");
             //DBG(fmt, http);
        last = strstr(start, "http");
        if (!last) break;
        http = last;
     }
   if (!buf) return;
   if (http[len]) eina_strbuf_append(buf, http + len);
   free(*str);
   *str = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
}

static void
_chat_window_otherclick(Elm_Object_Item *it, Evas_Object *obj __UNUSED__, const char *emission, const char *source __UNUSED__)
{
   Contact *c;
   int button;

   c = elm_object_item_data_get(it);
   button = atoi(emission + sizeof("elm,action,click,") - 1);
   if (button == 2) /* middle click */
     contact_chat_window_close(c);
   else
     _chat_window_toolbar_menu(c);
}

static void
_chat_window_key(Chat_Window *cw, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, Evas_Event_Key_Down *ev)
{
   Contact_List *cl;
   Eina_Bool next = EINA_FALSE;
   //DBG("%s", ev->keyname);
   if ((!strcmp(ev->keyname, "Return")) || (!strcmp(ev->keyname, "KP_Enter")))
     {
        _chat_window_send_cb(cw);
        return;
     }
   cl = cw->cl;
   if ((!strcmp(ev->keyname, "Next")) || (!strcmp(ev->keyname, "KP_Next")))
     next = EINA_TRUE;
   if (next || (!strcmp(ev->keyname, "Tab")) || (!strcmp(ev->keyname, "Prior")) || (!strcmp(ev->keyname, "KP_Prior")))
     {
        Elm_Object_Item *cur, *new, *smart;
        Contact *c;
        double timer;
        static double throttle;

        if (elm_toolbar_items_count(cw->toolbar) == 1) return;
        /* fast-repeating keyboards will break this, so throttle here to avoid it */
        timer = ecore_time_get();
        if (timer - throttle < 0.1)
          {
             throttle = timer;
             return;
          }
        throttle = timer;
        cur = elm_toolbar_selected_item_get(cw->toolbar);
        if (next || (evas_key_modifier_is_set(ev->modifiers, "Shift")))
          {
             new = elm_toolbar_item_prev_get(cur);
             if (!new) new = elm_toolbar_last_item_get(cw->toolbar);
             c = elm_object_item_data_get(new);
             if (!c->animator)
               {
                  for (smart = elm_toolbar_item_prev_get(new); smart && (!c->animator); smart = elm_toolbar_item_prev_get(smart))
                    c = elm_object_item_data_get(smart);
                  if (c->animator && smart && (smart != cur)) new = smart;
               }
          }
        else
          {
             new = elm_toolbar_item_next_get(cur);
             if (!new) new = elm_toolbar_first_item_get(cw->toolbar);
             c = elm_object_item_data_get(new);
             if (!c->animator)
               {
                  for (smart = elm_toolbar_item_next_get(new); smart && (!c->animator); smart = elm_toolbar_item_next_get(smart))
                    c = elm_object_item_data_get(smart);
                  if (c->animator && smart && (smart != cur)) new = smart;
               }
          }
        if (new && (new != cur)) elm_toolbar_item_selected_set(new, EINA_TRUE);
     }
   IF_ILLUME(cl)
     {
        if (!strcmp(ev->keyname, "w"))
          _chat_window_close_cb(cw, NULL, NULL);
     }
   else
     {
        if ((!strcmp(ev->keyname, "Escape")) || (!strcmp(ev->keyname, "w")))
          _chat_window_close_cb(cw, NULL, NULL);
     }
}

static void
_chat_window_switch(Contact *c, Evas_Object *obj __UNUSED__, Elm_Object_Item *it __UNUSED__)
{
   if (elm_object_content_get(c->chat_window->pager) == c->chat_panes) return;
   contact_chat_window_animator_del(c);
   elm_naviframe_item_simple_promote(c->chat_window->pager, c->chat_panes);
   elm_win_title_set(c->chat_window->win, contact_name_get(c));
   c->chat_window->contacts = eina_list_promote_list(c->chat_window->contacts, eina_list_data_find_list(c->chat_window->contacts, c));
   elm_object_focus_set(c->chat_input, EINA_TRUE);
}

void
chat_status_entry_toggle(Contact *c)
{
   Evas_Object *status, *box;

   box = elm_object_part_content_get(c->chat_panes, "elm.swallow.left");
   if (c->list->settings->disable_chat_status_entry)
     {
        if (c->status_line)
          {
             evas_object_del(c->status_line);
             c->status_line = NULL;
          }
        return;
     }
   if (c->status_line) return;
   c->status_line = status = elm_entry_add(c->chat_window->win);
   elm_entry_single_line_set(status, 1);
   elm_entry_cnp_mode_set(status, ELM_CNP_MODE_NO_IMAGE);
   elm_scroller_policy_set(status, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_OFF);
   elm_entry_editable_set(status, 0);
   elm_object_focus_allow_set(status, 0);
   elm_entry_scrollable_set(status, 1);
   elm_entry_line_wrap_set(status, ELM_WRAP_MIXED);
   FILL(status);
   WEIGHT(status, EVAS_HINT_EXPAND, 0);
   elm_entry_markup_filter_append(status, (Elm_Entry_Filter_Cb)_chat_conv_filter, c->list);
   evas_object_smart_callback_add(status, "anchor,in", (Evas_Smart_Cb)chat_conv_image_show, c);
   evas_object_smart_callback_add(status, "anchor,out", (Evas_Smart_Cb)chat_conv_image_hide, c);
   evas_object_smart_callback_add(status, "anchor,clicked", (Evas_Smart_Cb)_chat_conv_anchor_click, c);
   if (c->description)
     {
        char *s;

        s = elm_entry_utf8_to_markup(c->description);
        elm_entry_entry_append(status, s);
        free(s);
     }
   elm_box_pack_start(box, status);
   evas_object_show(status);
}

void
chat_resource_ignore_toggle(Contact *c, Evas_Object *obj __UNUSED__, Elm_Object_Item *ev)
{
   Elm_Object_Item *next;

   if (ev)
     c->ignore_resource = !c->ignore_resource;
   else
     {
        ev = elm_menu_first_item_get(c->chat_jid_menu);
        c->ignore_resource = c->list->settings->enable_chat_noresource;
     }
   next = elm_menu_item_next_get(ev);
   if (c->ignore_resource)
     {
        const Eina_List *l;
        Evas_Object *radio;
        elm_object_item_text_set(ev, "Unignore Resource");
        l = elm_menu_item_subitems_get(next);
        radio = elm_object_item_content_get(l->data);
        c->force_resource = NULL;
        elm_radio_state_value_set(radio, 0);
     }
   else
     elm_object_item_text_set(ev, "Ignore Resource");
   elm_object_item_disabled_set(next, c->ignore_resource);
}

void
chat_window_new(Contact_List *cl)
{
   Evas_Object *win, *bg, *box, *tb, *pg;
   Chat_Window *cw;
   Evas *e;
   Evas_Modifier_Mask ctrl, shift, alt;

   cw = calloc(1, sizeof(Chat_Window));

   IF_ILLUME(cl)
     cw->win = win = cl->win;
   else
     {
        cw->win = win = elm_win_add(NULL, "chat-window", ELM_WIN_BASIC);
        evas_object_smart_callback_add(win, "delete,request", (Evas_Smart_Cb)chat_window_free, cw);
     }
   evas_object_smart_callback_add(win, "focus,in", (Evas_Smart_Cb)_chat_window_focus, cw);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, (Evas_Object_Event_Cb)_chat_window_key, cw);
   e = evas_object_evas_get(win);
   ctrl = evas_key_modifier_mask_get(e, "Control");
   shift = evas_key_modifier_mask_get(e, "Shift");
   alt = evas_key_modifier_mask_get(e, "Alt");
   1 | evas_object_key_grab(win, "w", ctrl, shift | alt, 1); /* worst warn_unused ever. */
   1 | evas_object_key_grab(win, "Tab", ctrl, alt, 1); /* worst warn_unused ever. */
   1 | evas_object_key_grab(win, "Prior", ctrl, alt, 1); /* worst warn_unused ever. */
   1 | evas_object_key_grab(win, "KP_Prior", ctrl, alt, 1); /* worst warn_unused ever. */
   1 | evas_object_key_grab(win, "Tab", ctrl | shift, alt, 1); /* worst warn_unused ever. */
   1 | evas_object_key_grab(win, "Return", 0, ctrl | shift | alt, 1); /* worst warn_unused ever. */
   1 | evas_object_key_grab(win, "KP_Enter", 0, ctrl | shift | alt, 1); /* worst warn_unused ever. */

   IF_NOT_ILLUME(cl)
     {
        1 | evas_object_key_grab(win, "Escape", 0, ctrl | shift | alt, 1); /* worst warn_unused ever. */
        evas_object_resize(win, cl->settings->chat_w ?: 550, cl->settings->chat_h ?: 330);
        evas_object_show(win);

        bg = elm_bg_add(win);
        elm_object_focus_allow_set(bg, 0);
        EXPAND(bg);
        elm_win_resize_object_add(win, bg);
        evas_object_show(bg);
     }

   cw->box = box = elm_box_add(win);
   elm_object_focus_allow_set(box, 0);
   EXPAND(box);
   FILL(box);
   elm_win_resize_object_add(win, box);
   IF_ILLUME(cl)
     elm_box_pack_end(cl->illume_box, box);
   evas_object_show(box);

   cw->toolbar = tb = elm_toolbar_add(win);
   elm_toolbar_shrink_mode_set(tb, ELM_TOOLBAR_SHRINK_SCROLL);
   elm_toolbar_select_mode_set(tb, ELM_OBJECT_SELECT_MODE_ALWAYS);
   elm_toolbar_homogeneous_set(tb, 0);
   elm_object_style_set(tb, "item_horizontal");
   evas_object_smart_callback_add(tb, "longpressed", (Evas_Smart_Cb)_chat_window_longpress, cw);
   ALIGN(tb, EVAS_HINT_FILL, 0.5);
   elm_toolbar_align_set(tb, 0.5);
   elm_box_pack_end(box, tb);
   evas_object_show(tb);

   cw->pager = pg = elm_naviframe_add(win);
   EXPAND(pg);
   FILL(pg);
   elm_box_pack_end(box, pg);
   elm_object_style_set(pg, "slide");
   evas_object_show(pg);

   IF_NOT_ILLUME(cl)
     elm_win_activate(win);

   cw->cl = cl;
   cl->chat_wins = eina_list_append(cl->chat_wins, cw);
   IF_ILLUME(cl)
     {
        if (cl->settings->chat_w)
          evas_object_resize(cw->win, cl->settings->chat_w + cl->settings->list_w, MAX(cl->settings->list_h, cl->settings->chat_h));
        else
          evas_object_resize(cw->win, 850, 700);
        evas_object_resize(cw->cl->illume_frame, cl->settings->list_w ?: 300, cl->settings->list_h ?: 700);
     }
}

void
chat_window_chat_new(Contact *c, Chat_Window *cw, Eina_Bool focus)
{
   Evas_Object *win, *box, *convo, *entry, *obj, *panes;
   void *it;
   const char *icon = (c->info && c->info->photo.size) ? NULL : "shotgun/userunknown";

   if (c->list->settings->enable_logging)
     {
        if (!c->log)
          {
             logging_contact_init(c);
             logging_contact_file_refresh(c);
          }
     }
   win = cw->win;
   c->chat_window = cw;
   if (focus)
     cw->contacts = eina_list_prepend(cw->contacts, c);
   else
     cw->contacts = eina_list_append(cw->contacts, c);
   c->chat_tb_item = it = elm_toolbar_item_append(cw->toolbar, icon, contact_name_get(c), (Evas_Smart_Cb)_chat_window_switch, c);
   elm_object_item_tooltip_text_set(it, "Left click to select<ps>"
                                        "Right click or Longpress for options");
   elm_object_item_tooltip_window_mode_set(it, EINA_TRUE);
   obj = elm_toolbar_item_object_get(it);
   edje_object_signal_callback_add(obj, "elm,action,click,*", "elm", (Edje_Signal_Cb)_chat_window_otherclick, it);
   if (!icon)
     {
        char buf[1024];

        snprintf(buf, sizeof(buf), "%s/%s/img", shotgun_jid_get(c->list->account), c->base->jid);
        if (!elm_toolbar_item_icon_file_set(it, eet_file_get(shotgun_data_get(c->list->account)), buf))
          elm_toolbar_item_icon_set(it, "shotgun/userunknown");
     }

   c->chat_panes = panes = elm_panes_add(win);
   EXPAND(panes);
   FILL(panes);
   elm_panes_horizontal_set(panes, EINA_TRUE);

   box = elm_box_add(win);
   elm_object_focus_allow_set(box, 0);
   /* EXPAND(box); */
   elm_object_part_content_set(panes, "elm.swallow.left", box);
   evas_object_show(box);

   chat_status_entry_toggle(c);

   c->chat_buffer = convo = elm_entry_add(win);
   elm_entry_cnp_mode_set(convo, ELM_CNP_MODE_NO_IMAGE);
   elm_scroller_policy_set(convo, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
   elm_entry_editable_set(convo, 0);
   elm_entry_single_line_set(convo, 0);
   elm_entry_scrollable_set(convo, 1);
   elm_object_focus_allow_set(convo, 0);
   elm_entry_line_wrap_set(convo, ELM_WRAP_MIXED);
   elm_entry_markup_filter_append(convo, (Elm_Entry_Filter_Cb)_chat_conv_filter, c->list);
   evas_object_smart_callback_add(convo, "anchor,in", (Evas_Smart_Cb)chat_conv_image_show, c);
   evas_object_smart_callback_add(convo, "anchor,out", (Evas_Smart_Cb)chat_conv_image_hide, c);
   evas_object_smart_callback_add(convo, "anchor,clicked", (Evas_Smart_Cb)_chat_conv_anchor_click, c);
   EXPAND(convo);
   FILL(convo);
   elm_box_pack_end(box, convo);
   evas_object_show(convo);

   c->chat_input = entry = elm_entry_add(win);
   elm_entry_scrollable_set(entry, 1);
   elm_entry_line_wrap_set(entry, ELM_WRAP_MIXED);
   elm_scroller_policy_set(entry, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
   EXPAND(entry);
   FILL(entry);
   evas_object_show(entry);
   if (c->list->settings->enable_chat_typing)
     evas_object_smart_callback_add(entry, "changed,user", (Evas_Smart_Cb)contact_chat_window_typing, c);

   elm_object_part_content_set(panes, "elm.swallow.right", entry);
   elm_panes_content_left_size_set(panes, c->list->settings->chat_panes ?: 0.8);

   evas_object_data_set(panes, "contact", c);
   evas_object_data_set(panes, "list", c->list);

   if (c->last_conv)
     elm_entry_entry_set(convo, c->last_conv);
   elm_entry_cursor_end_set(convo);

   obj = elm_object_content_get(cw->pager);
   elm_naviframe_item_simple_push(cw->pager, panes);
   if (focus)
     {
        elm_win_title_set(cw->win, contact_name_get(c));
        elm_toolbar_item_selected_set(c->chat_tb_item, EINA_TRUE);
        elm_object_focus_set(c->chat_input, EINA_TRUE);
     }
   else
     {
        elm_naviframe_item_simple_promote(cw->pager, obj);
        contact_chat_window_animator_add(c);
     }
   evas_object_show(panes);

   if (c->list->settings->enable_chat_focus)
     elm_win_activate(cw->win);
}

void
chat_window_free(Chat_Window *cw, Evas_Object *obj __UNUSED__, const char *ev __UNUSED__)
{
   Contact *c;
   Contact_List *cl = cw->cl;

   cl->chat_wins = eina_list_remove(cl->chat_wins, cw);
   EINA_LIST_FREE(cw->contacts, c)
     {
        const char *ent = elm_entry_entry_get(c->chat_buffer);
        if (c->last_conv != ent)
          {
             eina_stringshare_del(c->last_conv);
             c->last_conv = eina_stringshare_ref(ent);
          }
        if (c->animator)
          {
             ecore_animator_del(c->animator);
             evas_object_del(c->animated);
          }
        memset(&c->chat_window, 0, sizeof(void*) * 9);
     }
   IF_ILLUME(cl)
     {
        Evas *e;
        Evas_Modifier_Mask ctrl, shift, alt;

        evas_object_event_callback_del_full(cl->win, EVAS_CALLBACK_KEY_DOWN, (Evas_Object_Event_Cb)_chat_window_key, cw);
        evas_object_smart_callback_del(cl->win, "focus,in", (Evas_Smart_Cb)_chat_window_focus);
        e = evas_object_evas_get(cl->win);
        ctrl = evas_key_modifier_mask_get(e, "Control");
        shift = evas_key_modifier_mask_get(e, "Shift");
        alt = evas_key_modifier_mask_get(e, "Alt");
        evas_object_key_ungrab(cl->win, "w", ctrl, shift | alt);
        evas_object_key_ungrab(cl->win, "Tab", ctrl, alt);
        evas_object_key_ungrab(cl->win, "Tab", ctrl | shift, alt);
        evas_object_del(cw->box);
        evas_object_resize(cw->win, cl->settings->list_w ?: 300, cl->settings->list_h ?: 700);
     }
   else
     {
        evas_object_geometry_get(cw->win, NULL, NULL, &cl->settings->chat_w, &cl->settings->chat_h);
        evas_object_del(cw->win);
     }

   free(cw);
}
