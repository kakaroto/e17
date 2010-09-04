#include "em_global.h"

#define MIN_WIN_WIDTH 200
#define MIN_WIN_HEIGHT 200

/* local function prototypes */
static Eina_Bool _em_gui_chansel_add_channel(const Eina_Hash *hash __UNUSED__, const void *key, void *data __UNUSED__, void *fdata __UNUSED__);
static Eina_Bool _em_gui_chansel_add_server(const Eina_Hash *hash __UNUSED__, const void *key, void *data __UNUSED__, void *fdata __UNUSED__);
static void _em_gui_chansel_update(const char *label);
static void _em_gui_server_del(void *obj);
static void _em_gui_server_free(void *obj);
static void _em_gui_channel_del(void *obj);
static void _em_gui_channel_free(void *obj);
static void _em_gui_cb_free(void *obj);
static void _em_gui_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_cb_settings(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_cb_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_server_entry_cb_enter(void *data, Evas_Object *obj, void *event __UNUSED__);
static void _em_gui_channel_entry_cb_enter(void *data, Evas_Object *obj, void *event __UNUSED__);
static void _em_gui_server_send_message(Emote_Protocol *p, const char *server, const char *nick, const char *text);
static void _em_gui_hoversel_cb_server_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_hoversel_cb_channel_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_switch(const char *server, const char *channel);
static Em_Gui_Channel *_em_gui_channel_create(const char *server, const char *channel);
static Em_Gui_Server *_em_gui_server_create(const char *server, Emote_Protocol *p);
static Eina_Bool _em_gui_server_first_get(const Eina_Hash *hash __UNUSED__, const void *key, void *data __UNUSED__, void *fdata __UNUSED__);
static Eina_Bool _em_gui_channel_nick_update(const Eina_Hash *hash __UNUSED__, const void *key __UNUSED__, void *data, void *fdata);

/* local variables */
static Em_Gui *gui = NULL;

/* public functions */
EM_INTERN int
em_gui_init(void)
{
   Evas_Object *o;

   /* allocate our object */
   gui = EM_OBJECT_ALLOC(Em_Gui, EM_GUI_TYPE, _em_gui_cb_free);
   if (!gui) return 0;

   /* create window */
   gui->win = elm_win_add(NULL, "emote", ELM_WIN_BASIC);
   elm_win_title_set(gui->win, "Emote");
   elm_win_keyboard_mode_set(gui->win, ELM_WIN_KEYBOARD_ALPHA);
   evas_object_smart_callback_add(gui->win, "delete-request",
                                  _em_gui_cb_win_del, NULL);

   /* create background */
   o = elm_bg_add(gui->win);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_min_set(o, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   elm_win_resize_object_add(gui->win, o);
   evas_object_show(o);

   /* create packing box */
   gui->o_box = elm_box_add(gui->win);
   elm_box_horizontal_set(gui->o_box, EINA_FALSE);
   elm_box_homogenous_set(gui->o_box, EINA_FALSE);
   evas_object_size_hint_weight_set(gui->o_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gui->o_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(gui->win, gui->o_box);
   evas_object_show(gui->o_box);

   /* create hover select at top for quick channel change */
   gui->o_chansel = elm_hoversel_add(gui->win);
   elm_hoversel_label_set(gui->o_chansel, _(""));
   elm_hoversel_hover_parent_set(gui->o_chansel, gui->win);
   evas_object_size_hint_weight_set(gui->o_chansel, 0.0, 0.0);
   evas_object_size_hint_align_set(gui->o_chansel, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(gui->o_box, gui->o_chansel);
   evas_object_show(gui->o_chansel);

   /* create main toolbar */
   gui->o_tb = elm_toolbar_add(gui->win);
   elm_toolbar_icon_size_set(gui->o_tb, 16);
   elm_toolbar_align_set(gui->o_tb, 0.0);
   elm_toolbar_scrollable_set(gui->o_tb, EINA_FALSE);
   evas_object_size_hint_weight_set(gui->o_tb, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(gui->o_tb, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(gui->o_box, gui->o_tb);
   evas_object_show(gui->o_tb);

   o = em_util_icon_add(gui->win, "preferences-system");
   elm_toolbar_item_add(gui->o_tb, o, _("Settings"), _em_gui_cb_settings, NULL);
   o = em_util_icon_add(gui->win, "application-exit");
   elm_toolbar_item_add(gui->o_tb, o, _("Quit"), _em_gui_cb_quit, NULL);

   /* set min size of window and show it */
   evas_object_size_hint_min_set(gui->win, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   evas_object_resize(gui->win, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   evas_object_show(gui->win);

   gui->servers = eina_hash_string_small_new(_em_gui_server_del);

   return 1;
}

EM_INTERN void
em_gui_server_add(const char *server, Emote_Protocol *p)
{
   Em_Gui_Server *s;

   s = _em_gui_server_create(server, p);
   if (s)
     {
        eina_hash_set(gui->servers, server, s);
        _em_gui_switch(server, NULL);
     }
}

EM_INTERN void
em_gui_server_del(const char *server, Emote_Protocol *p __UNUSED__)
{
   eina_hash_del_by_key(gui->servers, server);

   if (gui->servers && eina_hash_population(gui->servers))
     eina_hash_foreach(gui->servers, _em_gui_server_first_get, NULL);
   else
     _em_gui_chansel_update("");
}

EM_INTERN void
em_gui_channel_add(const char *server, const char *channel, Emote_Protocol *p __UNUSED__)
{
   Em_Gui_Channel *c;

   c = _em_gui_channel_create(server, channel);

   if (c)
     _em_gui_switch(server, channel);
}

EM_INTERN void
em_gui_channel_del(const char *server, const char *channel, Emote_Protocol *p __UNUSED__)
{
   Em_Gui_Server *s;

   s = eina_hash_find(gui->servers, server);
   eina_hash_del_by_key(s->channels, channel);

  _em_gui_switch(server, NULL);
}

EM_INTERN void
em_gui_message_add(const char *server, const char *channel, const char *user, const char *text)
{
   char buf[4096];
   Em_Gui_Server *s;
   Em_Gui_Channel *c;

   if (user)
     snprintf(buf, sizeof(buf), "<br>%s: %s", user, text);
   else
     snprintf(buf, sizeof(buf), "<br>*: %s", text);

   s = eina_hash_find(gui->servers, server);
   if (channel)
     {
        c = eina_hash_find(s->channels, channel);
        if (c)
          {
             elm_scrolled_entry_cursor_end_set(c->text);
             elm_scrolled_entry_entry_insert(c->text, buf);
          }
        else
          {
             elm_scrolled_entry_cursor_end_set(s->text);
             elm_scrolled_entry_entry_insert(s->text, buf);
          }
     }
   else
     {
        elm_scrolled_entry_cursor_end_set(s->text);
        elm_scrolled_entry_entry_insert(s->text, buf);
     }
}

EM_INTERN int
em_gui_shutdown(void)
{
   if (gui) em_object_del(EM_OBJECT(gui));
   return 1;
}

static Eina_Bool
_em_gui_channel_nick_update(const Eina_Hash *hash __UNUSED__, const void *key __UNUSED__, void *data, void *fdata)
{
   Em_Gui_Channel *c;
   const char *n;
   char buf[8192];

   c = data;
   n = fdata;

   snprintf(buf, sizeof(buf), "You are now known as %s", n);
   em_gui_message_add(elm_hoversel_item_label_get(c->server->select),
                      elm_hoversel_item_label_get(c->select),
                      NULL,
                      buf
                      );
   elm_label_label_set(c->nick, n);

   return EINA_TRUE;
}

EM_INTERN void
em_gui_nick_update(const char *server, const char *old, const char *new)
{
   Em_Gui_Server *s;
   char buf[8192];

   s = eina_hash_find(gui->servers, server);

   if (!old || !strcmp(old, elm_label_label_get(s->nick)))
   {
     elm_label_label_set(s->nick, new);
     eina_hash_foreach(s->channels, _em_gui_channel_nick_update, new);
   }
   else
   {
     // TODO: Add way to track users in a channel so we can send this
     //       message to the relevant channels instead of server window
     snprintf(buf, sizeof(buf), "%s is now known as %s", old, new);
     em_gui_message_add(server, NULL, NULL, buf);
   }
}

EM_INTERN void
em_gui_channel_status_update(const char *server, const char *channel, const char *user, Emote_Protocol *p, Eina_Bool joined)
{
   Em_Gui_Server *s;
   char buf[8192];

   s = eina_hash_find(gui->servers, server);

   if (!strcmp(user, elm_label_label_get(s->nick)))
     {
        if (joined)
          {
             snprintf(buf, sizeof(buf), "You have joined %s", channel);
             em_gui_channel_add(server, channel, p);
          }
        else
          {
             snprintf(buf, sizeof(buf), "You have left %s", channel);
             em_gui_channel_del(server, channel, p);
          }
     }
   else
     {
        if (joined)
          snprintf(buf, sizeof(buf), "%s has joined %s", user, channel);
        else
          snprintf(buf, sizeof(buf), "%s has left %s", user, channel);
     }

   em_gui_message_add(server, channel, NULL, buf);
}

EM_INTERN void
em_gui_topic_show(const char *server, const char *channel, const char *user, const char *message)
{
   Em_Gui_Server *s;
   char buf[8192];

   s = eina_hash_find(gui->servers, server);

   if (user)
     snprintf(buf, sizeof(buf), "%s has changed topic to: %s", user, message);
   else
     snprintf(buf, sizeof(buf), "Topic for %s is: %s", channel, message);

   em_gui_message_add(server, channel, NULL, buf);
}

EM_INTERN void
em_gui_user_list_add(const char *server, const char *channel, const char *users)
{
   static Eina_Bool started = EINA_FALSE;
   char buf[8192];
   Em_Gui_Server *s;
   Em_Gui_Channel *c;

   s = eina_hash_find(gui->servers, server);
   c = eina_hash_find(s->channels, channel);

   if (!users)
     {
        started = EINA_FALSE;
        return;
     }

   if (started == EINA_FALSE)
     {
        snprintf(buf, sizeof(buf), "<br>*: Users in %s:<br>%s", channel, users);
        started = EINA_TRUE;
     }
   else
     snprintf(buf, sizeof(buf), "%s", users);

   elm_scrolled_entry_cursor_end_set(c->text);
   elm_scrolled_entry_entry_insert(c->text, buf);

   //em_gui_message_add(server, channel, NULL, buf);
}

/* local functions */
static Eina_Bool
_em_gui_chansel_add_channel(const Eina_Hash *hash __UNUSED__, const void *key, void *data __UNUSED__, void *fdata __UNUSED__)
{
  Em_Gui_Channel *c;

  c = data;

  c->select = elm_hoversel_item_add(gui->o_chansel, (const char *)key, NULL, ELM_ICON_NONE,
                         _em_gui_hoversel_cb_channel_clicked, c);

  return EINA_TRUE;
}

static Eina_Bool
_em_gui_chansel_add_server(const Eina_Hash *hash __UNUSED__, const void *key, void *data __UNUSED__, void *fdata __UNUSED__)
{
  Em_Gui_Server *s;

  s = data;

  s->select = elm_hoversel_item_add(gui->o_chansel, (const char *)key, NULL, ELM_ICON_NONE,
                         _em_gui_hoversel_cb_server_clicked, s);

  eina_hash_foreach(s->channels, _em_gui_chansel_add_channel, NULL);

  return EINA_TRUE;
}

static void
_em_gui_chansel_update(const char *label)
{
  elm_hoversel_clear(gui->o_chansel);

  eina_hash_foreach(gui->servers, _em_gui_chansel_add_server, NULL);

  elm_hoversel_label_set(gui->o_chansel, label);
}

static void
_em_gui_server_del(void *obj)
{
   em_object_del(EM_OBJECT(obj));
}

static Eina_Bool
_em_gui_server_first_get(const Eina_Hash *hash __UNUSED__, const void *key, void *data __UNUSED__, void *fdata __UNUSED__)
{
   _em_gui_switch((const char *)key, NULL);

   return EINA_FALSE;
}

static void
_em_gui_server_free(void *obj)
{
   Em_Gui_Server *s;

   s = obj;
   evas_object_del(elm_object_parent_widget_get(s->entry));
   evas_object_del(s->text);
   evas_object_del(s->box);

   eina_hash_free(s->channels);
}

static void
_em_gui_channel_del(void *obj)
{
   em_object_del(EM_OBJECT(obj));
}

static void
_em_gui_channel_free(void *obj)
{
   Em_Gui_Channel *c;

   c = obj;
   evas_object_del(elm_object_parent_widget_get(c->entry));
   evas_object_del(c->text);
   evas_object_del(c->box);

   elm_hoversel_item_del(c->select);
}

static void
_em_gui_cb_free(void *obj)
{
   Em_Gui *g;

   g = (Em_Gui *)obj;
}

static void
_em_gui_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   elm_exit();
}

static void
_em_gui_cb_settings(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   elm_toolbar_item_unselect_all(gui->o_tb);
   em_config_show(gui->win);
}

static void
_em_gui_cb_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   elm_exit();
}

static void
_em_gui_hoversel_cb_server_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   Em_Gui_Server *s;

   s = data;
   _em_gui_switch(elm_hoversel_item_label_get(s->select), NULL);
}

static void
_em_gui_hoversel_cb_channel_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   Em_Gui_Channel *c;

   c = data;
   _em_gui_switch(elm_hoversel_item_label_get(c->server->select),
                 elm_hoversel_item_label_get(c->select));
}

static void
_em_gui_server_send_message(Emote_Protocol *p, const char *server, const char *nick, const char *text)
{
   Emote_Event *d;

   d = emote_event_new(
                         p,
                         EMOTE_EVENT_SERVER_MESSAGE_SEND,
                         server,
                         nick,
                         text
                      );
   emote_event_send(d);
}

static void
_em_gui_server_entry_cb_enter(void *data, Evas_Object *obj, void *event __UNUSED__)
{
   const char *text;
   Em_Gui_Server *s;

   s = (Em_Gui_Server *)data;
   printf("Sending message to %s", elm_hoversel_item_label_get(s->select));

   text = elm_scrolled_entry_entry_get(obj);
   if ((text[0] == '/') && (text[1] != '/'))
     {
        _em_gui_server_send_message(
                                      s->protocol,
                                      elm_hoversel_item_label_get(s->select),
                                      elm_label_label_get(s->nick),
                                      text
                                   );
     }
   // Clear entry
   elm_scrolled_entry_entry_set((Evas_Object *)obj, NULL);
}

static void
_em_gui_channel_entry_cb_enter(void *data, Evas_Object *obj, void *event __UNUSED__)
{
   const char *text;
   Emote_Event *d;
   Em_Gui_Channel *c;
  // char buf[8192];

   c = (Em_Gui_Channel *)data;
   text = elm_scrolled_entry_entry_get(obj);

   printf("Sending message to %s->%s", elm_hoversel_item_label_get(c->server->select),
                                        elm_hoversel_item_label_get(c->select));

   // No / or double //
   if (!((text[0] == '/') && (text[1] != '/')))
     {
        d = emote_event_new(
                              c->server->protocol,
                              EMOTE_EVENT_CHAT_MESSAGE_SEND,
                              elm_hoversel_item_label_get(c->server->select),
                              elm_hoversel_item_label_get(c->select),
                              elm_label_label_get(c->nick),
                              text
                           );
        emote_event_send(d);

        // We have // at beginning of string
        if (text[0] == '/') text++;

        // Add message to textblock
        em_gui_message_add(elm_hoversel_item_label_get(c->server->select),
                           elm_hoversel_item_label_get(c->select),
                           elm_label_label_get(c->nick),
                           text);
     }
   else // Single /, don't print to textblock
     {
        _em_gui_server_send_message(
                                      c->server->protocol,
                                      elm_hoversel_item_label_get(c->server->select),
                                      elm_label_label_get(c->server->nick),
                                      text
                                   );
     }

    // Clear entry
    elm_scrolled_entry_entry_set((Evas_Object *)obj, NULL);
}

static void
_em_gui_switch(const char *server, const char *channel)
{
   char buf[256];
   Em_Gui_Server *s;
   Em_Gui_Channel *c;

   if (gui->o_block)
     {
        evas_object_hide(gui->o_block);
        elm_box_unpack(gui->o_box, gui->o_block);
     }

   s = eina_hash_find(gui->servers, server);
   if (!channel)
     {
        gui->o_block = s->box;
        _em_gui_chansel_update(server);
        snprintf(buf, sizeof(buf), "Emote - %s", server);
     }
   else
     {
        c = eina_hash_find(s->channels, channel);
        gui->o_block = c->box;
        _em_gui_chansel_update(channel);
        snprintf(buf, sizeof(buf), "Emote - %s : %s", server, channel);
     }

   elm_box_pack_after(gui->o_box, gui->o_block, gui->o_chansel);
   evas_object_show(gui->o_block);
   elm_win_title_set(gui->win, buf);
}

static Em_Gui_Server *
_em_gui_server_create(const char *server, Emote_Protocol *p)
{
   Em_Gui_Server *s;
   Evas_Object *o;

   /* Allocate new server widgets */
   s = EM_OBJECT_ALLOC(Em_Gui_Server, EM_GUI_SERVER_TYPE, _em_gui_server_free);

   /* create packing box */
   s->box = elm_box_add(gui->win);
   elm_box_horizontal_set(s->box, EINA_FALSE);
   elm_box_homogenous_set(s->box, EINA_FALSE);
   evas_object_size_hint_weight_set(s->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(gui->win, s->box);

   /* Create textblock for server text */
   s->text = elm_scrolled_entry_add(gui->win);
   elm_scrolled_entry_editable_set(s->text, EINA_FALSE);
   elm_scrolled_entry_line_wrap_set(s->text, EINA_TRUE);
   evas_object_size_hint_weight_set(s->text, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->text, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(s->box, s->text);
   evas_object_show(s->text);

   /* Create box to hold nick and entry */
   o = elm_box_add(gui->win);
   elm_box_horizontal_set(o, EINA_TRUE);
   elm_box_homogenous_set(o, EINA_FALSE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(s->box, o);
   evas_object_show(o);

   /* Create label for nick */
   s->nick = elm_label_add(gui->win);
   elm_label_label_set(s->nick, "");
   elm_label_line_wrap_set(s->nick, 0);
   evas_object_size_hint_weight_set(s->nick, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->nick, 0.0, EVAS_HINT_FILL);
   elm_box_pack_end(o, s->nick);
   evas_object_show(s->nick);

   /* create entry for user input */
   s->entry = elm_scrolled_entry_add(gui->win);
   elm_scrolled_entry_single_line_set(s->entry, EINA_TRUE);
   evas_object_size_hint_weight_set(s->entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(o, s->entry);
   evas_object_smart_callback_add(s->entry, "activated", _em_gui_server_entry_cb_enter, s);
   evas_object_show(s->entry);

   /* Init channel hash */
   s->channels = eina_hash_string_small_new(_em_gui_channel_del);

   s->select = elm_hoversel_item_add(gui->o_chansel, server, NULL, ELM_ICON_NONE,
                         _em_gui_hoversel_cb_server_clicked, s);

   s->protocol = p;
   return s;
}

static Em_Gui_Channel *
_em_gui_channel_create(const char *server, const char *channel)
{
   Em_Gui_Channel *c;
   Evas_Object *o;

   /* Allocate new server widgets */
   c = EM_OBJECT_ALLOC(Em_Gui_Channel, EM_GUI_CHANNEL_TYPE, _em_gui_channel_free);
   c->server = eina_hash_find(gui->servers, server);

   /* create packing box */
   c->box = elm_box_add(gui->win);
   elm_box_horizontal_set(c->box, EINA_FALSE);
   elm_box_homogenous_set(c->box, EINA_FALSE);
   evas_object_size_hint_weight_set(c->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(gui->win, c->box);

   /* Create textblock for server text */
   c->text = elm_scrolled_entry_add(gui->win);
   elm_scrolled_entry_editable_set(c->text, EINA_FALSE);
   elm_scrolled_entry_line_wrap_set(c->text, EINA_TRUE);
   evas_object_size_hint_weight_set(c->text, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->text, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(c->box, c->text);
   evas_object_show(c->text);

   /* Create box to hold nick and entry */
   o = elm_box_add(gui->win);
   elm_box_horizontal_set(o, EINA_TRUE);
   elm_box_homogenous_set(o, EINA_FALSE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(c->box, o);
   evas_object_show(o);

   /* Create label for nick */
   c->nick = elm_label_add(gui->win);
   elm_label_label_set(c->nick, elm_label_label_get(c->server->nick));
   elm_label_line_wrap_set(c->nick, 0);
   evas_object_size_hint_weight_set(c->nick, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->nick, 0.0, EVAS_HINT_FILL);
   elm_box_pack_end(o, c->nick);
   evas_object_show(c->nick);

   /* create entry for user input */
   c->entry = elm_scrolled_entry_add(gui->win);
   elm_scrolled_entry_single_line_set(c->entry, EINA_TRUE);
   evas_object_size_hint_weight_set(c->entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(o, c->entry);
   evas_object_smart_callback_add(c->entry, "activated", _em_gui_channel_entry_cb_enter, c);
   evas_object_show(c->entry);

   c->select = elm_hoversel_item_add(gui->o_chansel, channel, NULL, ELM_ICON_NONE,
                         _em_gui_hoversel_cb_channel_clicked, c);

   eina_hash_add(c->server->channels, channel, c);

   return c;
}
