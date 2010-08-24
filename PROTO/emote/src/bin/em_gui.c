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
static Em_Gui *gui;

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

EM_INTERN void
em_gui_server_add(const char *server, Emote_Protocol *p)
{
   elm_hoversel_item_add(gui->o_chansel, server, NULL, ELM_ICON_NONE, 
                         _em_gui_hoversel_cb_item_clicked, server);
}

EM_INTERN void
em_gui_channel_add(const char *server, const char *channel, Emote_Protocol *p)
{
   elm_hoversel_item_add(gui->o_chansel, channel, NULL, ELM_ICON_NONE, 
                         _em_gui_hoversel_cb_item_clicked, server);
}

EM_INTERN void
em_gui_message_add(const char *server, const char *channel, const char *text)
{
   elm_scrolled_entry_entry_insert(gui->o_chantxt, text);
   elm_scrolled_entry_cursor_end_set(gui->o_chantxt);
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
}  

static void
_em_gui_entry_cb_enter(void *data __UNUSED__, Evas_Object *obj, void *event __UNUSED__)
{
   const char *text;
   char msg[5012];
   Emote_Event_Chat_Channel_Message *d;

   text = elm_scrolled_entry_entry_get(obj);
   snprintf(msg, sizeof(msg), "%s", text);

   d = EM_NEW(Emote_Event_Chat_Channel_Message, 1);
   d->protocol = eina_hash_find(em_protocols, "irc");
   d->server = "irc.freenode.net";
   d->channel = "#emote";
   d->message = msg;
   emote_event_send(EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_SEND, d);
   
   elm_scrolled_entry_cursor_end_set(gui->o_chantxt);
   elm_scrolled_entry_entry_insert(gui->o_chantxt, text);
   elm_scrolled_entry_cursor_end_set(gui->o_chantxt);
   elm_scrolled_entry_entry_insert(gui->o_chantxt, "<br>");
   elm_scrolled_entry_cursor_end_set(gui->o_chantxt);
   elm_scrolled_entry_entry_set(obj, NULL);
}
