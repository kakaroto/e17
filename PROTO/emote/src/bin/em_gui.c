#include "em_global.h"

#define MIN_WIN_WIDTH 200
#define MIN_WIN_HEIGHT 200

/* local function prototypes */
static void _em_gui_cb_free(void);
static void _em_gui_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_cb_settings(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_cb_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_hoversel_cb_item_clicked(void *data, Evas_Object *obj, void *event);
static void _em_gui_entry_cb_enter(void *data __UNUSED__, Evas_Object *obj, void *event __UNUSED__);

/* local variables */
static Em_Gui *gui = NULL;
static Eina_Hash *chantxts = NULL;
static Eina_Hash *channels = NULL;

/* public functions */
EM_INTERN int
em_gui_init(void)
{
   Evas_Object *o;

   channels = eina_hash_string_small_new(NULL);
   chantxts = eina_hash_string_small_new(NULL);

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
   elm_hoversel_label_set(gui->o_chansel, _("Channels"));
   elm_hoversel_hover_parent_set(gui->o_chansel, gui->win);
   evas_object_size_hint_weight_set(gui->o_chansel, 0.0, 0.0);
   evas_object_size_hint_align_set(gui->o_chansel, 0.0, EVAS_HINT_FILL);
   elm_box_pack_end(gui->o_box, gui->o_chansel);
   evas_object_show(gui->o_chansel);

   /* create scrolled entry for channel text */
   gui->o_chantxt = elm_scrolled_entry_add(gui->win);
   elm_scrolled_entry_editable_set(gui->o_chantxt, EINA_FALSE);
   elm_scrolled_entry_line_wrap_set(gui->o_chantxt, EINA_TRUE);
   evas_object_size_hint_weight_set(gui->o_chantxt,
                                    EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gui->o_chantxt,
                                   EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(gui->o_box, gui->o_chantxt);
   evas_object_show(gui->o_chantxt);
   gui->o_current_chantxt = gui->o_chantxt;

   /* create entry for user input */
   gui->o_entry = elm_scrolled_entry_add(gui->win);
   elm_scrolled_entry_single_line_set(gui->o_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(gui->o_entry,
                                    EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_align_set(gui->o_entry,
                                   EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(gui->o_box, gui->o_entry);
   evas_object_smart_callback_add(gui->o_entry, "activated",
                                 _em_gui_entry_cb_enter, NULL);
   evas_object_show(gui->o_entry);

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

   return 1;
}

static void
em_gui_switch_channel(const char *channel)
{
   char buf[256];

   if (gui->o_current_chantxt != NULL)
   {
     evas_object_hide(gui->o_current_chantxt);
     elm_box_unpack(gui->o_box, gui->o_current_chantxt);
   }

   gui->o_current_chantxt = (Evas_Object *)eina_hash_find(chantxts, channel);
   elm_box_pack_after(gui->o_box, gui->o_current_chantxt, gui->o_chansel);
   evas_object_show(gui->o_current_chantxt);

   gui->o_current_channel = (Elm_Hoversel_Item *)eina_hash_find(channels, channel);
   elm_hoversel_label_set(gui->o_chansel, elm_hoversel_item_label_get(gui->o_current_channel));

   snprintf(buf, sizeof(buf), "Emote - %s", channel);
   elm_win_title_set(gui->win, buf);
}

EM_INTERN void
em_gui_server_add(const char *server, Emote_Protocol *p)
{
   Elm_Hoversel_Item *i;

   i = elm_hoversel_item_add(gui->o_chansel, server, NULL, ELM_ICON_NONE,
                         _em_gui_hoversel_cb_item_clicked, i);

   eina_hash_set(chantxts, server, gui->o_chantxt);
   eina_hash_add(channels, server, i);

   em_gui_switch_channel(server);
}

EM_INTERN void
em_gui_server_del(const char *server, Emote_Protocol *p)
{
   elm_hoversel_item_del(eina_hash_find(channels, server));

   eina_hash_del_by_key(channels, server);
}

EM_INTERN void
em_gui_channel_add(const char *server, const char *channel, Emote_Protocol *p)
{
   Elm_Hoversel_Item *i = NULL;
   Evas_Object *t = NULL;

   i = elm_hoversel_item_add(gui->o_chansel, channel, NULL, ELM_ICON_NONE,
                         _em_gui_hoversel_cb_item_clicked, NULL);

   t = elm_scrolled_entry_add(gui->win);
   elm_scrolled_entry_editable_set(t, EINA_FALSE);
   elm_scrolled_entry_line_wrap_set(t, EINA_TRUE);
   evas_object_size_hint_weight_set(t,
                                    EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(t,
                                   EVAS_HINT_FILL, EVAS_HINT_FILL);

   eina_hash_add(chantxts, channel, t);
   eina_hash_add(channels, channel, i);

   em_gui_switch_channel(channel);
}

static Eina_Bool
em_gui_channel_set_first(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
  gui->o_current_chantxt = (Evas_Object *)data;
  return 0;
}

EM_INTERN void
em_gui_channel_del(const char *server, const char *channel, Emote_Protocol *p)
{
  evas_object_del(gui->o_current_chantxt);
  gui->o_current_chantxt = NULL;
  elm_hoversel_item_del(eina_hash_find(channels, channel));

  eina_hash_foreach(chantxts, em_gui_channel_set_first, NULL);

  if (gui->o_current_chantxt == NULL);
  em_gui_switch_channel(NULL);

  eina_hash_del_by_key(channels, channel);
}

EM_INTERN void
em_gui_message_add(const char *server, const char *channel, const char *user, const char *text)
{
   char buf[4096];

   if (user)
     snprintf(buf, sizeof(buf), "%s: %s<br>", user, text);
   else
     snprintf(buf, sizeof(buf), "%s<br>", text);

   if (channel)
   {
     Evas_Object *t;
     t = eina_hash_find(chantxts, channel);
     elm_scrolled_entry_entry_insert(t, buf);
     elm_scrolled_entry_cursor_end_set(t);
   }
   else
   {
     elm_scrolled_entry_entry_insert(gui->o_chantxt, buf);
     elm_scrolled_entry_cursor_end_set(gui->o_chantxt);
   }
}

EM_INTERN int
em_gui_shutdown(void)
{
   if (gui) em_object_del(EM_OBJECT(gui));
   return 1;
}

/* local functions */
static void
_em_gui_cb_free(void)
{
   EM_FREE(gui);
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
_em_gui_hoversel_cb_item_clicked(void *data, Evas_Object *obj, void *event)
{
   printf("Switching to %s\n", elm_hoversel_item_label_get((Elm_Hoversel_Item*)event));
   gui->o_current_channel = ((Elm_Hoversel_Item*)event);

   em_gui_switch_channel(elm_hoversel_item_label_get(gui->o_current_channel));
}

static void
_em_gui_entry_cb_enter(void *data __UNUSED__, Evas_Object *obj, void *event __UNUSED__)
{
   const char *text;
   //char msg[5012];
   Emote_Event *d;

   text = elm_scrolled_entry_entry_get(obj);
   //snprintf(msg, sizeof(msg), "%s", text);

   printf("Sending message to %s", elm_hoversel_item_label_get(gui->o_current_channel));

   // Single /, don't print to textblock
   if (!((text[0] == '/') && (text[1] != '/')) && gui->o_current_chantxt)
   {

      d = emote_event_new(
                            eina_hash_find(em_protocols, "irc"),
                            EMOTE_EVENT_CHAT_MESSAGE_SEND,
                            "irc.freenode.net",
                            elm_hoversel_item_label_get(gui->o_current_channel),
                            "emote",
                            text
                         );
      emote_event_send(d);

      // We have // at beginning of string
      if (text[0] == '/') text++;

      // Add message to textblock
      elm_scrolled_entry_cursor_end_set(gui->o_current_chantxt);
      elm_scrolled_entry_entry_insert(gui->o_current_chantxt, "emote: ");
      elm_scrolled_entry_cursor_end_set(gui->o_current_chantxt);
      elm_scrolled_entry_entry_insert(gui->o_current_chantxt, text);
      elm_scrolled_entry_cursor_end_set(gui->o_current_chantxt);
      elm_scrolled_entry_entry_insert(gui->o_current_chantxt, "<br>");
      elm_scrolled_entry_cursor_end_set(gui->o_current_chantxt);
   }
   else
   {
      d = emote_event_new(
                            eina_hash_find(em_protocols, "irc"),
                            EMOTE_EVENT_SERVER_MESSAGE_SEND,
                            "irc.freenode.net",
                            "emote",
                            text
                         );
      emote_event_send(d);
   }

   // Clear entry
   elm_scrolled_entry_entry_set(obj, NULL);
}
