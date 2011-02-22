#include "ephoto.h"

static void _entry_free(void *data __UNUSED__, const Ephoto_Entry *entry __UNUSED__);
static Evas_Object *_ephoto_slideshow_item_get(void *data, Evas_Object *obj);
static void _ephoto_mouse_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);

typedef struct _Ephoto_Slideshow Ephoto_Slideshow;
struct _Ephoto_Slideshow
{
   Evas_Object *slideshow;
   Ephoto_Entry *entry;
};
static Ephoto_Slideshow *ss;

static const Elm_Slideshow_Item_Class _ephoto_item_cls = {{_ephoto_slideshow_item_get, NULL}};

Evas_Object *
ephoto_slideshow_add(void)
{
   ss = calloc(1, sizeof(Ephoto_Slideshow));

   ss->slideshow = elm_slideshow_add(ephoto->win);
   elm_slideshow_layout_set(ss->slideshow, "fullscreen");
   elm_slideshow_loop_set(ss->slideshow, EINA_TRUE);
   elm_slideshow_transition_set(ss->slideshow, "fade");
   elm_slideshow_timeout_set(ss->slideshow, 5);
   evas_object_size_hint_weight_set
     (ss->slideshow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ss->slideshow, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_event_callback_add
     (ss->slideshow, EVAS_CALLBACK_MOUSE_DOWN, _ephoto_mouse_down, NULL);

   return ss->slideshow;
}

void
ephoto_slideshow_del(void)
{
   if (ss->entry)
     ephoto_entry_free_listener_del(ss->entry, _entry_free, NULL);
   free(ss);
   evas_object_del(ss->slideshow);
}

void
ephoto_slideshow_entry_set(Ephoto_Entry *entry)
{
   Ephoto_Entry *itr;
   Eina_List *l; 

   if (ss->entry)
     ephoto_entry_free_listener_del(ss->entry, _entry_free, NULL);
   ss->entry = entry;

   if (entry)
     ephoto_entry_free_listener_add(entry, _entry_free, NULL);

   elm_slideshow_clear(ss->slideshow);
   if (!entry)
     return;
   elm_win_fullscreen_set(ephoto->win, EINA_TRUE);
   EINA_LIST_FOREACH(ephoto->entries, l, itr)
     {
        Elm_Slideshow_Item *item;

        item = elm_slideshow_item_add(ss->slideshow, &_ephoto_item_cls, itr);
        if (itr == entry)
          elm_slideshow_show(item);
     }
}

static void
_entry_free(void *data __UNUSED__, const Ephoto_Entry *entry __UNUSED__)
{
   ss->entry = NULL;
}

static 
Evas_Object *_ephoto_slideshow_item_get(void *data, Evas_Object *obj)
{
   Ephoto_Entry *entry = data;

   Evas_Object *image = elm_photo_add(obj);
   elm_photo_file_set(image, entry->path);
   elm_photo_fill_inside_set(image, EINA_TRUE);
   elm_object_style_set(image, "shadow");

   return image;
}

static void
_ephoto_mouse_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_win_fullscreen_set(ephoto->win, EINA_FALSE);
   if (ephoto->prev_state == EPHOTO_STATE_THUMB)
     ephoto_thumb_browser_show(ss->entry);
   else if (ephoto->prev_state == EPHOTO_STATE_FLOW)
     ephoto_flow_browser_show(ss->entry);
}
