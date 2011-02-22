#include "ephoto.h"

static void _entry_free(void *data __UNUSED__, const Ephoto_Entry *entry __UNUSED__);
static Evas_Object *_ephoto_add_image_edje(const char *swallow);
static Evas_Object *_ephoto_add_image(Evas_Object *swallow);
static void _ephoto_flow_done(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _ephoto_flow_back(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_flow_prev(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_flow_next(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_center_image_clicked(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_show_slideshow(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_key_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info);
static void _ephoto_mouse_wheel(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info);

typedef enum _Ephoto_Flow_State
{
   EPHOTO_FLOW_STATE_FLOW,
   EPHOTO_FLOW_STATE_SINGLE
} Ephoto_Flow_State;

typedef struct _Ephoto_Flow_Browser Ephoto_Flow_Browser;
struct _Ephoto_Flow_Browser
{
   Eina_Bool key_down;
   Eina_Bool mouse_wheel;
   Eina_List *current_index;
   Eina_List *items;
   Evas_Object *box;
   Evas_Object *layout;
   Evas_Object *images[5];
   Evas_Object *img_edje[5];
   Evas_Object *toolbar;
   Ephoto_Flow_State efs;
   Ephoto_Entry *entry;
   char *swallows[5];
   int flow_direct;
   struct {
      Elm_Toolbar_Item *go_back;
      Elm_Toolbar_Item *go_prev;
      Elm_Toolbar_Item *go_next;
      Elm_Toolbar_Item *slideshow;
   } action;
};

Ephoto_Flow_Browser *efb;

Evas_Object *
ephoto_flow_browser_add(void)
{
   int i;

   efb = calloc(1, sizeof(Ephoto_Flow_Browser));

   efb->efs = EPHOTO_FLOW_STATE_FLOW;
   efb->swallows[0] = "offscreen_left";
   efb->swallows[1] = "left";
   efb->swallows[2] = "center";
   efb->swallows[3] = "right";
   efb->swallows[4] = "offscreen_right";
   efb->key_down = EINA_FALSE;
   efb->mouse_wheel = EINA_FALSE;

   efb->box = elm_box_add(ephoto->win);
   elm_box_horizontal_set(efb->box, EINA_FALSE);
   elm_box_homogenous_set(efb->box, EINA_FALSE);
   evas_object_size_hint_weight_set
     (efb->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(ephoto->win, efb->box);
   evas_object_event_callback_add
     (efb->box, EVAS_CALLBACK_MOUSE_WHEEL, _ephoto_mouse_wheel, NULL);
   evas_object_event_callback_add
     (efb->box, EVAS_CALLBACK_KEY_DOWN, _ephoto_key_down, NULL);
   evas_object_show(efb->box);

   efb->toolbar = elm_toolbar_add(efb->box);
   elm_toolbar_homogenous_set(efb->toolbar, EINA_TRUE);
   elm_toolbar_mode_shrink_set(efb->toolbar, ELM_TOOLBAR_SHRINK_MENU);
   elm_toolbar_menu_parent_set(efb->toolbar, efb->box);
   evas_object_size_hint_weight_set(efb->toolbar, 0.0, 0.0);
   evas_object_size_hint_align_set(efb->toolbar, EVAS_HINT_FILL, 0.0);

    efb->action.go_back = elm_toolbar_item_append
     (efb->toolbar, "edit-undo", "Back", _ephoto_flow_back, NULL);
   elm_toolbar_item_priority_set(efb->action.go_back, 0);
   efb->action.go_prev = elm_toolbar_item_append
     (efb->toolbar, "go-previous", "Previous", _ephoto_flow_prev, NULL);
   elm_toolbar_item_priority_set(efb->action.go_prev, 1);
   efb->action.go_next = elm_toolbar_item_append
     (efb->toolbar, "go-next", "Next", _ephoto_flow_next, NULL);
   elm_toolbar_item_priority_set(efb->action.go_next, 2);
   efb->action.slideshow = elm_toolbar_item_append
     (efb->toolbar, "media-playback-start", "Slideshow", _ephoto_show_slideshow, NULL);
   elm_toolbar_item_priority_set(efb->action.slideshow, 3);

   elm_toolbar_icon_size_set(efb->toolbar, 32);
   elm_box_pack_end(efb->box, efb->toolbar);
   evas_object_show(efb->toolbar);

   efb->layout = elm_layout_add(ephoto->win);
   elm_layout_file_set
     (efb->layout, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "flow");
   evas_object_size_hint_weight_set
     (efb->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set
     (efb->layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(ephoto->win, efb->layout);
   elm_box_pack_end(efb->box, efb->layout);
   evas_object_show(efb->layout);

   edje_object_signal_callback_add
     (elm_layout_edje_get(efb->layout), "done", "ephoto", _ephoto_flow_done, NULL);

   for (i = 0; i < 5; i++)
     {
        efb->img_edje[i] = _ephoto_add_image_edje(efb->swallows[i]);
        efb->images[i] = _ephoto_add_image(efb->img_edje[i]);
        if (i == 2)
          evas_object_event_callback_add
            (efb->img_edje[2], EVAS_CALLBACK_MOUSE_DOWN, _ephoto_center_image_clicked, NULL);
     }

   return efb->box;
}

void
ephoto_flow_browser_image_set(void)
{
   Eina_List *prevv, *prev, *next, *nextt; 
   Ephoto_Entry *pp, *p, *n, *nn, *c;
   int i;

   prev = eina_list_prev(efb->current_index);
   if (!eina_list_data_get(prev))
     prev = eina_list_last(efb->items);
   prevv = eina_list_prev(prev);
   if (!eina_list_data_get(prevv))
     prevv = eina_list_last(efb->items);
   next = eina_list_next(efb->current_index);
   if (!eina_list_data_get(next))
     next = eina_list_nth_list(efb->items, 0);
   nextt = eina_list_next(next);
   if (!eina_list_data_get(nextt))
     nextt = eina_list_nth_list(efb->items, 0);

   pp = eina_list_data_get(prevv);
   p = eina_list_data_get(prev);
   c = eina_list_data_get(efb->current_index);
   n = eina_list_data_get(next);
   nn = eina_list_data_get(nextt);

   for (i = 0; i < 5; i++)
     {
        elm_layout_content_unset(efb->layout, efb->swallows[i]);
        elm_layout_content_unset(efb->img_edje[i], "image");
     }

   elm_thumb_file_set(efb->images[0], pp->path, NULL);
   elm_layout_content_set(efb->img_edje[0], "image", efb->images[0]);
   elm_layout_content_set(efb->layout, "offscreen_left", efb->img_edje[0]);
   elm_thumb_file_set(efb->images[1], p->path, NULL);
   elm_layout_content_set(efb->img_edje[1], "image", efb->images[1]);
   elm_layout_content_set(efb->layout, "left", efb->img_edje[1]);
   elm_thumb_file_set(efb->images[2], c->path, NULL);
   elm_layout_content_set(efb->img_edje[2], "image", efb->images[2]);
   elm_layout_content_set(efb->layout, "center", efb->img_edje[2]);
   elm_thumb_file_set(efb->images[3], n->path, NULL);
   elm_layout_content_set(efb->img_edje[3], "image", efb->images[3]);
   elm_layout_content_set(efb->layout, "right", efb->img_edje[3]);
   elm_thumb_file_set(efb->images[4], nn->path, NULL);
   elm_layout_content_set(efb->img_edje[4], "image", efb->images[4]);
   elm_layout_content_set(efb->layout, "offscreen_right", efb->img_edje[4]);

   elm_object_focus(efb->box);
}

void
ephoto_flow_browser_del(void)
{
   int i;

   for (i = 0; i < 5; i++)
     evas_object_del(efb->images[i]);
   eina_list_free(efb->items);
   evas_object_del(efb->layout);
   evas_object_del(efb->toolbar);
   evas_object_del(efb->box);
   free(efb);
}

void
ephoto_flow_browser_entry_set(Ephoto_Entry *entry)
{
   Eina_Bool same_file = EINA_FALSE;
   Eina_List *l;
   Ephoto_Entry *itr;

   elm_object_focus(efb->layout);
   if (!evas_object_key_grab(efb->box, "Escape", 0, 0, 1))
     printf("Couldn't grab Escape key\n");
   if (!evas_object_key_grab(efb->box, "Left", 0, 0, 1))
     printf("Couldn't grab Left key\n");
   if (!evas_object_key_grab(efb->box, "Right", 0, 0, 1))
     printf("Couldn't grab Right key\n");
   if (!evas_object_key_grab(efb->box, "BackSpace", 0, 0, 1))
     printf("Couldn't grab BackSpace key\n");
   if (!evas_object_key_grab(efb->box, "space", 0, 0, 1))
     printf("Couldn't grab space key\n");

   if (efb->entry)
     {
        ephoto_entry_free_listener_del(efb->entry, _entry_free, NULL);
        if (entry && entry->path == efb->entry->path)
          same_file = EINA_TRUE;
     }

   efb->entry = entry;

   if (entry)
     ephoto_entry_free_listener_add(entry, _entry_free, NULL);
   if (!efb->entry || same_file)
     {
        elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_TRUE);
        elm_toolbar_item_disabled_set(efb->action.go_next, EINA_TRUE);
        elm_toolbar_item_disabled_set(efb->action.slideshow, EINA_TRUE);
        return;
     }
   else
     {
        elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_FALSE);
        elm_toolbar_item_disabled_set(efb->action.go_next, EINA_FALSE);
        elm_toolbar_item_disabled_set(efb->action.slideshow, EINA_FALSE);
     }
   eina_list_free(efb->items);
   efb->items = NULL;
   EINA_LIST_FOREACH(ephoto->entries, l, itr)
    {
       efb->items = eina_list_append(efb->items, itr);
       if (itr == entry) efb->current_index = eina_list_last(efb->items);
    }
   ephoto_flow_browser_image_set();
}

static void
_entry_free(void *data __UNUSED__, const Ephoto_Entry *entry __UNUSED__)
{
   efb->entry = NULL;
}

static Evas_Object *
_ephoto_add_image_edje(const char *swallow)
{
   Evas_Object *o;

   o = elm_layout_add(efb->box);
   elm_layout_file_set
     (o, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "flow_image");
   elm_layout_content_set(efb->layout, swallow, o);
   evas_object_show(o);

   return o;
}

static Evas_Object *
_ephoto_add_image(Evas_Object *swallow)
{
   Evas_Object *o;

   o = elm_thumb_add(efb->box);
   elm_object_style_set(o, "noframe");
   elm_layout_content_set(swallow, "image", o);
   evas_object_show(o);

   return o;
}

static void 
_ephoto_flow_done(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Evas_Object *edje;

   edje = elm_layout_edje_get(efb->layout);
   edje_object_freeze(edje);

   if (efb->flow_direct == 0)
     {
        efb->current_index = eina_list_prev(efb->current_index);
        if (!eina_list_data_get(efb->current_index))
          efb->current_index = eina_list_last(efb->items);
        ephoto_flow_browser_image_set();
     }
   else
     {
        efb->current_index = eina_list_next(efb->current_index);
        if (!eina_list_data_get(efb->current_index))
          efb->current_index = eina_list_nth_list(efb->items, 0);
        ephoto_flow_browser_image_set();
     }
   
   edje_object_signal_emit(edje, "reset", "ephoto");
   edje_object_thaw(edje);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_FALSE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_FALSE);
   efb->mouse_wheel = EINA_FALSE;
   efb->key_down = EINA_FALSE;
}

static void
_ephoto_flow_back(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_toolbar_item_selected_set(efb->action.go_back, EINA_FALSE);

   evas_object_key_ungrab(efb->box, "Escape", 0, 0);
   evas_object_key_ungrab(efb->box, "Left", 0, 0);
   evas_object_key_ungrab(efb->box, "Right", 0, 0);
   evas_object_key_ungrab(efb->box, "BackSpace", 0, 0);
   evas_object_key_ungrab(efb->box, "space", 0, 0);

   elm_object_unfocus(efb->layout);
   ephoto_thumb_browser_show(efb->entry);
}

static void 
_ephoto_flow_prev(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *edje;

   elm_toolbar_item_selected_set(efb->action.go_prev, EINA_FALSE);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_TRUE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_TRUE);
   efb->mouse_wheel = EINA_TRUE;
   efb->key_down = EINA_TRUE;

   edje = elm_layout_edje_get(efb->layout);

   if (efb->efs == EPHOTO_FLOW_STATE_SINGLE)
     {
        edje_object_signal_emit(edje, "full_reset", "ephoto");
        efb->efs = EPHOTO_FLOW_STATE_FLOW;
     }
   efb->flow_direct = 0;
   edje_object_signal_emit(edje, "right", "ephoto");
}

static void 
_ephoto_flow_next(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *edje;

   elm_toolbar_item_selected_set(efb->action.go_next, EINA_FALSE);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_TRUE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_TRUE);
   efb->mouse_wheel = EINA_TRUE;
   efb->key_down = EINA_TRUE;

   edje = elm_layout_edje_get(efb->layout);

   if (efb->efs == EPHOTO_FLOW_STATE_SINGLE)
     {
        edje_object_signal_emit(edje, "full_reset", "ephoto");
        efb->efs = EPHOTO_FLOW_STATE_FLOW;
     }
   efb->flow_direct = 1;
   edje_object_signal_emit(edje, "left", "ephoto");
}

static void 
_ephoto_center_image_clicked(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *edje;

   edje = elm_layout_edje_get(efb->layout);

   if (efb->efs == EPHOTO_FLOW_STATE_FLOW)
     {
        edje_object_signal_emit(edje, "full", "ephoto");
        efb->efs = EPHOTO_FLOW_STATE_SINGLE;
     }
   else
     {
        edje_object_signal_emit(edje, "full_reset", "ephoto");
        efb->efs = EPHOTO_FLOW_STATE_FLOW;
     }
}

static void
_ephoto_show_slideshow(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_toolbar_item_selected_set(efb->action.slideshow, EINA_FALSE);

   ephoto_slideshow_show(efb->entry);
}

static void
_ephoto_key_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
   const char *k = ev->keyname;
   
   if (efb->key_down)
     return;
   efb->key_down = EINA_TRUE;

   if (!strcmp(k, "Escape"))
     _ephoto_flow_back(NULL, NULL, NULL);
   if (!strcmp(k, "Left") || !strcmp(k, "BackSpace"))
     _ephoto_flow_prev(NULL, NULL, NULL);
   if (!strcmp(k, "Right") || !strcmp(k, "space"))
     _ephoto_flow_next(NULL, NULL, NULL);
}

static void
_ephoto_mouse_wheel(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Event_Mouse_Wheel *ev = event_info;

   if (efb->mouse_wheel)
     return;
   efb->mouse_wheel = EINA_TRUE;

   if (ev->z > 0) 
     _ephoto_flow_next(NULL, NULL, NULL);
   else
     _ephoto_flow_prev(NULL, NULL, NULL);
}
