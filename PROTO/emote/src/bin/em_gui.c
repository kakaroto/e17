#include "em_global.h"

#define MIN_WIN_WIDTH 640
#define MIN_WIN_HEIGHT 400

/* local function prototypes */
static Eina_Bool        _em_gui_chansel_add_channel(const Eina_Hash *hash __UNUSED__, const void *key, void *data, void *fdata __UNUSED__);
static Eina_Bool        _em_gui_chansel_add_server(const Eina_Hash *hash __UNUSED__, const void *key, void *data, void *fdata __UNUSED__);
static void             _em_gui_chansel_update(const char *label);
static void             _em_gui_server_del(void *obj);
static void             _em_gui_server_free(void *obj);
static void             _em_gui_channel_del(void *obj);
static void             _em_gui_channel_free(void *obj);
static void             _em_gui_cb_free(void *obj);
static void             _em_gui_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void             _em_gui_cb_settings(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void             _em_gui_cb_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void             _em_gui_server_entry_cb_enter(void *data, Evas_Object *obj, void *event __UNUSED__);
static void             _em_gui_channel_entry_cb_enter(void *data, Evas_Object *obj, void *event __UNUSED__);
static void             _em_gui_server_send_message(Emote_Protocol *p, const char *server, const char *nick, const char *text);
static char            *_em_gui_chansel_cb_label_get(void *data, Evas_Object *obj, const char *part);
static void             _em_gui_chansel_cb_server_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void             _em_gui_chansel_cb_channel_clicked(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void             _em_gui_switch(const char *server, const char *channel);
static Eina_Bool        _em_gui_server_first_get(const Eina_Hash *hash __UNUSED__, const void *key, void *data __UNUSED__, void *fdata __UNUSED__);
static Eina_Bool        _em_gui_channel_nick_update(const Eina_Hash *hash __UNUSED__, const void *key __UNUSED__, void *data, void *fdata);
static Em_Gui_Channel  *_em_gui_channel_create(const char *server, const char *channel);
static Em_Gui_Server   *_em_gui_server_create(const char *server, Emote_Protocol *p);
static void             _em_gui_text_update(Em_Gui_Text *text);
static void             _em_gui_text_cb_resized(void *data, Evas *e, Evas_Object *obj, void *event_info);

/* local variables */
static Em_Gui *gui = NULL;

typedef struct _Color Color;

struct _Color
{
   unsigned char r, g, b, a;
};

const Color colors[2][2][12] =
{
   { // normal
        { // normal
             { 0x00, 0x00, 0x00, 0x00 }, // COL_DEF
             { 0x00, 0x00, 0x00, 0xff }, // COL_BLACK
             { 0xcc, 0x33, 0x33, 0xff }, // COL_RED
             { 0x33, 0xcc, 0x33, 0xff }, // COL_GREEN
             { 0xcc, 0x88, 0x33, 0xff }, // COL_YELLOW
             { 0x33, 0x33, 0xcc, 0xff }, // COL_BLUE
             { 0xcc, 0x33, 0xcc, 0xff }, // COL_MAGENTA
             { 0x33, 0xcc, 0xcc, 0xff }, // COL_CYAN
             { 0xcc, 0xcc, 0xcc, 0xff }, // COL_WHITE
             { 0x00, 0x00, 0x00, 0x00 }, // COL_INVIS
             { 0x22, 0x22, 0x22, 0xff }, // COL_INVERSE
             { 0xaa, 0xaa, 0xaa, 0xff }, // COL_INVERSEBG
        },
        { // bright/bold
             { 0xee, 0xee, 0xee, 0xff }, // COL_DEF
             { 0x66, 0x66, 0x66, 0xff }, // COL_BLACK
             { 0xff, 0x66, 0x66, 0xff }, // COL_RED
             { 0x66, 0xff, 0x66, 0xff }, // COL_GREEN
             { 0xff, 0xff, 0x66, 0xff }, // COL_YELLOW
             { 0x66, 0x66, 0xff, 0xff }, // COL_BLUE
             { 0xff, 0x66, 0xff, 0xff }, // COL_MAGENTA
             { 0x66, 0xff, 0xff, 0xff }, // COL_CYAN
             { 0xff, 0xff, 0xff, 0xff }, // COL_WHITE
             { 0x00, 0x00, 0x00, 0x00 }, // COL_INVIS
             { 0x11, 0x11, 0x11, 0xff }, // COL_INVERSE
             { 0xee, 0xee, 0xee, 0xff }, // COL_INVERSEBG
        },
   },
   { // intense
        { // normal
             { 0xdd, 0xdd, 0xdd, 0xff }, // COL_DEF
             { 0x00, 0x00, 0x00, 0xff }, // COL_BLACK
             { 0xcc, 0x88, 0x88, 0xff }, // COL_RED
             { 0x88, 0xcc, 0x88, 0xff }, // COL_GREEN
             { 0xcc, 0xaa, 0x88, 0xff }, // COL_YELLOW
             { 0x88, 0x88, 0xcc, 0xff }, // COL_BLUE
             { 0xcc, 0x88, 0xcc, 0xff }, // COL_MAGENTA
             { 0x88, 0xcc, 0xcc, 0xff }, // COL_CYAN
             { 0xcc, 0xcc, 0xcc, 0xff }, // COL_WHITE
             { 0x00, 0x00, 0x00, 0x00 }, // COL_INVIS
             { 0x11, 0x11, 0x11, 0xff }, // COL_INVERSE
             { 0xcc, 0xcc, 0xcc, 0xff }, // COL_INVERSEBG
        },
        { // bright/bold
             { 0xff, 0xff, 0xff, 0xff }, // COL_DEF
             { 0xcc, 0xcc, 0xcc, 0xff }, // COL_BLACK
             { 0xff, 0xcc, 0xcc, 0xff }, // COL_RED
             { 0xcc, 0xff, 0xcc, 0xff }, // COL_GREEN
             { 0xff, 0xff, 0xcc, 0xff }, // COL_YELLOW
             { 0xcc, 0xcc, 0xff, 0xff }, // COL_BLUE
             { 0xff, 0xcc, 0xff, 0xff }, // COL_MAGENTA
             { 0xcc, 0xff, 0xff, 0xff }, // COL_CYAN
             { 0xff, 0xff, 0xff, 0xff }, // COL_WHITE
             { 0x00, 0x00, 0x00, 0x00 }, // COL_INVIS
             { 0x00, 0x00, 0x00, 0xff }, // COL_INVERSE
             { 0xff, 0xff, 0xff, 0xff }, // COL_INVERSEBG
        }
   }
};

/* public functions */
EM_INTERN Eina_Bool
em_gui_init(void)
{
   Evas_Object *o, *oo;

   /* allocate our object */
   gui = EM_OBJECT_ALLOC(Em_Gui, EM_GUI_TYPE, _em_gui_cb_free);
   if (!gui) return EINA_FALSE;

   /* create window */
   gui->w_win = elm_win_add(NULL, "emote", ELM_WIN_BASIC);
   elm_win_title_set(gui->w_win, "Emote");
   elm_win_keyboard_mode_set(gui->w_win, ELM_WIN_KEYBOARD_ALPHA);
   evas_object_smart_callback_add(gui->w_win, "delete,request",
                                  _em_gui_cb_win_del, NULL);

   /* create background */
   o = elm_bg_add(gui->w_win);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(gui->w_win, o);
   evas_object_show(o);

   o = elm_box_add(gui->w_win);
   elm_box_homogeneous_set(o, EINA_FALSE);
   elm_win_resize_object_add(gui->w_win, o);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_align_set(o, 0.5, 0.5);
   evas_object_show(o);

   /* create main toolbar */
   gui->w_tb = elm_toolbar_add(gui->w_win);
   elm_toolbar_icon_size_set(gui->w_tb, (8 * elm_config_scale_get()));
   elm_toolbar_align_set(gui->w_tb, 1.0);
   elm_toolbar_shrink_mode_set(gui->w_tb, ELM_TOOLBAR_SHRINK_SCROLL);
   elm_toolbar_item_append(gui->w_tb, "preferences-system", _("Settings"),
                        _em_gui_cb_settings, NULL);
   elm_toolbar_item_append(gui->w_tb, "application-exit", _("Quit"),
                        _em_gui_cb_quit, NULL);
   evas_object_size_hint_weight_set(gui->w_tb, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(gui->w_tb, EVAS_HINT_FILL, 0.0);
   evas_object_show(gui->w_tb);
   elm_box_pack_start(o, gui->w_tb);

   /* create packing box */
   gui->w_box = elm_panes_add(gui->w_win);
   elm_panes_fixed_set(gui->w_box, EINA_FALSE);
   elm_panes_horizontal_set(gui->w_box, EINA_FALSE);
   elm_panes_content_left_size_set(gui->w_box, 0.20);
   evas_object_size_hint_weight_set(gui->w_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gui->w_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(o, gui->w_box);
   evas_object_show(gui->w_box);

   gui->w_chansel_itc = elm_genlist_item_class_new();
   gui->w_chansel_itc->item_style = "default";
   gui->w_chansel_itc->func.text_get = _em_gui_chansel_cb_label_get;
   gui->w_chansel_itc->func.content_get = NULL;
   gui->w_chansel_itc->func.state_get = NULL;
   gui->w_chansel_itc->func.del = NULL;

   /* create channel selector w/ frame */
   gui->w_chansel = elm_genlist_add(gui->w_win);
   elm_genlist_mode_set(gui->w_chansel, ELM_LIST_SCROLL);
   evas_object_size_hint_min_set(gui->w_chansel, 200, MIN_WIN_HEIGHT);
   evas_object_size_hint_weight_set(gui->w_chansel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(gui->w_chansel);

   o = elm_frame_add(gui->w_win);
   elm_object_part_text_set(o, "default", "Channels");
   oo = elm_frame_add(gui->w_win);
   elm_object_style_set(oo, "pad_small");
   elm_object_part_content_set(o, "default", oo);
   elm_object_part_content_set(oo, "default", gui->w_chansel);
   elm_object_part_content_set(gui->w_box, "left", o);
   evas_object_show(oo);
   evas_object_show(o);

   /* set min size of window and show it */
   evas_object_show(gui->w_win);
   evas_object_resize(gui->w_win, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);

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
_em_gui_text_update(Em_Gui_Text *text)
{
  int i;
  unsigned int j, l;
  int w, h;
  int last_text_row;
  int last_tg_row;
  int num_rows;
  Evas_Textgrid_Cell *tc;
  Eina_Inarray *row;
  Em_Gui_Text_Cell *c;

  evas_object_textgrid_size_get(text->tg, &w, &h);

  last_text_row = eina_array_count_get(text->rows) - 1;
  last_tg_row = h;
  while ((last_tg_row >= 0) && (last_text_row >= 0))
  {
    row = (Eina_Inarray *)eina_array_data_get(text->rows, last_text_row);
    l = eina_inarray_count(row);

    num_rows = (l / w);
    if ((l % w) != 0)
      num_rows++;

    last_tg_row -= num_rows;

    j = 0;
    while (j < l)
    {
      if (last_tg_row < 0)
      {
        j += w;
        last_tg_row++;
        continue;
      }

      tc = evas_object_textgrid_cellrow_get(text->tg, last_tg_row);

      for (i = 0; i < w; ++j, ++i)
      {
        if (j < eina_inarray_count(row))
          c = eina_inarray_nth(row, j);
        else
          c = NULL;

        tc[i].codepoint = (c ? c->codepoint : '\0');
        tc[i].fg = (c ? c->fg : 0);
        tc[i].bg = (c ? c->bg : 0);
      }

      evas_object_textgrid_cellrow_set(text->tg, last_tg_row, tc);
      evas_object_textgrid_update_add(text->tg, 0, last_tg_row, w, 1);

      last_tg_row++;
    }

    last_tg_row -= num_rows;

    last_text_row--;
  }
}

EM_INTERN void
em_gui_message_add(const char *server, const char *channel, const char *user, const char *text)
{
   char buf[PATH_MAX];
   Em_Gui_Server *s;
   Em_Gui_Channel *c;
   Eina_Inarray *a;
   int i;
   int len;
   Em_Gui_Text_Cell cell;
   Em_Gui_Text *gt;

   if (user)
     len = snprintf(buf, sizeof(buf), "%s: %s", user, text);
   else
     len = snprintf(buf, sizeof(buf), "*: %s", text);

   // Create inarray containing a row of text.  Wrapping will be
   // dealt with in update function.
   a = eina_inarray_new(sizeof(Em_Gui_Text_Cell), 8);
   if (!a)
     return;

   for (i = 0; i < len; i++)
   {
     cell.codepoint = buf[i];
     cell.fg = 1;
     cell.bg = 0;

     eina_inarray_push(a, &cell);
   }

   s = eina_hash_find(gui->servers, server);
   if (channel)
   {
     if ((c = eina_hash_find(s->channels, channel)))
       gt = &c->text;
     else
       gt = &s->text;
   }
   else
      gt = &s->text;

   eina_array_push(gt->rows, a);
   _em_gui_text_update(gt);
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
   char buf[8192];

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
     return;

   em_gui_message_add(server, channel, NULL, buf);
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
          elm_genlist_item_append(gui->w_chansel, gui->w_chansel_itc, key, NULL, ELM_GENLIST_ITEM_NONE,
                                  _em_gui_chansel_cb_channel_clicked, c);

        elm_genlist_item_bring_in(elm_genlist_last_item_get(gui->w_chansel), ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
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
          elm_genlist_item_append(gui->w_chansel, gui->w_chansel_itc, key, NULL, ELM_GENLIST_ITEM_NONE,
                                  _em_gui_chansel_cb_server_clicked, s);

          elm_genlist_item_bring_in(elm_genlist_last_item_get(gui->w_chansel), ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
     }

  eina_hash_foreach(s->channels, _em_gui_chansel_add_channel, NULL);

  return EINA_TRUE;
}

/** FIXME: Determine method for selecting an item in the genlist based on it's label **/
static void
_em_gui_chansel_update(const char *label)
{
   eina_hash_foreach(gui->servers, _em_gui_chansel_add_server, NULL);
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
   unsigned int i;
   Eina_Inarray *a;
   Eina_Array_Iterator it;

   s = obj;
   evas_object_del(elm_object_parent_widget_get(s->w_entry));
   evas_object_del(s->text.tg);
   EINA_ARRAY_ITER_NEXT(s->text.rows, i, a, it)
     eina_inarray_free(a);
   eina_array_free(s->text.rows);
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
   unsigned int i;
   Eina_Inarray *a;
   Eina_Array_Iterator it;

   c = obj;
   evas_object_del(elm_object_parent_widget_get(c->w_entry));
   evas_object_del(c->text.tg);
   EINA_ARRAY_ITER_NEXT(c->text.rows, i, a, it)
     eina_inarray_free(a);
   eina_array_free(c->text.rows);
   evas_object_del(c->w_box);

   elm_object_item_del(c->w_select);
}

static void
_em_gui_cb_free(void *obj)
{
   Em_Gui *g;

   g = (Em_Gui *)obj;
   /* FIXME: Do we need to free anything? */
}

static void
_em_gui_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   elm_exit();
}

static void
_em_gui_cb_settings(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   Elm_Object_Item *it = elm_toolbar_selected_item_get(gui->w_tb);
   if (it)
     elm_toolbar_item_selected_set(it, EINA_FALSE);
   em_config_show(gui->w_win);
}

static void
_em_gui_cb_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   elm_exit();
}

static char *
_em_gui_chansel_cb_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
  return strdup((char *)data);
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
   char *text;
   Em_Gui_Server *s;

   s = (Em_Gui_Server *)data;

   text = elm_entry_markup_to_utf8(elm_entry_entry_get(obj));
   if ((text[0] == '/') && (text[1] != '/'))
     _em_gui_server_send_message(s->protocol, s->name, s->nick, text);

   // Clear entry
   elm_entry_entry_set((Evas_Object *)obj, NULL);

   free(text);
}

static void
_em_gui_channel_entry_cb_enter(void *data, Evas_Object *obj, void *event __UNUSED__)
{
   char *text;
   Emote_Event *d;
   Em_Gui_Channel *c;
  // char buf[8192];

   c = (Em_Gui_Channel *)data;
   text = elm_entry_markup_to_utf8(elm_entry_entry_get(obj));

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
    elm_entry_entry_set((Evas_Object *)obj, NULL);

    free(text);
}

static void
_em_gui_text_cb_resized(void *data, Evas *e __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   int x, y, w, h;
   int w2, h2;
   Evas_Object *tg;
   Em_Gui_Conversation *c;

   tg = (Evas_Object *)obj;
   c = (Em_Gui_Conversation *)data;

   evas_object_geometry_get(tg, &x, &y, &w, &h);
   evas_object_textgrid_cell_size_get(tg, &w2, &h2);

   w = (w / w2);
   h = (h / h2);

   evas_object_textgrid_size_set(tg, w, h);

   _em_gui_text_update(&c->text);
}

static void
_em_gui_switch(const char *server, const char *channel)
{
   char buf[256];
   Em_Gui_Server *s;

   if (gui->w_block)
     {
        elm_object_part_content_unset(gui->w_box, "right");
        evas_object_hide(gui->w_block);
        gui->w_block = NULL;
     }

   s = eina_hash_find(gui->servers, server);
   if (!channel)
     {
        gui->w_block = s->w_frame;
        _em_gui_chansel_update(server);
        snprintf(buf, sizeof(buf), "Emote - %s", server);
     }
   else
     {
        Em_Gui_Channel *c;

        c = eina_hash_find(s->channels, channel);
        gui->w_block = c->w_frame;
        _em_gui_chansel_update(channel);
        snprintf(buf, sizeof(buf), "Emote - %s : %s", server, channel);
     }

   elm_object_part_content_set(gui->w_box, "right", gui->w_block);

   evas_object_show(gui->w_block);

   elm_win_title_set(gui->w_win, buf);
}

static Em_Gui_Server *
_em_gui_server_create(const char *server, Emote_Protocol *p)
{
   Em_Gui_Server *s;
   Evas_Object *o;
   int i, j, n, l, k;

   /* Allocate new server widgets */
   s = EM_OBJECT_ALLOC(Em_Gui_Server, EM_GUI_SERVER_TYPE, _em_gui_server_free);

   /* create packing box w frame*/
   s->w_frame = elm_frame_add(gui->w_win);
   elm_object_part_text_set(s->w_frame, "default", server);
   evas_object_show(s->w_frame);

   o = elm_frame_add(gui->w_win);
   elm_object_style_set(o, "pad_small");
   elm_object_part_content_set(s->w_frame, "default", o);
   evas_object_show(o);

   s->w_box = elm_box_add(gui->w_win);
   evas_object_size_hint_weight_set(s->w_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->w_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_content_set(o, "default", s->w_box);
   evas_object_show(s->w_box);

   s->text.tg = evas_object_textgrid_add(evas_object_evas_get(gui->w_win));
   evas_object_pass_events_set(s->text.tg, EINA_TRUE);
   evas_object_propagate_events_set(s->text.tg, EINA_FALSE);
   evas_object_size_hint_weight_set(s->text.tg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->text.tg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_textgrid_size_set(s->text.tg, 80, 24);
   evas_object_textgrid_font_set(s->text.tg, "DejaVu Sans Mono:style=Book", 10);
   evas_object_event_callback_add(s->text.tg, EVAS_CALLBACK_RESIZE, _em_gui_text_cb_resized, s);
   evas_object_show(s->text.tg);
   elm_box_pack_start(s->w_box, s->text.tg);

   s->text.rows = eina_array_new(8);

   for (n = 0, l = 0; l < 2; l++) // normal/intense
   {
      for (k = 0; k < 2; k++) // normal/faint
        {
           for (j = 0; j < 2; j++) // normal/bright
             {
                for (i = 0; i < 12; i++, n++) //colors
                  evas_object_textgrid_palette_set
                  (s->text.tg, EVAS_TEXTGRID_PALETTE_STANDARD, n,
                      colors[l][j][i].r / (k + 1),
                      colors[l][j][i].g / (k + 1),
                      colors[l][j][i].b / (k + 1),
                      colors[l][j][i].a / (k + 1));
             }
        }
   }

   o = elm_box_add(gui->w_win);
   elm_box_horizontal_set(o, EINA_TRUE);
   elm_box_homogeneous_set(o, EINA_FALSE);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(s->w_box, o);
   evas_object_show(o);

   s->w_nick = elm_label_add(gui->w_win);
   elm_label_line_wrap_set(s->w_nick, 0);
   evas_object_size_hint_weight_set(s->w_nick, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->w_nick, 0.0, EVAS_HINT_FILL);
   elm_box_pack_end(o, s->w_nick);
   evas_object_show(s->w_nick);

   s->w_entry = elm_entry_add(gui->w_win);
   elm_entry_scrollable_set(s->w_entry, EINA_TRUE);
   elm_entry_single_line_set(s->w_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(s->w_entry, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(s->w_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(s->w_entry, 2, 2, 2, 2);
   evas_object_smart_callback_add(s->w_entry, "activated",
                                  _em_gui_server_entry_cb_enter, s);
   elm_box_pack_end(o, s->w_entry);
   evas_object_show(s->w_entry);

   /* Init channel hash */
   s->channels = eina_hash_string_small_new(_em_gui_channel_del);

   s->w_select =
     elm_genlist_item_append(gui->w_chansel, gui->w_chansel_itc, server, NULL, ELM_GENLIST_ITEM_NONE,
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
   int i, j, n, l, k;

   /* Allocate new server widgets */
   c = EM_OBJECT_ALLOC(Em_Gui_Channel, EM_GUI_CHANNEL_TYPE,
                       _em_gui_channel_free);
   c->server = eina_hash_find(gui->servers, server);

   /* create packing box w frame*/
   c->w_frame = elm_frame_add(gui->w_win);
   elm_object_part_text_set(c->w_frame, "default", channel);
   evas_object_show(c->w_frame);

   o = elm_frame_add(gui->w_win);
   elm_object_style_set(o, "pad_small");
   elm_object_part_content_set(c->w_frame, "default", o);
   evas_object_show(o);

   c->w_box = elm_box_add(gui->w_win);
   evas_object_size_hint_weight_set(c->w_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->w_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(c->w_box, 10, 10, 10, 10);
   elm_object_part_content_set(o, "default", c->w_box);
   evas_object_show(c->w_box);

   c->text.tg = evas_object_textgrid_add(evas_object_evas_get(gui->w_win));
   evas_object_pass_events_set(c->text.tg, EINA_TRUE);
   evas_object_propagate_events_set(c->text.tg, EINA_FALSE);
   evas_object_size_hint_weight_set(c->text.tg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->text.tg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_textgrid_size_set(c->text.tg, 80, 24);
   evas_object_textgrid_font_set(c->text.tg, "DejaVu Sans Mono:style=Book", 10);
   evas_object_event_callback_add(c->text.tg, EVAS_CALLBACK_RESIZE, _em_gui_text_cb_resized, c);
   evas_object_show(c->text.tg);
   elm_box_pack_start(c->w_box, c->text.tg);

   c->text.rows = eina_array_new(8);

   for (n = 0, l = 0; l < 2; l++) // normal/intense
   {
      for (k = 0; k < 2; k++) // normal/faint
        {
           for (j = 0; j < 2; j++) // normal/bright
             {
                for (i = 0; i < 12; i++, n++) //colors
                  evas_object_textgrid_palette_set
                  (c->text.tg, EVAS_TEXTGRID_PALETTE_STANDARD, n,
                      colors[l][j][i].r / (k + 1),
                      colors[l][j][i].g / (k + 1),
                      colors[l][j][i].b / (k + 1),
                      colors[l][j][i].a / (k + 1));
             }
        }
   }

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
   c->w_entry = elm_entry_add(gui->w_win);
   elm_entry_scrollable_set(c->w_entry, EINA_TRUE);
   elm_entry_single_line_set(c->w_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(c->w_entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(c->w_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(c->w_entry, 2, 2, 2, 2);
   elm_box_pack_end(o, c->w_entry);
   evas_object_smart_callback_add(c->w_entry, "activated",
                                  _em_gui_channel_entry_cb_enter, c);
   evas_object_show(c->w_entry);

   c->w_select =
      elm_genlist_item_append(gui->w_chansel, gui->w_chansel_itc, channel, NULL, ELM_GENLIST_ITEM_NONE,
                              _em_gui_chansel_cb_channel_clicked, c);

   c->name = eina_stringshare_add(channel);

   eina_hash_add(c->server->channels, channel, c);

   return c;
}
