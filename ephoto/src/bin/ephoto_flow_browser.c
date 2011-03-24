#include "ephoto.h"

static Elm_Genlist_Item_Class egli;

typedef struct _Ephoto_Flow_Browser Ephoto_Flow_Browser;
struct _Ephoto_Flow_Browser
{
   Eina_Bool key_down;
   Eina_Bool mouse_wheel;
   Eina_List *current_index;
   Eina_List *items;
   Elm_Genlist_Item *egi;
   Evas_Object *box;
   Evas_Object *edje;
   Evas_Object *layout;
   Evas_Object *list;
   Evas_Object *main_layout;
   Evas_Object *images[5];
   Evas_Object *img_edje[5];
   Evas_Object *toolbar;
   Ephoto_Entry *entry;
   Ephoto *ephoto;
   char *swallows[5];
   int flow_direct;
   int selected_handled;
   struct {
      Elm_Toolbar_Item *go_back;
      Elm_Toolbar_Item *go_prev;
      Elm_Toolbar_Item *go_next;
      Elm_Toolbar_Item *slideshow;
   } action;
};


static void
_entry_free(void *data, const Ephoto_Entry *entry __UNUSED__)
{
   Ephoto_Flow_Browser *efb = data;

   efb->entry = NULL;
}

static Evas_Object *
_add_image_edje(const char *swallow, Evas_Object *parent)
{
   Evas_Object *o;

   o = elm_layout_add(parent);
   elm_layout_file_set
     (o, THEME_FILE, "flow_image");
   elm_layout_content_set(parent, swallow, o);
   evas_object_show(o);

   return o;
}

static Evas_Object *
_add_image(Evas_Object *swallow, Evas_Object *parent)
{
   Evas_Object *o;

   o = elm_thumb_add(parent);
   elm_object_style_set(o, "noframe");
   elm_layout_content_set(swallow, "image", o);
   evas_object_show(o);

   return o;
}

static void
_flow_browser_image_set(Ephoto_Flow_Browser *efb)
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

   elm_object_focus(efb->main_layout);
}

static void 
_flow_done(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Ephoto_Flow_Browser *efb = data;
   Evas_Object *edje;

   edje = elm_layout_edje_get(efb->layout);
   edje_object_freeze(edje);

   if (efb->flow_direct == 0)
     {
        efb->current_index = eina_list_prev(efb->current_index);
        if (!eina_list_data_get(efb->current_index))
          efb->current_index = eina_list_last(efb->items);
        _flow_browser_image_set(efb);
     }
   else
     {
        efb->current_index = eina_list_next(efb->current_index);
        if (!eina_list_data_get(efb->current_index))
          efb->current_index = eina_list_nth_list(efb->items, 0);
        _flow_browser_image_set(efb);
     }
   
   edje_object_signal_emit(edje, "reset", "ephoto");
   edje_object_thaw(edje);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_FALSE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_FALSE);
   elm_genlist_no_select_mode_set(efb->list, EINA_FALSE);
   efb->mouse_wheel = EINA_FALSE;
   efb->key_down = EINA_FALSE;
   efb->selected_handled = 0;
}

static void
_flow_back(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Flow_Browser *efb = data;

   elm_toolbar_item_selected_set(efb->action.go_back, EINA_FALSE);

   evas_object_key_ungrab(efb->main_layout, "Escape", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "Left", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "Right", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "BackSpace", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "space", 0, 0);

   efb->entry = eina_list_data_get(efb->current_index);

   elm_object_unfocus(efb->main_layout);
   evas_object_smart_callback_call(efb->main_layout, "back", efb->entry);
}

static void 
_flow_prev(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Elm_Genlist_Item *egi;
   Ephoto_Flow_Browser *efb = data;
   Evas_Object *edje;

   elm_toolbar_item_selected_set(efb->action.go_prev, EINA_FALSE);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_TRUE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_TRUE);
   efb->mouse_wheel = EINA_TRUE;
   efb->key_down = EINA_TRUE;

   edje = elm_layout_edje_get(efb->layout);

   egi = elm_genlist_item_prev_get(efb->egi);
   if (!egi)
     egi = elm_genlist_last_item_get(efb->list);
   elm_genlist_item_bring_in(egi);
   efb->egi = egi;
   efb->selected_handled = 1;
   elm_genlist_item_selected_set(egi, EINA_TRUE);
   elm_genlist_no_select_mode_set(efb->list, EINA_TRUE);

   efb->flow_direct = 0;
   edje_object_signal_emit(edje, "right", "ephoto");
}

static void 
_flow_next(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Elm_Genlist_Item *egi;
   Ephoto_Flow_Browser *efb = data;
   Evas_Object *edje;

   elm_toolbar_item_selected_set(efb->action.go_next, EINA_FALSE);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_TRUE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_TRUE);
   efb->mouse_wheel = EINA_TRUE;
   efb->key_down = EINA_TRUE;

   edje = elm_layout_edje_get(efb->layout);

   egi = elm_genlist_item_next_get(efb->egi);
   if (!egi)
     egi = elm_genlist_first_item_get(efb->list);
   elm_genlist_item_bring_in(egi);
   efb->egi = egi;
   efb->selected_handled = 1;
   elm_genlist_item_selected_set(egi, EINA_TRUE);
   elm_genlist_no_select_mode_set(efb->list, EINA_TRUE);

   efb->flow_direct = 1;
   edje_object_signal_emit(edje, "left", "ephoto");
}

static void 
_center_image_clicked(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Flow_Browser *efb = data;
   
   evas_object_key_ungrab(efb->main_layout, "Escape", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "Left", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "Right", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "BackSpace", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "space", 0, 0);

   efb->entry = eina_list_data_get(efb->current_index);

   elm_object_unfocus(efb->main_layout);
   evas_object_smart_callback_call(efb->main_layout, "single", efb->entry);
}

static void
_flow_single(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Flow_Browser *efb = data;

   evas_object_key_ungrab(efb->main_layout, "Escape", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "Left", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "Right", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "BackSpace", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "space", 0, 0);

   efb->entry = eina_list_data_get(efb->current_index);

   elm_object_unfocus(efb->main_layout);
   evas_object_smart_callback_call(efb->main_layout, "single", efb->entry);
}

static void
_show_slideshow(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Flow_Browser *efb = data;

   elm_toolbar_item_selected_set(efb->action.slideshow, EINA_FALSE);

   evas_object_key_ungrab(efb->main_layout, "Escape", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "Left", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "Right", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "BackSpace", 0, 0);
   evas_object_key_ungrab(efb->main_layout, "space", 0, 0);

   elm_object_unfocus(efb->main_layout);
   evas_object_smart_callback_call(efb->main_layout, "slideshow", efb->entry);
}

static void
_key_down(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ephoto_Flow_Browser *efb = data;
   Evas_Event_Key_Down *ev = event_info;
   const char *k = ev->keyname;
   
   if (efb->key_down)
     return;
   efb->key_down = EINA_TRUE;

   if (!strcmp(k, "Escape"))
     _flow_back(efb, NULL, NULL);
   if (!strcmp(k, "Left") || !strcmp(k, "BackSpace"))
     _flow_prev(efb, NULL, NULL);
   if (!strcmp(k, "Right") || !strcmp(k, "space"))
     _flow_next(efb, NULL, NULL);
}

static void
_mouse_wheel(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ephoto_Flow_Browser *efb = data;
   Evas_Event_Mouse_Wheel *ev = event_info;

   if (efb->mouse_wheel)
     return;
   efb->mouse_wheel = EINA_TRUE;

   if (ev->z > 0) 
     _flow_next(efb, NULL, NULL);
   else
     _flow_prev(efb, NULL, NULL);
}

static void
_layout_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Flow_Browser *efb = data;

   if (efb->entry)
     ephoto_entry_free_listener_del(efb->entry, _entry_free, efb);
 
   free(efb);
}

static Elm_Toolbar_Item *
_toolbar_item_add(Ephoto_Flow_Browser *efb, const char *icon, const char *label, int priority, Evas_Smart_Cb cb)
{
   Elm_Toolbar_Item *item = elm_toolbar_item_append(efb->toolbar, icon, label,
                                                    cb, efb);
   elm_toolbar_item_priority_set(item, priority);
   return item;
}

static char *
_gl_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Ephoto_Entry *en = data;
   char *label = (char *)en->basename;
   return strdup(label);
}

static Evas_Object *
_gl_icon_get(void *data __UNUSED__, Evas_Object *obj, const char *part)
{
   Evas_Object *o = NULL;

   if (!strcmp(part, "elm.swallow.end"))
     return o;

   o = elm_icon_add(obj);
   elm_icon_standard_set(o, "image");
   evas_object_size_hint_aspect_set(o, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   
   return o;
}

static Eina_Bool
_gl_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

static void
_gl_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{
}

static int
_entry_cmp(const void *pa, const void *pb)
{
   const Ephoto_Entry *a = pa, *b = pb;
   int ret, s;

   s = strcmp(a->basename, b->basename);
   if (s > 0)
     ret = 1;
   else if (s < 0)
     ret = -1;
   else
     ret = 0;
   return ret;
}

static void
_gl_sel(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *egi, *egip, *egin;
   Ephoto_Entry *en;
   Ephoto_Flow_Browser *efb = data;

   egi = event_info;
   if (!efb->egi || efb->selected_handled)
     {
        efb->selected_handled = 0;
        return;
     }

   egin = elm_genlist_item_next_get(efb->egi);
   if (!egin)
     egin = elm_genlist_first_item_get(efb->list);
   egip = elm_genlist_item_prev_get(efb->egi);
   if (!egip)
     egip = elm_genlist_last_item_get(efb->list);

   if (egi == egin)
     _flow_next(efb, NULL, NULL);
   else if (egi == egip)
     _flow_prev(efb, NULL, NULL);
   else
     {
        en = elm_genlist_item_data_get(egi);
        efb->current_index = eina_list_search_sorted_list
          (efb->items, _entry_cmp, en);
        efb->egi = egi;
        _flow_browser_image_set(efb);
     }
}

Evas_Object *
ephoto_flow_browser_add(Ephoto *e, Evas_Object *parent)
{
   Ephoto_Flow_Browser *efb;
   int i;

   efb = calloc(1, sizeof(Ephoto_Flow_Browser));

   efb->swallows[0] = "offscreen_left";
   efb->swallows[1] = "left";
   efb->swallows[2] = "center";
   efb->swallows[3] = "right";
   efb->swallows[4] = "offscreen_right";
   efb->key_down = EINA_FALSE;
   efb->mouse_wheel = EINA_FALSE;
   efb->ephoto = e;
   efb->selected_handled = 0;

   egli.item_style = "default";
   egli.func.label_get = _gl_label_get;
   egli.func.icon_get = _gl_icon_get;
   egli.func.state_get = _gl_state_get;
   egli.func.del = _gl_del;

   efb->main_layout = elm_layout_add(parent);
   efb->edje = elm_layout_edje_get(efb->main_layout);
   evas_object_event_callback_add
     (efb->main_layout, EVAS_CALLBACK_DEL, _layout_del, efb);
   evas_object_event_callback_add
     (efb->main_layout, EVAS_CALLBACK_KEY_DOWN, _key_down, efb);
   evas_object_data_set(efb->main_layout, "flow_browser", efb);

   if (efb->ephoto->config->autohide_toolbar)
     {
        edje_object_signal_callback_add
          (efb->edje, "mouse,clicked,1", "toolbar_event",
                   ephoto_auto_hide_toolbar, efb->ephoto);
        if (!elm_layout_file_set
             (efb->main_layout, THEME_FILE, "ephoto/layout/simple/autohide"))
          {
              ERR("could not load style 'ephoto/layout/simple/autohide' from theme");
              goto error;
          }
     }
   else
     {
        if (!elm_layout_file_set
             (efb->main_layout, THEME_FILE, "ephoto/layout/simple"))
          {
              ERR("could not load style 'ephoto/layout/simple' from theme");
              goto error;
          }
     }

   efb->toolbar = elm_toolbar_add(efb->main_layout);
   elm_toolbar_homogenous_set(efb->toolbar, EINA_TRUE);
   elm_toolbar_mode_shrink_set(efb->toolbar, ELM_TOOLBAR_SHRINK_MENU);
   elm_toolbar_menu_parent_set(efb->toolbar, parent);
   evas_object_size_hint_weight_set(efb->toolbar, 0.0, 0.0);
   evas_object_size_hint_align_set(efb->toolbar, EVAS_HINT_FILL, 0.0);

    efb->action.go_back = _toolbar_item_add
     (efb, "edit-undo", "Back", 120, _flow_back);
   efb->action.go_prev = _toolbar_item_add
     (efb, "go-previous", "Previous", 100, _flow_prev);
   efb->action.go_next = _toolbar_item_add
     (efb, "go-next", "Next", 80, _flow_next);
   efb->action.slideshow = _toolbar_item_add
     (efb, "media-playback-start", "Slideshow", 70, _show_slideshow);

   elm_layout_content_set
     (efb->main_layout, "ephoto.toolbar.swallow", efb->toolbar);
   evas_object_show(efb->toolbar);

   efb->box = elm_panes_add(efb->main_layout);
   elm_panes_horizontal_set(efb->box, EINA_TRUE);
   evas_object_size_hint_weight_set
     (efb->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(efb->box);

   efb->layout = elm_layout_add(efb->main_layout);
   elm_layout_file_set
     (efb->layout, THEME_FILE, "flow");
   evas_object_size_hint_weight_set
     (efb->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set
     (efb->layout, EVAS_HINT_FILL, EVAS_HINT_EXPAND);
   elm_panes_content_left_set(efb->box, efb->layout);
   evas_object_show(efb->layout);
   evas_object_event_callback_add
     (efb->layout, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, efb);
   edje_object_signal_callback_add
     (elm_layout_edje_get(efb->layout), "done", "ephoto", _flow_done, efb);

   efb->list = elm_genlist_add(efb->main_layout);
   elm_genlist_bounce_set(efb->list, EINA_FALSE, EINA_TRUE);
   elm_genlist_height_for_width_mode_set(efb->list, EINA_TRUE);
   elm_genlist_compress_mode_set(efb->list, EINA_TRUE);
   elm_genlist_longpress_timeout_set(efb->list, 0.5);
   evas_object_size_hint_weight_set(efb->list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(efb->list, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_panes_content_right_set(efb->box, efb->list);
   evas_object_show(efb->list);

   elm_panes_content_left_size_set(efb->box, 0.4);

   elm_layout_content_set
     (efb->main_layout, "ephoto.content.swallow", efb->box);
   

   for (i = 0; i < 5; i++)
     {
        efb->img_edje[i] = _add_image_edje(efb->swallows[i], efb->layout);
        efb->images[i] = _add_image(efb->img_edje[i], efb->layout);
        if (i == 2)
          evas_object_event_callback_add
            (efb->img_edje[2], EVAS_CALLBACK_MOUSE_DOWN, _center_image_clicked, efb);
     }

   elm_object_focus_custom_chain_append(efb->main_layout, efb->layout, NULL);   

   return efb->main_layout;

 error:
   evas_object_del(efb->main_layout);
   return NULL;
}

void
ephoto_flow_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry)
{
   Ephoto_Flow_Browser *efb = evas_object_data_get(obj, "flow_browser");
   Eina_Bool same_file = EINA_FALSE;
   Eina_List *l;
   Ephoto_Entry *itr;

   if (!evas_object_key_grab(efb->main_layout, "Escape", 0, 0, 1))
     printf("Couldn't grab Escape key\n");
   if (!evas_object_key_grab(efb->main_layout, "Left", 0, 0, 1))
     printf("Couldn't grab Left key\n");
   if (!evas_object_key_grab(efb->main_layout, "Right", 0, 0, 1))
     printf("Couldn't grab Right key\n");
   if (!evas_object_key_grab(efb->main_layout, "BackSpace", 0, 0, 1))
     printf("Couldn't grab BackSpace key\n");
   if (!evas_object_key_grab(efb->main_layout, "space", 0, 0, 1))
     printf("Couldn't grab space key\n");

   if (efb->entry)
     {
        ephoto_entry_free_listener_del(efb->entry, _entry_free, efb);
        if (entry && entry->path == efb->entry->path)
          same_file = EINA_TRUE;
     }

   efb->entry = entry;

   if (entry)
     ephoto_entry_free_listener_add(entry, _entry_free, efb);
   if (!efb->entry)
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
   elm_genlist_clear(efb->list);
   eina_list_free(efb->items);
   efb->items = NULL;
   EINA_LIST_FOREACH(efb->ephoto->entries, l, itr)
    {
       Elm_Genlist_Item *egi;

       egi = elm_genlist_item_append(efb->list, &egli,
                               (void *)itr,
                               NULL, ELM_GENLIST_ITEM_NONE,
                               _gl_sel, efb);
       evas_object_smart_callback_add
         (efb->list, "clicked", _flow_single, efb);
       efb->items = eina_list_append(efb->items, itr);
       if (itr == entry) 
         {
            efb->current_index = eina_list_last(efb->items);
            elm_genlist_item_bring_in(egi);
            elm_genlist_item_selected_set(egi, EINA_TRUE);
            efb->egi = egi;
         }
    }
   _flow_browser_image_set(efb);
}

