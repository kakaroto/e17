#include "em_global.h"

#define MIN_WIN_WIDTH 200
#define MIN_WIN_HEIGHT 200

/* local function prototypes */
static void _em_gui_cb_free(void);
static void _em_gui_cb_win_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_cb_protocol_selected(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_cb_protocols(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_cb_settings(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _em_gui_cb_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);

/* local variables */
static Em_Gui *gui;

/* public functions */
EM_INTERN int
em_gui_init(void)
{
   Evas_Object *o, *box;

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
   box = elm_box_add(gui->win);
   elm_box_horizontal_set(box, EINA_FALSE);
   elm_box_homogenous_set(box, EINA_FALSE);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(gui->win, box);
   evas_object_show(box);

   /* create connected protocols list */
   /* TODO FIX ME: If the buddy lists for aim clients
    * is to be housed here, it probably needs to be a
    * genlist tree! */
   gui->o_proto = elm_list_add(gui->win);
   evas_object_size_hint_weight_set(gui->o_proto, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(gui->o_proto, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, gui->o_proto);
   evas_object_show(gui->o_proto); 
   elm_list_go(gui->o_proto);

   /* create main toolbar */
   gui->o_tb = elm_toolbar_add(gui->win);
   elm_toolbar_icon_size_set(gui->o_tb, 16);
   elm_toolbar_align_set(gui->o_tb, 0.0);
   elm_toolbar_scrollable_set(gui->o_tb, EINA_FALSE);
   evas_object_size_hint_weight_set(gui->o_tb, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(gui->o_tb, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(box, gui->o_tb);
   evas_object_show(gui->o_tb);

   o = em_util_icon_add(gui->win, "preferences-system-network");
   elm_toolbar_item_add(gui->o_tb, o, _("Protocols"), _em_gui_cb_protocols, NULL);
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
_em_gui_cb_protocol_selected(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   const char *name;
   const Eina_List *protocols, *n;
   const Elm_List_Item *selected;
   Evas_Object *icon, *list, *win;

   list = data;
   protocols = elm_list_selected_items_get(list);

   if (!protocols)
     return;

   EINA_LIST_FOREACH(protocols, n, selected)
     {
        name = elm_list_item_label_get(selected);
        icon = em_util_icon_add(gui->win, "applications-internet");
        evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
        elm_list_item_append(gui->o_proto, name, icon, NULL, NULL, NULL);
        elm_list_go(gui->o_proto);
     }
     win = elm_object_top_widget_get(obj);
     evas_object_del(win);
}

static void
_em_gui_cb_protocols(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   const char *name;
   Eina_List *protocols, *n;
   Evas_Object *win, *box, *list, *o, *icon;

   elm_toolbar_item_unselect_all(gui->o_tb);

   win = elm_win_add(NULL, "emote", ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(win, "Emote Protocols");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_show(win);

   o = elm_bg_add(win);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_min_set(o, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   elm_win_resize_object_add(win, o);
   evas_object_show(o);

   box = elm_box_add(win);
   elm_box_horizontal_set(box, EINA_FALSE);
   elm_box_homogenous_set(box, EINA_FALSE);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   list = elm_list_add(win);
   elm_list_multi_select_set(list, EINA_TRUE);
   evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, list);
   evas_object_show(list);
   elm_list_go(list);

   protocols = emote_protocol_list();
   EINA_LIST_FOREACH(protocols, n, name)
     {
        icon = em_util_icon_add(win, "applications-internet");
        evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
        elm_list_item_append(list, name, icon, NULL, NULL, NULL);
        elm_list_go(list);
     }

   icon = em_util_icon_add(win, "preferences-system-network");
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   o = elm_button_add(win);
   elm_button_label_set(o, "Connect");
   elm_button_icon_set(o, icon);
   elm_box_pack_end(box, o);
   evas_object_smart_callback_add(o, "clicked", _em_gui_cb_protocol_selected, list);
   evas_object_show(o);
   evas_object_show(icon);

   evas_object_size_hint_min_set(win, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   evas_object_resize(win, MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
   evas_object_show(win);
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

