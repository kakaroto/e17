#include "ephoto.h"

static Evas_Object *_ephoto_add_image_edje(const char *swallow);
static Evas_Object *_ephoto_add_image(Evas_Object *swallow);
static void _ephoto_flow_done(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _ephoto_flow_back(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_flow_prev(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_flow_next(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_center_image_clicked(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__);

typedef enum _Ephoto_Flow_State
{
   EPHOTO_FLOW_STATE_FLOW,
   EPHOTO_FLOW_STATE_SINGLE
} Ephoto_Flow_State;

typedef struct _Ephoto_Flow_Browser Ephoto_Flow_Browser;
struct _Ephoto_Flow_Browser
{
   Evas_Object *box;
   Evas_Object *layout;
   Evas_Object *images[5];
   Evas_Object *img_edje[5];
   Evas_Object *toolbar;
   Ephoto_Flow_State efs;
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

   efb->box = elm_box_add(ephoto->win);
   elm_box_horizontal_set(efb->box, EINA_FALSE);
   elm_box_homogenous_set(efb->box, EINA_FALSE);
   evas_object_size_hint_weight_set
     (efb->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(ephoto->win, efb->box);
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
     (efb->toolbar, "media-playback-start", "Slideshow", NULL, NULL);
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
   int i;

   prev = eina_list_prev(ephoto->current_index);
   if (!eina_list_data_get(prev))
     prev = eina_list_last(ephoto->images);
   prevv = eina_list_prev(prev);
   if (!eina_list_data_get(prevv))
     prevv = eina_list_last(ephoto->images);
   next = eina_list_next(ephoto->current_index);
   if (!eina_list_data_get(next))
     next = eina_list_nth_list(ephoto->images, 0);
   nextt = eina_list_next(next);
   if (!eina_list_data_get(nextt))
     nextt = eina_list_nth_list(ephoto->images, 0);

   for (i = 0; i < 5; i++)
     {
        elm_layout_content_unset(efb->layout, efb->swallows[i]);
        elm_layout_content_unset(efb->img_edje[i], "image");
     }

   elm_thumb_file_set(efb->images[0], eina_list_data_get(prevv), NULL);
   elm_layout_content_set(efb->img_edje[0], "image", efb->images[0]);
   elm_layout_content_set(efb->layout, "offscreen_left", efb->img_edje[0]);
   elm_thumb_file_set(efb->images[1], eina_list_data_get(prev), NULL);
   elm_layout_content_set(efb->img_edje[1], "image", efb->images[1]);
   elm_layout_content_set(efb->layout, "left", efb->img_edje[1]);
   elm_thumb_file_set(efb->images[2], eina_list_data_get(ephoto->current_index), NULL);
   elm_layout_content_set(efb->img_edje[2], "image", efb->images[2]);
   elm_layout_content_set(efb->layout, "center", efb->img_edje[2]);
   elm_thumb_file_set(efb->images[3], eina_list_data_get(next), NULL);
   elm_layout_content_set(efb->img_edje[3], "image", efb->images[3]);
   elm_layout_content_set(efb->layout, "right", efb->img_edje[3]);
   elm_thumb_file_set(efb->images[4], eina_list_data_get(nextt), NULL);
   elm_layout_content_set(efb->img_edje[4], "image", efb->images[4]);
   elm_layout_content_set(efb->layout, "offscreen_right", efb->img_edje[4]);
}

void
ephoto_flow_browser_del(void)
{
   int i;

   for (i = 0; i < 5; i++)
     evas_object_del(efb->images[i]);
   evas_object_del(efb->layout);
   evas_object_del(efb->toolbar);
   evas_object_del(efb->box);
   free(efb);
}

void
ephoto_flow_browser_show(void)
{
   ephoto->prev_state = ephoto->state;
   ephoto->state = EPHOTO_STATE_FLOW;

   ephoto_flow_browser_image_set();
   elm_pager_content_promote(ephoto->pager, ephoto->flow_browser);
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
   Eina_List *prevv, *prev, *next, *nextt;
   int i;

   prev = eina_list_prev(ephoto->current_index);
   if (!eina_list_data_get(prev))
     prev = eina_list_last(ephoto->images);
   prevv = eina_list_prev(prev);
   if (!eina_list_data_get(prevv))
     prevv = eina_list_last(ephoto->images);
   next = eina_list_next(ephoto->current_index);
   if (!eina_list_data_get(next))
     next = eina_list_nth_list(ephoto->images, 0);
   nextt = eina_list_next(next);
   if (!eina_list_data_get(nextt))
     nextt = eina_list_nth_list(ephoto->images, 0);

   edje = elm_layout_edje_get(efb->layout);
   edje_object_freeze(edje);

   for (i = 0; i < 5; i++)
     {
        elm_layout_content_unset(efb->layout, efb->swallows[i]);
        elm_layout_content_unset(efb->img_edje[i], "image");
     }
   if (efb->flow_direct == 0)
     {
        elm_thumb_file_set(efb->images[4], eina_list_data_get(nextt), NULL);
        elm_layout_content_set(efb->img_edje[4], "image", efb->images[4]);
        elm_layout_content_set(efb->layout, "offscreen_right", efb->img_edje[4]);
        elm_thumb_file_set(efb->images[3], eina_list_data_get(next), NULL);
        elm_layout_content_set(efb->img_edje[3], "image", efb->images[3]);
        elm_layout_content_set(efb->layout, "right", efb->img_edje[3]);
        elm_thumb_file_set(efb->images[2], eina_list_data_get(ephoto->current_index), NULL);
        elm_layout_content_set(efb->img_edje[2], "image", efb->images[2]);
        elm_layout_content_set(efb->layout, "center", efb->img_edje[2]);
        elm_thumb_file_set(efb->images[1], eina_list_data_get(prev), NULL);
        elm_layout_content_set(efb->img_edje[1], "image", efb->images[1]);
        elm_layout_content_set(efb->layout, "left", efb->img_edje[1]);
        elm_thumb_file_set(efb->images[0], eina_list_data_get(prevv), NULL);
        elm_layout_content_set(efb->img_edje[0], "image", efb->images[0]);
        elm_layout_content_set(efb->layout, "offscreen_left", efb->img_edje[0]);
     }
   else
     {
        elm_thumb_file_set(efb->images[0], eina_list_data_get(prevv), NULL);
        elm_layout_content_set(efb->img_edje[0], "image", efb->images[0]);
        elm_layout_content_set(efb->layout, "offscreen_left", efb->img_edje[0]);
        elm_thumb_file_set(efb->images[1], eina_list_data_get(prev), NULL);
        elm_layout_content_set(efb->img_edje[1], "image", efb->images[1]);
        elm_layout_content_set(efb->layout, "left", efb->img_edje[1]);
        elm_thumb_file_set(efb->images[2], eina_list_data_get(ephoto->current_index), NULL);
        elm_layout_content_set(efb->img_edje[2], "image", efb->images[2]);
        elm_layout_content_set(efb->layout, "center", efb->img_edje[2]);
        elm_thumb_file_set(efb->images[3], eina_list_data_get(next), NULL);
        elm_layout_content_set(efb->img_edje[3], "image", efb->images[3]);
        elm_layout_content_set(efb->layout, "right", efb->img_edje[3]);
        elm_thumb_file_set(efb->images[4], eina_list_data_get(nextt), NULL);
        elm_layout_content_set(efb->img_edje[4], "image", efb->images[4]);
        elm_layout_content_set(efb->layout, "offscreen_right", efb->img_edje[4]);
     }
   edje_object_signal_emit(edje, "reset", "ephoto");
   edje_object_thaw(edje);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_FALSE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_FALSE);
}

static void
_ephoto_flow_back(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_toolbar_item_selected_set
     (elm_toolbar_selected_item_get(efb->toolbar), EINA_FALSE);

   ephoto_thumb_browser_show();
}

static void 
_ephoto_flow_prev(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *edje;

   elm_toolbar_item_selected_set
     (elm_toolbar_selected_item_get(efb->toolbar), EINA_FALSE);

   ephoto->current_index = eina_list_prev(ephoto->current_index);
   if (!eina_list_data_get(ephoto->current_index))
     ephoto->current_index = eina_list_last(ephoto->images);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_TRUE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_TRUE);

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

   elm_toolbar_item_selected_set
     (elm_toolbar_selected_item_get(efb->toolbar), EINA_FALSE);

   ephoto->current_index = eina_list_next(ephoto->current_index);
   if (!ephoto->current_index)
     ephoto->current_index = eina_list_nth_list(ephoto->images, 0);

   elm_toolbar_item_disabled_set(efb->action.go_prev, EINA_TRUE);
   elm_toolbar_item_disabled_set(efb->action.go_next, EINA_TRUE);

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

