#include "ephoto.h"

static Evas_Object *_ephoto_slideshow_item_get(void *data, Evas_Object *obj);
static void _ephoto_mouse_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);

static Evas_Object *slideshow;
static const Elm_Slideshow_Item_Class _ephoto_item_cls = {{_ephoto_slideshow_item_get, NULL}};

Evas_Object *
ephoto_slideshow_add(void)
{
   slideshow = elm_slideshow_add(ephoto->win);
   elm_slideshow_layout_set(slideshow, "fullscreen");
   elm_slideshow_loop_set(slideshow, EINA_TRUE);
   elm_slideshow_transition_set(slideshow, "fade");
   elm_slideshow_timeout_set(slideshow, 3);
   evas_object_size_hint_weight_set
     (slideshow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(slideshow, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_event_callback_add
     (slideshow, EVAS_CALLBACK_MOUSE_DOWN, _ephoto_mouse_down, NULL);

   return slideshow;
}

void
ephoto_slideshow_del(void)
{
   evas_object_del(slideshow);
}

void
ephoto_slideshow_show(void)
{
   Eina_List *l;
   const char *image;

   ephoto->prev_state = ephoto->state;
   ephoto->state = EPHOTO_STATE_SLIDESHOW;
   elm_pager_content_promote(ephoto->pager, ephoto->slideshow);

   elm_slideshow_clear(slideshow);
   elm_win_fullscreen_set(ephoto->win, EINA_TRUE);
   EINA_LIST_FOREACH(ephoto->images, l, image)
     {
        Elm_Slideshow_Item *item;

        item = elm_slideshow_item_add(slideshow, &_ephoto_item_cls, image);
        if (l == ephoto->current_index)
          elm_slideshow_show(item);
     }
}

static 
Evas_Object *_ephoto_slideshow_item_get(void *data, Evas_Object *obj)
{
   const char *file = data;

   Evas_Object *image = elm_photo_add(obj);
   elm_photo_file_set(image, file);
   elm_photo_fill_inside_set(image, EINA_TRUE);
   elm_object_style_set(image, "shadow");

   return image;
}

static void
_ephoto_mouse_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_win_fullscreen_set(ephoto->win, EINA_FALSE);
   if (ephoto->prev_state == EPHOTO_STATE_THUMB)
     ephoto_thumb_browser_show();
   else if (ephoto->prev_state == EPHOTO_STATE_FLOW)
     ephoto_flow_browser_show();
}
