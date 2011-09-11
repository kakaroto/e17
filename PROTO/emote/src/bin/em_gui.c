#include "em_global.h"

#define MIN_WIN_WIDTH 200
#define MIN_WIN_HEIGHT 200

/* local function prototypes */
static Eina_Bool _em_gui_chansel_add_channel(const Eina_Hash *hash __UNUSED__, const void *key, void *data, void *fdata __UNUSED__);
static Eina_Bool _em_gui_chansel_add_server(const Eina_Hash *hash __UNUSED__, const void *key, void *data, void *fdata __UNUSED__);
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
static void _em_gui_chansel_cb_server_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_chansel_cb_channel_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_switch(const char *server, const char *channel);
static Em_Gui_Channel *_em_gui_channel_create(const char *server, const char *channel);
static Em_Gui_Server *_em_gui_server_create(const char *server, Emote_Protocol *p);
static Eina_Bool _em_gui_server_first_get(const Eina_Hash *hash __UNUSED__, const void *key, void *data __UNUSED__, void *fdata __UNUSED__);
static Eina_Bool _em_gui_channel_nick_update(const Eina_Hash *hash __UNUSED__, const void *key __UNUSED__, void *data, void *fdata);

/* local variables */
static Em_Gui *gui = NULL;

/* public functions */
EM_INTERN Eina_Bool 
em_gui_init(void)
{
   Evas_Object *o;

   /* allocate our object */
   gui = EM_OBJECT_ALLOC(Em_Gui, EM_GUI_TYPE, _em_gui_cb_free);
   if (!gui) return EINA_FALSE;

   // Set finger size to 4 to avoid huge widgets
//   elm_finger_size_set(4);

   /* create window */
   gui->w_win = elm_win_add(NULL, "emote", ELM_WIN_BASIC);
   elm_win_title_set(gui->w_win, "Emote");
   elm_win_keyboard_mode_set(gui->w_win, ELM_WIN_KEYBOARD_ALPHA);
   evas_object_smart_callback_add(gui->w_win, "delete-request",
                                  _em_gui_cb_win_del, NULL);

   /* create background */
   o = elm_bg_add(gui->w_win);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//   evas_object_size_hint_min_set(o, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   elm_win_resize_object_add(gui->w_win, o);
   evas_object_show(o);

   /* create packing box */
   gui->w_box = elm_box_add(gui->w_win);
   elm_win_resize_object_add(gui->w_win, gui->w_box);
   evas_object_size_hint_weight_set(gui->w_box, EVAS_HINT_EXPAND, 
                                    EVAS_HINT_EXPAND);
   evas_object_show(gui->w_box);

   /* create main toolbar */
   gui->w_tb = elm_toolbar_add(gui->w_win);
   elm_toolbar_icon_size_set(gui->w_tb, (16 * elm_scale_get()));
   elm_toolbar_align_set(gui->w_tb, 0.0);
   elm_toolbar_mode_shrink_set(gui->w_tb, ELM_TOOLBAR_SHRINK_SCROLL);
   evas_object_size_hint_weight_set(gui->w_tb, 0.0, 0.0);
   evas_object_size_hint_align_set(gui->w_tb, EVAS_HINT_FILL, 0.0);

   elm_toolbar_item_append(gui->w_tb, "preferences-system", _("Settings"), 
                        _em_gui_cb_settings, NULL);
   elm_toolbar_item_append(gui->w_tb, "application-exit", _("Quit"), 
                        _em_gui_cb_quit, NULL);
   evas_object_show(gui->w_tb);
   elm_box_pack_start(gui->w_box, gui->w_tb);

   /* create channel selector */
   gui->w_chansel = elm_toolbar_add(gui->w_win);
   elm_toolbar_homogeneous_set(gui->w_chansel, EINA_FALSE);
   elm_toolbar_align_set(gui->w_chansel, 0.0);
   elm_toolbar_mode_shrink_set(gui->w_chansel, ELM_TOOLBAR_SHRINK_SCROLL);
   evas_object_size_hint_weight_set(gui->w_chansel, 0.0, 0.0);
   evas_object_size_hint_align_set(gui->w_chansel, EVAS_HINT_FILL, 0.0);
   evas_object_show(gui->w_chansel);
   elm_box_pack_end(gui->w_box, gui->w_chansel);

   /* set min size of window and show it */
//   evas_object_size_hint_min_set(gui->w_win, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   evas_object_resize(gui->w_win, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   evas_object_show(gui->w_win);

   gui->servers = eina_hash_string_small_new(_em_gui_server_del);

   return EINA_TRUE;
}

EM_INTERN Eina_Bool 
em_gui_shutdown(void)
{
   if (gui) em_object_del(EM_OBJECT(gui));
   return EINA_TRUE;
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

   if ((gui->servers) && (eina_hash_population(gui->servers)))
     eina_hash_foreach(gui->servers, _em_gui_server_first_get, NULL);
   else
     _em_gui_chansel_update("");
}

EM_INTERN void
em_gui_channel_add(const char *server, const char *channel, Emote_Protocol *p __UNUSED__)
{
   Em_Gui_Channel *c;

   if ((c = _em_gui_channel_create(server, channel)))
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
   char buf[PATH_MAX];
   Em_Gui_Server *s;
   Em_Gui_Channel *c;

   if (user)
     snprintf(buf, sizeof(buf), "<br>%s: %s", user, text);
   else
     snprintf(buf, sizeof(buf), "<br>*: %s", text);

   s = eina_hash_find(gui->servers, server);
   if (channel)
     {
        if ((c = eina_hash_find(s->channels, channel)))
          {
             elm_scrolled_entry_cursor_end_set(c->w_text);
             elm_scrolled_entry_entry_insert(c->w_text, buf);
          }
        else
          {
             elm_scrolled_entry_cursor_end_set(s->w_text);
             elm_scrolled_entry_entry_insert(s->w_text, buf);
          }
     }
   else
     {
        elm_scrolled_entry_cursor_end_set(s->w_text);
        elm_scrolled_entry_entry_insert(s->w_text, buf);
     }
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
   em_gui_message_add(c->server->name, c->name, NULL, buf);
   elm_object_text_set(c->w_nick, n);

   return EINA_TRUE;
}

EM_INTERN void
em_gui_nick_update(const char *server, const char *old, const char *new)
{
   Em_Gui_Server *s;
   char buf[8192];

   s = eina_hash_find(gui->servers, server);

   if ((!old) || (!strcmp(old, s->nick)))
     {
        elm_object_text_set(s->w_nick, new);
        eina_hash_foreach(s->channels, _em_gui_channel_nick_update, new);

        if (s->nick) eina_stringshare_del(s->nick);
        s->nick = eina_stringshare_add(new);
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

   if (!strcmp(user, s->nick))
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

   if (!c)
     {
        printf("Server %p, Channel %p\n", s, c);
        return;
     }

   elm_scrolled_entry_cursor_end_set(c->w_text);
   elm_scrolled_entry_entry_insert(c->w_text, buf);

   //em_gui_message_add(server, channel, NULL, buf);
}

/* local functions */
static Eina_Bool
_em_gui_chansel_add_channel(const Eina_Hash *hash __UNUSED__, const void *key, void *data, void *fdata __UNUSED__)
{
   Em_Gui_Channel *c;

   c = data;
   if (!c->w_select) 
     {
        c->w_select = 
          elm_toolbar_item_append(gui->w_chansel, NULL, key, 
                                  _em_gui_chansel_cb_channel_clicked, c);
     }

  return EINA_TRUE;
}

static Eina_Bool
_em_gui_chansel_add_server(const Eina_Hash *hash __UNUSED__, const void *key, void *data, void *fdata __UNUSED__)
{
   Em_Gui_Server *s;

   s = data;
   if (!s->w_select) 
     {
        s->w_select = 
          elm_toolbar_item_append(gui->w_chansel, NULL, key, 
                                  _em_gui_chansel_cb_server_clicked, s);
     }

  eina_hash_foreach(s->channels, _em_gui_chansel_add_channel, NULL);

  return EINA_TRUE;
}

static void
_em_gui_chansel_update(const char *label)
{
   Elm_Toolbar_Item *item;

   eina_hash_foreach(gui->servers, _em_gui_chansel_add_server, NULL);

   if ((item = elm_toolbar_item_find_by_label(gui->w_chansel, label)))
     elm_toolbar_item_selected_set(item, EINA_TRUE);
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
   evas_object_del(elm_object_parent_widget_get(s->w_entry));
   evas_object_del(s->w_text);
   evas_object_del(s->w_box);

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
   evas_object_del(elm_object_parent_widget_get(c->w_entry));
   evas_object_del(c->w_text);
   evas_object_del(c->w_box);

   elm_toolbar_item_del(c->w_select);
}

static void
_em_gui_cb_free(void *obj)
{
   Em_Gui *g;

   g = (Em_Gui *)obj;
   /* FIXME: This is dumb ... why are we not freeing this obj ? */
}

static void
_em_gui_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   elm_exit();
}

static void
_em_gui_cb_settings(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   Elm_Toolbar_Item *it = elm_toolbar_selected_item_get(gui->w_tb);
   if (it)
     elm_toolbar_item_selected_set(it, EINA_FALSE);
   em_config_show(gui->w_win);
}

static void
_em_gui_cb_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   elm_exit();
}

static void 
_em_gui_chansel_cb_server_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__) 
{
   Em_Gui_Server *s;

   s = data;
   _em_gui_switch(s->name, NULL);
}

static void 
_em_gui_chansel_cb_channel_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__) 
{
   Em_Gui_Channel *c;

   c = data;
   _em_gui_switch(c->server->name, c->name);
}

static void
_em_gui_server_send_message(Emote_Protocol *p, const char *server, const char *nick, const char *text)
{
   Emote_Event *d;

   d = emote_event_new(p, EMOTE_EVENT_SERVER_MESSAGE_SEND, server, nick, text);
   emote_event_send(d);
}

static void
_em_gui_server_entry_cb_enter(void *data, Evas_Object *obj, void *event __UNUSED__)
{
   const char *text;
   Em_Gui_Server *s;

   s = (Em_Gui_Server *)data;
   printf("Sending message to %s", s->name);

   text = elm_scrolled_entry_entry_get(obj);
   if ((text[0] == '/') && (text[1] != '/'))
     _em_gui_server_send_message(s->protocol, s->name, s->nick, text);

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

   printf("Sending message to %s->%s", c->server->name, c->name);

   // No / or double //
   if (!((text[0] == '/') && (text[1] != '/')))
     {
        d = emote_event_new(c->server->protocol, EMOTE_EVENT_CHAT_MESSAGE_SEND,
                            c->server->name, c->name, c->server->nick, text);
        emote_event_send(d);

        // We have // at beginning of string
        if (text[0] == '/') text++;

        // Add message to textblock
        em_gui_message_add(c->server->name, c->name, c->server->nick, text);
     }
   else // Single /, don't print to textblock
     {
        _em_gui_server_send_message(c->server->protocol, c->server->name,
                                    c->server->nick, text);
     }

    // Clear entry
    elm_scrolled_entry_entry_set((Evas_Object *)obj, NULL);
}

static void
_em_gui_switch(const char *server, const char *channel)
{
   char buf[256];
   Em_Gui_Server *s;

   if (gui->w_block)
     {
        printf("HIDE EXISTING BLOCK\n");
        elm_box_unpack(gui->w_box, gui->w_block);
        evas_object_hide(gui->w_block);
        gui->w_block = NULL;
     }

   s = eina_hash_find(gui->servers, server);
   if (!channel)
     {
        printf("Server Block\n");
        gui->w_block = s->w_box;
        _em_gui_chansel_update(server);
        snprintf(buf, sizeof(buf), "Emote - %s", server);
     }
   else
     {
        Em_Gui_Channel *c;

        printf("Channel Block\n");
        c = eina_hash_find(s->channels, channel);
        gui->w_block = c->w_box;
        _em_gui_chansel_update(channel);
        snprintf(buf, sizeof(buf), "Emote - %s : %s", server, channel);
     }

   evas_object_show(gui->w_block);
//   elm_box_pack_end(gui->w_box, gui->w_block);
   elm_box_pack_before(gui->w_box, gui->w_block, gui->w_chansel);

   elm_win_title_set(gui->w_win, buf);
}

static Em_Gui_Server *
_em_gui_server_create(const char *server, Emote_Protocol *p)
{
   Em_Gui_Server *s;
   Evas_Object *o;

   /* Allocate new server widgets */
   s = EM_OBJECT_ALLOC(Em_Gui_Server, EM_GUI_SERVER_TYPE, _em_gui_server_free);

   /* create packing box */
   s->w_box = elm_box_add(gui->w_win);
   evas_object_size_hint_weight_set(s->w_box, EVAS_HINT_EXPAND, 
                                    EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->w_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_min_set(s->w_box, 100, 400);
   evas_object_show(s->w_box);

   s->w_text = elm_scrolled_entry_add(gui->w_win);
   elm_scrolled_entry_editable_set(s->w_text, EINA_FALSE);
   elm_scrolled_entry_line_wrap_set(s->w_text, EINA_TRUE);
   evas_object_size_hint_weight_set(s->w_text, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->w_text, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(s->w_text);
   elm_box_pack_end(s->w_box, s->w_text);

   o = elm_box_add(gui->w_win);
   elm_box_horizontal_set(o, EINA_TRUE);
//   elm_box_homogeneous_set(o, EINA_FALSE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(o);
   elm_box_pack_end(s->w_box, o);

   s->w_nick = elm_label_add(gui->w_win);
   elm_label_line_wrap_set(s->w_nick, 0);
   evas_object_size_hint_weight_set(s->w_nick, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->w_nick, 0.0, EVAS_HINT_FILL);
   evas_object_show(s->w_nick);
   elm_box_pack_end(o, s->w_nick);

   s->w_entry = elm_scrolled_entry_add(gui->w_win);
   elm_scrolled_entry_single_line_set(s->w_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(s->w_entry, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(s->w_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(s->w_entry, "activated", 
                                  _em_gui_server_entry_cb_enter, s);
   evas_object_show(s->w_entry);
   elm_box_pack_end(o, s->w_entry);

   /* Init channel hash */
   s->channels = eina_hash_string_small_new(_em_gui_channel_del);

   s->w_select = 
     elm_toolbar_item_append(gui->w_chansel, NULL, server, 
                             _em_gui_chansel_cb_server_clicked, s);

   s->name = eina_stringshare_add(server);

   s->protocol = p;
   return s;
}

static Em_Gui_Channel *
_em_gui_channel_create(const char *server, const char *channel)
{
   Em_Gui_Channel *c;
   Evas_Object *o;

   /* Allocate new server widgets */
   c = EM_OBJECT_ALLOC(Em_Gui_Channel, EM_GUI_CHANNEL_TYPE, 
                       _em_gui_channel_free);
   c->server = eina_hash_find(gui->servers, server);

   /* create packing box */
   c->w_box = elm_box_add(gui->w_win);
   elm_box_horizontal_set(c->w_box, EINA_FALSE);
   elm_box_homogeneous_set(c->w_box, EINA_FALSE);
   evas_object_size_hint_weight_set(c->w_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->w_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
//   elm_win_resize_object_add(gui->w_win, c->w_box);
//   evas_object_show(c->w_box);

   /* Create textblock for server text */
   c->w_text = elm_scrolled_entry_add(gui->w_win);
   elm_scrolled_entry_editable_set(c->w_text, EINA_FALSE);
   elm_scrolled_entry_line_wrap_set(c->w_text, EINA_TRUE);
   evas_object_size_hint_weight_set(c->w_text, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->w_text, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(c->w_box, c->w_text);
   evas_object_show(c->w_text);

   /* Create box to hold nick and entry */
   o = elm_box_add(gui->w_win);
   elm_box_horizontal_set(o, EINA_TRUE);
   elm_box_homogeneous_set(o, EINA_FALSE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(c->w_box, o);
   evas_object_show(o);

   /* Create label for nick */
   c->w_nick = elm_label_add(gui->w_win);
   elm_object_text_set(c->w_nick, c->server->nick);
   elm_label_line_wrap_set(c->w_nick, 0);
   evas_object_size_hint_weight_set(c->w_nick, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->w_nick, 0.0, EVAS_HINT_FILL);
   elm_box_pack_end(o, c->w_nick);
   evas_object_show(c->w_nick);

   /* create entry for user input */
   c->w_entry = elm_scrolled_entry_add(gui->w_win);
   elm_scrolled_entry_single_line_set(c->w_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(c->w_entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->w_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(o, c->w_entry);
   evas_object_smart_callback_add(c->w_entry, "activated", 
                                  _em_gui_channel_entry_cb_enter, c);
   evas_object_show(c->w_entry);

   c->w_select = 
      elm_toolbar_item_append(gui->w_chansel, NULL, channel, 
                              _em_gui_chansel_cb_channel_clicked, c);

   c->name = eina_stringshare_add(channel);

   eina_hash_add(c->server->channels, channel, c);

   return c;
}
