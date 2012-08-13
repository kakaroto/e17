#include "ui.h"

enum
{
   SETTINGS_BROWSER_BROWSER = 0,
   SETTINGS_BROWSER_CHROME,
   SETTINGS_BROWSER_FIREFOX,
   SETTINGS_BROWSER_OPERA,
   SETTINGS_BROWSER_OTHER
};

static char *BROWSERS[] = { NULL, "chrome", "firefox", "opera", NULL };

#define SETTINGS_FRAME(LABEL) do { \
   fr = elm_frame_add(ui->win); \
   WEIGHT(fr, EVAS_HINT_EXPAND, 0.0); \
   ALIGN(fr, EVAS_HINT_FILL, 0.5); \
   elm_object_text_set(fr, LABEL); \
   elm_frame_autocollapse_set(fr, EINA_TRUE); \
   elm_box_pack_end(box, fr); \
   evas_object_show(fr); \
\
   frbox = elm_box_add(ui->win); \
   EXPAND(frbox); \
   FILL(frbox); \
   elm_object_content_set(fr, frbox); \
   evas_object_show(frbox); \
} while (0)

#define SETTINGS_CHECK(LABEL, POINTER, TOOLTIP) do { \
   ck = elm_check_add(fr); \
   EXPAND(ck); \
   FILL(ck); \
   elm_object_text_set(ck, LABEL); \
   elm_check_state_pointer_set(ck, &ui->settings->POINTER); \
   elm_object_tooltip_text_set(ck, TOOLTIP); \
   elm_object_tooltip_window_mode_set(ck, EINA_TRUE); \
   elm_box_pack_end(frbox, ck); \
   evas_object_show(ck); \
} while (0)

#define SETTINGS_CHECK_CALLBACK(CALLBACK) do { \
   IF_UI_IS_NOT_LOGIN(ui) \
     evas_object_smart_callback_add(ck, "changed", (Evas_Smart_Cb)(CALLBACK), cl); \
} while (0)

#define SETTINGS_SUBRADIO(LABEL, STATE, TOOLTIP) \
   SETTINGS_RADIO(LABEL, STATE, (TOOLTIP)); \
   elm_radio_group_add(sradio, radio)

#define SETTINGS_RADIO(LABEL, STATE, TOOLTIP) \
   sradio = elm_radio_add(fr); \
   EXPAND(sradio); \
   ALIGN(sradio, 0, EVAS_HINT_FILL); \
   elm_radio_state_value_set(sradio, SETTINGS_BROWSER_##STATE); \
   elm_object_text_set(sradio, LABEL); \
   elm_object_tooltip_text_set(sradio, TOOLTIP); \
   elm_object_tooltip_window_mode_set(sradio, EINA_TRUE); \
   evas_object_smart_callback_add(sradio, "changed", (Evas_Smart_Cb)_settings_browser_change, ui); \
   elm_box_pack_end(frbox, sradio); \
   evas_object_show(sradio)

#define SETTINGS_SLIDER(LABEL, TOOLTIP, UNITS, MAX, CB) do { \
   sl = elm_slider_add(fr); \
   EXPAND(sl); \
   FILL(sl); \
   elm_slider_unit_format_set(sl, UNITS); \
   elm_slider_min_max_set(sl, 0, MAX); \
   elm_object_text_set(sl, LABEL); \
   elm_object_tooltip_text_set(sl, TOOLTIP); \
   elm_object_tooltip_window_mode_set(sl, EINA_TRUE); \
   evas_object_smart_callback_add(sl, "delay,changed", (Evas_Smart_Cb)_settings_ ##CB## _change, ui); \
   elm_box_pack_end(frbox, sl); \
   evas_object_show(sl); \
} while (0)

static void
_settings_image_size_change(UI_WIN *ui, Evas_Object *obj, void *event_info __UNUSED__)
{
   ui->settings->allowed_image_size = elm_slider_value_get(obj);
}

static void
_settings_image_age_change(UI_WIN *ui, Evas_Object *obj, void *event_info __UNUSED__)
{
   ui->settings->allowed_image_age = elm_slider_value_get(obj);
}

static void
_settings_logging_change(Contact_List *cl, Evas_Object *obj, void *event_info __UNUSED__)
{
   Eina_List *l, *ll;
   const char *dir;
   Chat_Window *cw;
   Contact *c;

   if (!elm_check_state_get(obj))
     {
        /* close existing logs */
        EINA_LIST_FOREACH(cl->users_list, l, c)
          logging_contact_file_close(c);
        return;
     }

   dir = logging_dir_get();
   if (!dir[0]) logging_dir_create(cl);
   EINA_LIST_FOREACH(cl->chat_wins, l, cw)
     {
        EINA_LIST_FOREACH(cw->contacts, ll, c)
          /* open logs for all open chats */
          logging_contact_file_refresh(c);
     }
}

static void
_settings_chat_resource_ignore_toggle(Contact_List *cl, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Eina_List *l;
   Contact *c;

   EINA_LIST_FOREACH(cl->users_list, l, c)
     chat_resource_ignore_toggle(c, NULL, NULL);
}

static void
_settings_chat_typing_change(Contact_List *cl, Evas_Object *obj, void *event_info __UNUSED__)
{
   Eina_List *l, *ll;
   Chat_Window *cw;
   Contact *c;

   if (!elm_check_state_get(obj))
     {
        EINA_LIST_FOREACH(cl->chat_wins, l, cw)
          {
             EINA_LIST_FOREACH(cw->contacts, ll, c)
               evas_object_smart_callback_del(c->chat_input, "changed,user", (Evas_Smart_Cb)contact_chat_window_typing);
          }
        return;
     }
   EINA_LIST_FOREACH(cl->chat_wins, l, cw)
     {
        EINA_LIST_FOREACH(cw->contacts, ll, c)
          evas_object_smart_callback_add(c->chat_input, "changed,user", (Evas_Smart_Cb)contact_chat_window_typing, c);
     }
}

static void
_settings_chat_status_entry_change(Contact_List *cl, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Eina_List *l, *ll;
   Chat_Window *cw;
   Contact *c;

   EINA_LIST_FOREACH(cl->chat_wins, l, cw)
     {
        EINA_LIST_FOREACH(cw->contacts, ll, c)
          chat_status_entry_toggle(c);
     }
}

static void
_settings_browser_entry_cb(Evas_Object *hv, Evas_Object *obj __UNUSED__, void *ev __UNUSED__)
{
   elm_hover_dismiss(hv);
}

static void
_settings_browser_hover_close(UI_WIN *ui, Evas_Object *obj, void *ev __UNUSED__)
{
   Evas_Object *box, *o;
   Eina_List *l;
   const char *str;
   char buf[1024];

   box = elm_object_part_content_get(obj, "middle");
   l = elm_box_children_get(box);
   o = eina_list_data_get(eina_list_last(l));
   eina_list_free(l);
   str = elm_entry_entry_get(o);
   if ((!str) || (!str[0])) str = NULL;
   eina_stringshare_replace(&ui->settings->browser, str);
   ui_key_grab_set(ui, "Escape", EINA_TRUE);
   o = evas_object_data_get(ui->win, "browser-radio");
   snprintf(buf, sizeof(buf), "Other (%s)", str ?: "NOT SET");
   elm_object_text_set(o, buf);
   evas_object_del(obj);
}

static void
_settings_otr_change(UI_WIN *ui, Evas_Object *obj __UNUSED__, void *ev __UNUSED__)
{
   shotgun_iq_otr_set(ui->account, ui->settings->enable_global_otr);
}

static void
_settings_browser_change(UI_WIN *ui, Evas_Object *radio, void *ev __UNUSED__)
{
   Evas_Object *hv, *ent, *o, *box;
   unsigned int val;
   char buf[1024];

   val = elm_radio_value_get(radio);
   switch (val)
     {
      case SETTINGS_BROWSER_BROWSER:
        eina_stringshare_replace(&ui->settings->browser, getenv("BROWSER"));
        break;
      case SETTINGS_BROWSER_OTHER:
        hv = elm_hover_add(ui->win);
        box = elm_box_add(ui->win);
        evas_object_show(box);
        o = elm_label_add(ui->win);
        elm_object_text_set(o, "Enter browser command:");
        elm_box_pack_end(box, o);
        ALIGN(o, 0.5, EVAS_HINT_FILL);
        evas_object_show(o);
        ent = elm_entry_add(ui->win);
        ALIGN(ent, 0.5, EVAS_HINT_FILL);
        elm_box_pack_end(box, ent);
        elm_entry_single_line_set(ent, EINA_TRUE);
        elm_entry_scrollable_set(ent, EINA_FALSE);
        elm_entry_entry_set(ent, ui->settings->browser);
        elm_entry_select_all(ent);
        elm_entry_cursor_begin_set(ent);
        elm_object_part_content_set(hv, "middle", box);
        evas_object_show(ent);
        evas_object_smart_callback_add(ent, "activated", (Evas_Smart_Cb)_settings_browser_entry_cb, hv);
        evas_object_smart_callback_add(hv, "clicked", (Evas_Smart_Cb)_settings_browser_hover_close, ui);
        ui_key_grab_set(ui, "Escape", EINA_FALSE);
        elm_hover_parent_set(hv, ui->win);
        elm_hover_target_set(hv, elm_object_parent_widget_get(radio));
        evas_object_show(hv);
        elm_object_focus_set(ent, EINA_TRUE);
        return;
      default:
        eina_stringshare_replace(&ui->settings->browser, BROWSERS[val]);
     }
   o = evas_object_data_get(ui->win, "browser-radio");
   snprintf(buf, sizeof(buf), "Other (%s)", ui->settings->browser ?: "NOT SET");
   elm_object_text_set(o, buf);
}

static void
_settings_mail_notify_change(UI_WIN *ui, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   shotgun_iq_gsettings_mailnotify_set(ui->account, ui->settings->enable_mail_notifications);
}

void
settings_new(UI_WIN *ui)
{
   Evas_Object *scr, *ic, *back, *box, *ck, *fr, *frbox, *sl, *radio, *sradio;
   int init;
   Contact_List *cl;
   Login_Window *lw;
   char buf[1024];

   IF_UI_IS_LOGIN(ui) lw = (Login_Window*)ui;
   else cl = (Contact_List*)ui;

   ui->settings_box = box = elm_box_add(ui->win);
   EXPAND(box);
   FILL(box);
   elm_object_part_content_set(ui->flip, "back", box);
   evas_object_show(box);

   ic = elm_icon_add(ui->win);
   elm_icon_standard_set(ic, "back");
   evas_object_show(ic);
   back = elm_button_add(ui->win);
   elm_object_content_set(back, ic);
   IF_UI_IS_LOGIN(ui)
     elm_object_tooltip_text_set(back, "Return to login");
   else
     elm_object_tooltip_text_set(back, "Return to contact list");
   elm_object_tooltip_window_mode_set(back, EINA_TRUE);
   WEIGHT(back, 0, 0);
   ALIGN(back, 0, 0);
   elm_box_pack_end(box, back);
   evas_object_smart_callback_add(back, "clicked", (Evas_Smart_Cb)settings_toggle, ui);
   evas_object_show(back);

   scr = elm_scroller_add(ui->win);
   EXPAND(scr);
   FILL(scr);
   elm_box_pack_end(box, scr);

   box = elm_box_add(ui->win);
   EXPAND(box);
   FILL(box);
   evas_object_show(box);

   elm_object_content_set(scr, box);
   elm_scroller_bounce_set(scr, EINA_FALSE, EINA_FALSE);
   elm_scroller_policy_set(scr, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
   evas_object_show(scr);

   SETTINGS_FRAME("Account");
   SETTINGS_CHECK("Save account info", enable_account_info, "Remember account name and password");
   IF_UI_IS_LOGIN(ui) elm_object_disabled_set(ck, EINA_TRUE);
   SETTINGS_CHECK("Remember last account", enable_last_account, "Automatically sign in with current account on next run");
   IF_UI_IS_LOGIN(ui) elm_object_disabled_set(ck, EINA_TRUE);
   SETTINGS_CHECK("Remember last presence", enable_presence_save, "Automatically set last-used presence on next sign in");
   IF_UI_IS_LOGIN(ui) elm_object_disabled_set(ck, EINA_TRUE);

   SETTINGS_FRAME("Application");
   elm_frame_collapse_set(fr, EINA_TRUE);
   SETTINGS_CHECK("Enable single window mode", enable_illume, "Use a single window for the application - REQUIRES RESTART (embedded friendly)");
   SETTINGS_CHECK("Disable automatic reconnect", disable_reconnect, "Disable automatic reconnection when disconnected");
   SETTINGS_CHECK("Check for new email", enable_mail_notifications, "Show notifications when new emails are received");
   IF_UI_IS_LOGIN(ui) elm_object_disabled_set(ck, EINA_TRUE);
   else if (!shotgun_iq_gsettings_available(ui->account)) elm_object_disabled_set(ck, EINA_TRUE);
   SETTINGS_CHECK_CALLBACK(_settings_mail_notify_change);

   SETTINGS_FRAME("Browser");
   elm_frame_collapse_set(fr, EINA_TRUE);
   radio = SETTINGS_RADIO("BROWSER", BROWSER, "Use BROWSER environment variable");
   SETTINGS_SUBRADIO("Chrome", CHROME, "Use Chrome browser");
   SETTINGS_SUBRADIO("Firefox", FIREFOX, "Use Firefox browser");
   SETTINGS_SUBRADIO("Opera", OPERA, "Use Opera browser");
   snprintf(buf, sizeof(buf), "Other (%s)", ui->settings->browser ?: "NOT SET");
   SETTINGS_SUBRADIO(buf, OTHER, "Use user-defined browser");
   evas_object_data_set(ui->win, "browser-radio", sradio);

#ifdef HAVE_NOTIFY
   SETTINGS_FRAME("DBus");
   elm_frame_collapse_set(fr, EINA_TRUE);
   SETTINGS_CHECK("Disable notifications", disable_notify, "Disables use of notification popups");
#endif

   init = ecore_con_url_init();
   if (init)
     SETTINGS_FRAME("Images");
   else
     SETTINGS_FRAME("Images (DISABLED: REQUIRES CURL SUPPORT IN ECORE)");
   elm_frame_collapse_set(fr, EINA_TRUE);
   SETTINGS_CHECK("Disable automatic image fetching", disable_image_fetch, "Disables background fetching of images");
   if (init)
     {
        SETTINGS_SLIDER("Max image age", "Number of days to save linked images on disk before deleting them",
                        "%1.0f days", 60, image_age);
        SETTINGS_SLIDER("Max image memory", "Total size of images to keep in memory",
                        "%1.0f MB", 512, image_size);
     }
   else
     {
        elm_check_state_set(ck, EINA_TRUE);
        elm_object_disabled_set(ck, EINA_TRUE);
     }

   SETTINGS_FRAME("List");
   elm_frame_collapse_set(fr, EINA_TRUE);
   SETTINGS_CHECK("Don't show status in list item", disable_list_status, "Do not show a contact's status inlined under the name");
   SETTINGS_CHECK_CALLBACK(contact_list_mode_toggle);
   SETTINGS_CHECK("Promote contact on message", enable_chat_promote, "Move contact to top of list when message is received<ps>This will modify alphabetical sorting");
   SETTINGS_CHECK("Alphabetize contact list", enable_list_sort_alpha, "Sort the contact list alphabetically<ps>Toggling this will sort your list");
   SETTINGS_CHECK_CALLBACK(contact_list_mode_toggle);
   SETTINGS_CHECK("Show offline contacts", enable_list_offlines, "Show offline contacts in the list");
   SETTINGS_CHECK_CALLBACK(contact_list_show_toggle);


   SETTINGS_FRAME("Messages");
   elm_frame_collapse_set(fr, EINA_TRUE);
   SETTINGS_CHECK("Send keyboard events", enable_chat_typing, "Send additional notifications to contacts when you start or stop typing to them");
   SETTINGS_CHECK_CALLBACK(_settings_chat_typing_change);
   SETTINGS_CHECK("Disable chat status message display", disable_chat_status_entry, "Disable the text showing the contact's status in the chat window");
   SETTINGS_CHECK_CALLBACK(_settings_chat_status_entry_change);
   SETTINGS_CHECK("Focus chat window on message", enable_chat_focus, "Focus chat window whenever message is received");
   SETTINGS_CHECK("Always select new chat tabs", enable_chat_newselect, "When a message is received which would open a new tab, make that tab active");
   SETTINGS_CHECK("Send messages to all resources", enable_chat_noresource,
                  "XMPP specifies that only the highest priority instance of a contact should receive messages<ps>"
                  "This option ignores resource priority, sending to all instances by default<ps>"
                  "This setting can also be toggled on a contact-by-contact basis");
   SETTINGS_CHECK_CALLBACK(_settings_chat_resource_ignore_toggle);
   SETTINGS_CHECK("Log messages to disk", enable_logging, "All messages sent or received will appear in ~/.config/shotgun/logs");
   IF_UI_IS_LOGIN(ui) elm_object_disabled_set(ck, EINA_TRUE);
   SETTINGS_CHECK_CALLBACK(_settings_logging_change);
   SETTINGS_CHECK("Enable OTR", enable_global_otr, "Enable \"Off-The-Record\" conversations globally: no conversations will be logged by the server");
   IF_UI_IS_LOGIN(ui) elm_object_disabled_set(ck, EINA_TRUE);
   SETTINGS_CHECK_CALLBACK(_settings_otr_change);
}

void
settings_toggle(UI_WIN *ui, Evas_Object *obj __UNUSED__, void *event_info)
{
   Contact_List *cl;
   Login_Window *lw;

   IF_UI_IS_LOGIN(ui) lw = (Login_Window*)ui;
   else cl = (Contact_List*)ui;

   IF_UI_IS_NOT_LOGIN(ui)
     {
        if ((!cl->image_cleaner) && cl->settings->allowed_image_age)
          ui_eet_idler_start(cl);
        chat_image_cleanup(cl);
     }
   IF_ILLUME(ui)
     {
        if (elm_flip_front_visible_get(ui->flip))
          elm_object_text_set(ui->illume_frame, "Settings");
        else
          elm_object_text_set(ui->illume_frame, "Contacts");
     }
   IF_UI_IS_NOT_LOGIN(ui)
     {
        if (event_info) elm_toolbar_item_selected_set(event_info, EINA_FALSE);
     }
   if (elm_flip_front_visible_get(ui->flip))
     settings_new(ui);
   elm_flip_go(ui->flip, ELM_FLIP_ROTATE_Y_CENTER_AXIS);
}

void
settings_finagle(UI_WIN *ui)
{
   int argc, x;
   char **argv;
   Shotgun_Settings *ss;

   ss = ui_eet_settings_get(ui->account);
   ecore_app_args_get(&argc, &argv);
   if (ss) ui->settings = ss;
   else
     {
        int x, dash = 0;

        ss = ui->settings = calloc(1, sizeof(Shotgun_Settings));
        /* don't count --enable/disable args */
        for (x = 1; x < argc; x++)
          if (argv[x][0] == '-') dash++;
        switch (argc - dash)
          {
           case 1:
             ui->settings->enable_last_account = EINA_TRUE;
           case 3:
             ui->settings->enable_account_info = EINA_TRUE;
           default:
             break;
          }
        shotgun_settings_set(ui->account, ss);
     }

   ss->ui = ui;
   for (x = 1; x < argc; x++)
     {
        if ((!strcmp(argv[x], "--illume")) || (!strcmp(argv[x], "--enable-illume")))
          ui->settings->enable_illume = EINA_TRUE;
        if (!strcmp(argv[x], "--disable-illume"))
          ui->settings->enable_illume = EINA_FALSE;
     }
   INF("ILLUME: %s", ui->settings->enable_illume ? "ENABLED" : "DISABLED");
}
