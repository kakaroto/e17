#include "ephoto.h"

typedef struct _Ephoto_Slideshow Ephoto_Slideshow;

struct _Ephoto_Slideshow
{
   Ephoto *ephoto;
   Evas_Object *slideshow;
   Ephoto_Entry *entry;
};

static void
_key_down(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Slideshow *ss = data;
   Evas_Event_Key_Down *ev = event_info;
   const char *k = ev->keyname;

   if (!strcmp(k, "Escape"))
     {
        Evas_Object *win = ss->ephoto->win;
        Elm_Slideshow_Item *item;
        Ephoto_Entry *entry;

        if (elm_win_fullscreen_get(win))
          elm_win_fullscreen_set(win, EINA_FALSE);

        item = elm_slideshow_item_current_get(ss->slideshow);
        if (item) entry = elm_slideshow_item_data_get(item);
        else      entry = ss->entry;
        evas_object_smart_callback_call(ss->slideshow, "back", entry);
     }
   else if (!strcmp(k, "F11"))
     {
        Evas_Object *win = ss->ephoto->win;
        elm_win_fullscreen_set(win, !elm_win_fullscreen_get(win));
     }
}

static void
_mouse_down(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Slideshow *ss = data;
   evas_object_smart_callback_call(ss->slideshow, "back", ss->entry);
}

static void
_entry_free(void *data, const Ephoto_Entry *entry __UNUSED__)
{
   Ephoto_Slideshow *ss = data;
   ss->entry = NULL;
}

static void
_slideshow_del(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Slideshow *ss = data;
   if (ss->entry)
     ephoto_entry_free_listener_del(ss->entry, _entry_free, ss);
   free(ss);
}

Evas_Object *
ephoto_slideshow_add(Ephoto *ephoto, Evas_Object *parent)
{
   Evas_Object *slideshow = elm_slideshow_add(parent);
   Ephoto_Slideshow *ss;

   EINA_SAFETY_ON_NULL_RETURN_VAL(slideshow, NULL);

   ss = calloc(1, sizeof(Ephoto_Slideshow));
   EINA_SAFETY_ON_NULL_GOTO(ss, error);
   ss->ephoto = ephoto;
   ss->slideshow = slideshow;
   evas_object_event_callback_add
     (slideshow, EVAS_CALLBACK_DEL, _slideshow_del, ss);
   evas_object_event_callback_add
     (slideshow, EVAS_CALLBACK_KEY_DOWN, _key_down, ss);
   evas_object_event_callback_add
     (slideshow, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, ss);
   evas_object_data_set(slideshow, "slideshow", ss);

   elm_slideshow_loop_set(slideshow, EINA_TRUE);
   elm_slideshow_layout_set(slideshow, "fullscreen");
   evas_object_size_hint_weight_set
     (slideshow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(slideshow, EVAS_HINT_FILL, EVAS_HINT_FILL);

   return ss->slideshow;

 error:
   evas_object_del(slideshow);
   return NULL;
}

static void
_image_resized(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *image, void *event_info __UNUSED__)
{
   Evas_Map *map = evas_map_new(4);
   Evas_Coord cx, cy, x, y, w, h;
   int orient = (long)evas_object_data_get(image, "orient");

   evas_object_geometry_get(image, &x, &y, &w, &h);
   evas_map_util_points_populate_from_geometry(map, x, y, w, h, 0);
   cx = x + w / 2;
   cy = y + h / 2;

   switch (orient)
     {
      case EPHOTO_ORIENT_0:
         break;
      case EPHOTO_ORIENT_90:
         evas_map_util_rotate(map, 90.0, cx, cy);
         evas_object_map_enable_set(image, EINA_TRUE);
         evas_object_map_set(image, map);
         printf("rotated 90 around %d,%d (%dx%d)\n", cx, cy, w, h);
         break;
      case EPHOTO_ORIENT_180:
         evas_map_util_rotate(map, 180.0, cx, cy);
         evas_object_map_enable_set(image, EINA_TRUE);
         evas_object_map_set(image, map);
         printf("rotated 180 around %d,%d (%d,%d %dx%d)\n", cx, cy, x, y, w, h);
         break;
      case EPHOTO_ORIENT_270:
         evas_map_util_rotate(map, 270.0, cx, cy);
         evas_object_map_enable_set(image, EINA_TRUE);
         evas_object_map_set(image, map);
         printf("rotated 270 around %d,%d (%dx%d)\n", cx, cy, w, h);
         break;
      default:
         ERR("unknown orient %d", orient);
     }

   evas_map_free(map);
}

static Evas_Object *
_slideshow_item_get(void *data, Evas_Object *obj)
{
   Ephoto_Entry *entry = data;
   /* TODO use viewer from ephoto_single_browser.c */
   /* TODO consider using exif rotation, see ephoto_single_browser.c */
   Evas_Object *image = elm_photo_add(obj);
   elm_photo_file_set(image, entry->path);
   elm_photo_fill_inside_set(image, EINA_TRUE);
   elm_object_style_set(image, "shadow");

   evas_object_data_set
     (image, "orient", (void*)(long)ephoto_file_orient_get(entry->path));
   evas_object_event_callback_add
     (image, EVAS_CALLBACK_RESIZE, _image_resized, NULL);

   return image;
}

static const Elm_Slideshow_Item_Class _item_cls = {{_slideshow_item_get, NULL}};

void
ephoto_slideshow_entry_set(Evas_Object *obj, Ephoto_Entry *entry)
{
   Ephoto_Slideshow *ss = evas_object_data_get(obj, "slideshow");
   Ephoto_Config *conf;
   Ephoto_Entry *itr;
   const Eina_List *l;
   EINA_SAFETY_ON_NULL_RETURN(ss);

   conf = ss->ephoto->config;

   DBG("entry %p, was %p", entry, ss->entry);

   if (ss->entry)
     ephoto_entry_free_listener_del(ss->entry, _entry_free, ss);

   ss->entry = entry;

   if (entry)
     ephoto_entry_free_listener_add(entry, _entry_free, ss);

   elm_slideshow_loop_set(ss->slideshow, EINA_TRUE); /* move to config? */
   elm_slideshow_transition_set(ss->slideshow, conf->slideshow_transition);
   elm_slideshow_timeout_set(ss->slideshow, conf->slideshow_timeout);
   elm_slideshow_clear(ss->slideshow);
   if (!entry) return;

   elm_win_fullscreen_set(ss->ephoto->win, EINA_TRUE);
   EINA_LIST_FOREACH(ss->ephoto->entries, l, itr)
     {
        Elm_Slideshow_Item *item;
        if (itr->is_dir) continue;
        item = elm_slideshow_item_add(ss->slideshow, &_item_cls, itr);
        if (itr == entry) elm_slideshow_show(item);
     }
}
